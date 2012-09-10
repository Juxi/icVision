#include "yarpposecontroller.h"
#include "util.h"
#include "exception.h"
#include "spline.hpp"

#include <iostream>
#include <vector>
#include <cassert>
#include <string>

#include "sprintf.h"
//
//#include "pathplanner.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::dev;

//using namespace KinematicModel;
using namespace std;

void YarpPoseController::load_config(int argc, char **argv) {
	Property command_config;
	command_config.fromCommand(argc, argv);
	Property file_config;

	string file_name;
	Bottle settings;
	if (command_config.check("file")) {
		file_name = command_config.find("file").asString();
		file_config.fromConfigFile(file_name.c_str());
		settings = file_config.findGroup("SETTINGS");
	} else
		throw StringException("no init file");

	string robot_file = "";
	if ( settings.check("robot") )  { robot_file = settings.find("robot").asString().c_str(); }
	if ( command_config.check("robot") )  { robot_file = command_config.find("robot").asString().c_str(); }

	string world_file = "";
	if ( settings.check("world") )  { world_file = settings.find("world").asString().c_str(); }
	if ( command_config.check("world") )  { world_file = command_config.find("world").asString().c_str(); }

	string map_file = "";
	if ( settings.check("map") )  { map_file = settings.find("map").asString().c_str(); }
	if ( command_config.check("map") )  { map_file = command_config.find("map").asString().c_str(); }

	if ( settings.check("mover_port") )  { d_mover_portname = settings.find("mover_port").asString().c_str(); }
	if ( command_config.check("mover_port") )  { d_mover_portname = command_config.find("mover_port").asString().c_str(); }

	if ( settings.check("port") )  { d_portname = settings.find("port").asString().c_str(); }
	if ( command_config.check("port") )  { d_portname = command_config.find("port").asString().c_str(); }

	if (!robot_file.size()) {
		throw StringException("No robot file specified");
	}

	bool visualize(true);
	if (d_model) {
		delete d_model;
		d_model = 0;
	}
	d_model = new VirtualSkin::YarpModel( visualize );
	d_model->setStopOnFirstCollision(true); // we can stop at the first collision


	d_model->start(); /* if we want display lists to be created automatically,
					  // the model must be started prior to appending objects */
	d_model->openWorldRpcPort(QString("/poseControl/world"));

	//KinematicModel::Robot &robot = *d_model->loadRobot(QString(robot_file.c_str()), false);
	d_robot = d_model->loadRobot(QString(robot_file.c_str()), false);

	if (world_file.size()) {
		d_model->loadWorld( QString(world_file.c_str()), false );
	}

	d_model->openSimSyncer(QString("/poseControl/simSync"), QString("/icubSim/world"));

	load_path_planner(*d_model, *d_robot);
	if (map_file.size())
		d_path_planner->load_map("default", map_file);
	d_model->stop();

}

void YarpPoseController::load_path_planner(KinematicModel::Model& model, KinematicModel::Robot& robot) {
	cout << "loading path planner" << endl;
	if (d_path_planner) {
		delete d_path_planner;
		d_path_planner = 0;
	}
	d_path_planner = new PathPlanner(model, robot);
	cout << "loaded " << endl;
}

Bottle YarpPoseController::path_to_bottle(vector<vector<vector<double> > > &path) {
	Bottle root;
	for (int i = 0; i < path.size(); i++) {
		Bottle node_l1;
		for (int j=0; j < path[i].size(); j++) {
			Bottle node_l2;
			for (int k=0; k < path[i][j].size(); k++)
				node_l2.addDouble(path[i][j][k]);
			node_l1.addList() = node_l2;
		}
		root.addList() = node_l1;
	}
	return root;
}

void YarpPoseController::follow_path(Roadmap::path_t &path) {
	if (path.path.size() == 0 || path.length > 1000000)
		throw StringException("No Path Found");
	vector<vector<vector<double> > > crazy_path;
	for (size_t i(0); i < path.path.size(); ++i) {
		vector<double> pose(d_path_planner->getVertex(path.path[i]).q);
		//std::cout << pose.size() << std::endl;
		vector<vector<double> > cut_pose = d_path_planner->cut_pose(pose);

		crazy_path.push_back(cut_pose);
	}
	Bottle path_bottle = path_to_bottle(crazy_path);
	Bottle command;
	command.addString("go");
	command.addList() = path_bottle;
	Bottle response;
	cout << "writing path" << endl;
	d_mover.write(command, response);
	cout << response.toString() << endl;
	cout << "done" << endl;
}

void YarpPoseController::follow_interpolated_path(Roadmap::path_t &path) {
	int nposes = path.path.size();
	vector<vector<vector<double> > > crazy_path;
	const TreeMode dist_mode = MAXCONFIGURATIONSPACE;

	if (nposes == 0 || path.length > 1000000)
		throw StringException("No Path Found");

	if (nposes < 3) { // we need at least two poses for interpolation
		for (int ipose(0); ipose<nposes; ipose++)
			crazy_path.push_back(d_path_planner->cut_pose(d_path_planner->getVertex(path.path[ipose]).q));
	} else {

		int naxes = d_path_planner->getVertex(path.path.front()).q.size();
		vector<magnet::math::Spline> splines(naxes, magnet::math::Spline());
		double cumDist = 0.0; // cumulative distance
		vector<double> tmpDist(naxes, 0.0);
		
		for (size_t ipose(0); ipose < nposes; ++ipose) {
			Roadmap::Vertex second = d_path_planner->getVertex(path.path[ipose]);
			
			if (ipose > 0) {
				Roadmap::Vertex first = d_path_planner->getVertex(path.path[ipose-1]);
				
				switch (dist_mode) {
				case CONFIGURATIONSPACE: 	// take the rmse distance between the current and the previous pose
					cumDist = cumDist + calculate_distance(first.qs, second.qs);
					break;
				case MAXCONFIGURATIONSPACE:
					//take the maximum change between the current and the previous pose
					for (size_t iax(0); iax < naxes; iax++) {
						tmpDist[iax] = fabs(first.qs[iax] - second.qs[iax]);
					}
					cumDist = cumDist + *(std::max_element(tmpDist.begin(), tmpDist.end()));
					break;
				case WORKSPACE:
					//take the distance between current and previous end-effector pose
					cumDist = cumDist + calculate_distance( first.x, second.x);
					break;
				default:
					throw StringException("invalid distance mode");
					break;
				}
			}

			for (size_t iax(0); iax < naxes; iax++) {
				splines[iax].addPoint(cumDist, second.q[iax]);
			}
		}

		// create regularly-spaced vector
		int nsteps; // cumDist * stepFactor;
		switch (dist_mode) {
		case CONFIGURATIONSPACE: 	// take the rmse distance between the current and the previous pose
			nsteps = cumDist * 5; // 5 interpolation steps per degree of rmse movement
			break;
		case MAXCONFIGURATIONSPACE:
			//take the maximum change between the current and the previous pose
			nsteps = cumDist * 10; // 10 interpolation steps per degree of fastest-moving joint
			break;
		case WORKSPACE:
			//take the distance between current and previous end-effector pose
			nsteps = cumDist * 1000; // 1000 interpolation step per meter workspace change (i.e. one step per mm)
			break;
		default:
			throw StringException("invalid distance mode");
			break;
		}
		double stepsize = 1.0/(double)(nsteps - 1);
		vector<double> regDistances(nsteps, 0.0);
		for (int i(0); i<nsteps; i++) { 
			regDistances[i] = i*stepsize*cumDist;
		}

		// interpolate
		vector<vector<double > > interpolated(nsteps, vector<double>(naxes, 0.0));
		for (size_t istep(0); istep < nsteps; istep++) {
			for (size_t iax(0); iax < naxes; iax++) {
				interpolated[istep][iax] = splines[iax](regDistances[istep]);
			}
			vector<vector<double> > cut_pose = d_path_planner->cut_pose(interpolated[istep]);
			crazy_path.push_back(cut_pose);
		}
		cout << "cumulative distance: " << cumDist << " number of interpolated steps: " << nsteps << endl;		
	}


	// send crazy_path
	Bottle path_bottle = path_to_bottle(crazy_path);
	Bottle command;
	command.addString("go");
	command.addList() = path_bottle;
	Bottle response;
	cout << "writing path" << endl;
	d_mover.write(command, response);
	cout << response.toString() << endl;
	cout << "done" << endl;
}


vector<double> YarpPoseController::get_current_pose() {
	Bottle command;
	command.addString("get");
	command.addString("pose");
	Bottle response;
	cout << "getting pose" << endl;
	d_mover.write(command, response);
	cout << response.toString() << endl;

	vector<double> result_vector = bottle_to_vector(response.get(0));
	cout << "resulting pose vector: " << endl;
	print_vector(result_vector);
	return result_vector;
}

void YarpPoseController::run () {
	if (!d_port.open(d_portname.c_str()))
		throw StringException("Couldnt open rpc Server");
	if (!d_mover.open("/poseControl/to_mover"))
		throw StringException("Couldnt open rpc mover Client");
	if (!d_mover.addOutput(d_mover_portname.c_str()))
		throw StringException("Couldnt connect to port of mover");
	//	if (!d_yarp.connect("/blaat", d_mover_portname.c_str()))
	//			throw StringException("Couldnt connect the ports");

	while (true) {
		cout << "Waiting for a message..." << endl;
		Bottle query;
		Bottle response;
		d_port.read(query,true);

		int command = query.get(0).asVocab();

		char *help_message = "Possible Commands:\n"
			"load [name] [file]\t--\tload map in [file] under name [name]\n"
			"con [n]\t--\tconnect all maps with n neirest neighbours\n"
			"ran\t--\tshow all ranges\n"
			"ran [name]\t--\tshow range of mape [name]\n"
			"go [name] [workspace]\t--\tmove to point [workspace] of map [name]\n"
			"go [name]\t--\tmove to closest position on map [name]\n"
			"try [name] [workspace]\t--\treturn goal workspace and path distance of a move to point [workspace] of map [name]\n"
			"try [name]\t--\treturn goal workspace and path distance of a move to closest position on map [name]\n"
			"wrt [filename]\t--\twrite the map edges into file [filename] (format source and target workspace of an edge per line\n"
			"wrt [filename] [name]\t--\twrite the map edges of map [name] into file [filename] (format source and target workspace of an edge per line\n"
			"clr\t--\tremove all maps\n"
			"help\t--\tshow this help message\n"
			"info\t--\tshow info about all maps\n";

		std::string info;

		try {
			switch ( command ) {

			case VOCAB_GO:
				if (query.size() == 2 && query.get(1).isString()) { //go [name]

					vector<double> source_q = get_current_pose();
					vector<double> source_sq = d_path_planner->scale_q(source_q);
					string mapname = query.get(1).asString().c_str();

					Roadmap::vertex_t source_v = d_path_planner->nearestMainMapVertex(source_sq, SCALEDCONFIGURATIONSPACE);
					Roadmap::vertex_t target_v = d_path_planner->nearestMainMapVertex(source_sq, SCALEDCONFIGURATIONSPACE, mapname);

					Roadmap::path_t path = d_path_planner->find_path(source_v, target_v);
					follow_path(path);
				} else
					if (query.size() == 3 && query.get(1).isString() && query.get(2).isList()) { //go [name] [workspace]
						
						vector<double> source_q = get_current_pose();
						vector<double> source_sq = d_path_planner->scale_q(source_q);
						string mapname = query.get(1).asString().c_str();
						vector<double> target_x = bottle_to_vector(query.get(2));
						
						Roadmap::vertex_t source_v = d_path_planner->nearestMainMapVertex(source_sq, SCALEDCONFIGURATIONSPACE);
						Roadmap::vertex_t target_v = d_path_planner->nearestMainMapVertex(target_x, WORKSPACE, mapname);
						vector<double> target_q = d_path_planner->getVertex(target_v).q;

						if (source_q.size() != target_q.size())
							throw StringException(Sprintf("source (from mover) and target vector size dont match: ", source_q.size(), target_q.size()));

						cout << "robot:" << endl;
						print_vector(source_q);
						cout << "source:" << endl;
						print_vector(d_path_planner->getVertex(source_v).q);
						cout << "target workspace:" << endl;
						print_vector(target_x);
						cout << "target configuration space:" << endl;
						print_vector(target_q);
						cout << "finding path:" << endl;

						if (source_q.size() != target_q.size()) {
							throw StringException(Sprintf("source (from mover) and target vector size dont match: ", source_q.size(), target_q.size()));
						}

						Roadmap::path_t path = d_path_planner->find_path(source_v, target_v);
						//follow_path(path);
						follow_interpolated_path(path);
					} else
						throw StringException("Wrong arguments in command");
					break;

			case VOCAB_TRY:
				if (query.size() == 2 && query.get(1).isString()) { //go [name]

					vector<double> source_q = get_current_pose();
					vector<double> source_sq = d_path_planner->scale_q(source_q);
					string mapname = query.get(1).asString().c_str();
					
					
					Roadmap::vertex_t source_v = d_path_planner->nearestMainMapVertex(source_sq, SCALEDCONFIGURATIONSPACE);
					Roadmap::vertex_t target_v = d_path_planner->nearestMainMapVertex(source_sq, SCALEDCONFIGURATIONSPACE, mapname);
					Roadmap::path_t path = d_path_planner->find_path(source_v, target_v);

					Bottle bgoal;
					Roadmap::Vertex vgoal = d_path_planner->getVertex(path.path.back());
					for (size_t i(0); i < vgoal.x.size(); ++i)
						bgoal.addDouble(vgoal.x[i]);
					response.addList() = bgoal;
					response.addDouble(path.length);

					//path = d_path_planner->move_to_path(source_q, target_q);
					//follow_path(path);
				} else
					if (query.size() == 3 && query.get(1).isString() && query.get(2).isList()) { //go [name] [workspace]
						vector<double> source_q = get_current_pose();
						vector<double> source_sq = d_path_planner->scale_q(source_q);
						string mapname = query.get(1).asString().c_str();
						vector<double> target_x = bottle_to_vector(query.get(2));
						
						Roadmap::vertex_t source_v = d_path_planner->nearestMainMapVertex(source_sq, SCALEDCONFIGURATIONSPACE);
						Roadmap::vertex_t target_v = d_path_planner->nearestMainMapVertex(target_x, WORKSPACE, mapname);
						vector<double> target_q = d_path_planner->getVertex(target_v).q;

						if (source_q.size() != target_q.size())
							throw StringException(Sprintf("source (from mover) and target vector size dont match: ", source_q.size(), target_q.size()));

						Roadmap::path_t path = d_path_planner->find_path(source_v, target_v);

						Bottle bgoal;
						Roadmap::Vertex vgoal = d_path_planner->getVertex(path.path.back());
						for (size_t i(0); i < vgoal.x.size(); ++i)
							bgoal.addDouble(vgoal.x[i]);
						response.addList() = bgoal;
						response.addDouble(path.length);

						//follow_path(path);
					} else
						throw StringException("Wrong arguments in command");
					break;

			case VOCAB_HELP:
				if (query.size() == 1) { //help
					response.addVocab(Vocab::encode("many"));
					response.addString(help_message);
				} else
					throw StringException("Wrong arguments in command");
				break;

			case VOCAB_LOAD:
				if (query.size() == 3 && query.get(1).isString() && query.get(2).isString()) {//load [name] [file]
					std::string map_name(query.get(1).asString().c_str());
					std::string map_file(query.get(2).asString().c_str());
					d_path_planner->load_map(map_name, map_file);
				} else
					throw StringException("Wrong arguments in command");

				break;

			case VOCAB_CLEAR:
				if (query.size() == 1) { //clr
					load_path_planner(*d_model, *d_robot);
				} else
					throw StringException("Wrong arguments in command");
				break;

			case VOCAB_CONNECT:
				if (query.size() == 2 && query.get(1).isInt()) { //con [n]
					int number = query.get(1).asInt();
					d_path_planner->connect_map(number, SCALEDCONFIGURATIONSPACE);
				} else
					throw StringException("Wrong arguments in command");
				/*
				if (query.size() == 3 && query.get(1).isString() && query.get(2).isInt()) { //con [name] [n]
				std::string map_name(query.get(1).asString().c_str());
				int n = query.get(2).asInt();
				if (n < 1)
				throw StringException("n is too small or negative");
				d_path_planner->connect_map(map_name, n);
				}
				*/
				break;

			case VOCAB4('c', 'o', 'n','2'):
				if (query.size() == 2 && query.get(1).isInt()) { //con [n]
					int number = query.get(1).asInt();
					d_path_planner->connect_map2(number, SCALEDCONFIGURATIONSPACE);
				} else
					throw StringException("Wrong arguments in command");
				break;

			case VOCAB_GET_RANGE:
				if (query.size() == 1) {//ran
					response.addVocab(Vocab::encode("many"));
					response.addString(d_path_planner->range_strings().c_str());
				} else

					if (query.size() == 2 && query.get(1).isString()) {//ran [name]
						std::string map_name(query.get(1).asString().c_str());
						response.addVocab(Vocab::encode("many"));
						response.addString(d_path_planner->range_string(map_name).c_str());
					} else
						throw StringException("Wrong arguments in command");
					break;

			case VOCAB_INFO:  //info
				if (query.size() == 1) { //info
					info = d_path_planner->info();
					response.addVocab(Vocab::encode("many"));
					response.addString(info.c_str());
				} else
					throw StringException("Wrong arguments in command");
				break;

			case VOCAB_WRITE:  //info
				if (query.size() == 2 && query.get(1).isString()) // wrt
					d_path_planner->write_graph(query.get(1).asString().c_str());
				else if (query.size() == 3 && query.get(1).isString() && query.get(2).isString())//wrt [name]
					d_path_planner->write_graph(query.get(2).asString().c_str(), query.get(1).asString().c_str());
				else
					throw StringException("Wrong arguments in command");
				break;

			default:
				throw StringException("Not a recognized command");
			}
		}
		catch (StringException &error) {
			response.addString("FAIL");
			response.addString(error.what());
		}
		response.addString("==READY==");
		d_port.reply(response);
	}
}



Bottle YarpPoseController::poses_to_bottle(vector<vector<double> > &path) {
	Bottle b;
	//	b.clear();
	for (size_t p(0); p < path.size(); ++p) {
		vector<double> &pose(path[p]);
		Bottle pose_bottle;
		for (size_t n(0); n < pose.size(); n++) {
			pose_bottle.addInt((int)pose[n]);
		}
		b.addList() = pose_bottle;
	}
	return b;
}

std::vector<double> YarpPoseController::bottle_to_vector(yarp::os::Value &val) {
	std::vector<double> values;
	if (!val.isList()) { throw StringException("Value not a list"); }
	Bottle* val_pointer = val.asList();

	for (size_t i(0); i < val_pointer->size(); ++i) {
		if (val_pointer->get(i).isList()) {
			Bottle* val2_pointer = val_pointer->get(i).asList();
			for (size_t i(0); i < val2_pointer->size(); ++i)
				values.push_back(val2_pointer->get(i).asDouble());
		} else
			values.push_back(val_pointer->get(i).asDouble());
	}
	return values;
}


