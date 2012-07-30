#include "yarpposecontroller.h"

#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "util.h"
#include "exception.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <string>
//
//#include "pathplanner.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::dev;

using namespace KinematicModel;
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

    string robot_file = ""; // default is simulator
    if ( settings.check("robot") )  { robot_file = settings.find("robot").asString().c_str(); }
    if ( command_config.check("robot") )  { robot_file = command_config.find("robot").asString().c_str(); }

    string world_file = ""; // default is simulator
    if ( settings.check("world") )  { world_file = settings.find("world").asString().c_str(); }
    if ( command_config.check("world") )  { world_file = command_config.find("world").asString().c_str(); }

    string map_file = ""; // default is simulator
    if ( settings.check("map") )  { map_file = settings.find("map").asString().c_str(); }
    if ( command_config.check("map") )  { map_file = command_config.find("map").asString().c_str(); }

    if ( settings.check("port") )  { d_portname = settings.find("port").asString().c_str(); }
    if ( command_config.check("port") )  { d_portname = command_config.find("port").asString().c_str(); }

    if ( settings.check("mover_port") )  { d_mover_portname = settings.find("mover_port").asString().c_str(); }
    if ( command_config.check("mover_port") )  { d_mover_portname = command_config.find("mover_port").asString().c_str(); }

    if (!robot_file.size() || !world_file.size() || !d_portname.size()) {
    	cout << robot_file << endl << world_file << endl << map_file << endl << d_portname << endl;
    	throw StringException("One of the files/names not defined [robot/world/port]");
    }

    bool visualize(true);
    if (d_model) {
    	delete d_model;
    	d_model = 0;
    }
	d_model = new Model( visualize, false );
	Model &model(*d_model);

	model.start();	/* if we want display lists to be created automatically,
					  // the model must be started prior to appending objects */

	Robot &robot = *model.loadRobot(QString(robot_file.c_str()), false);
    d_portname = "/" + robot.getName().toStdString() + "/" + d_portname; // the name of the RPC server

	printf("loading world file: %s\n", argv[2]);
	model.loadWorld( QString(world_file.c_str()), false );

    load_path_planner(model, robot);
	if (map_file.size())
	  d_path_planner->load_map("default", map_file);
    model.stop();

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

void YarpPoseController::follow_path(vector<vector<double> > &path) {
  
	vector<vector<vector<double> > > crazy_path;
	for (size_t i(0); i < path.size(); ++i) {
		vector<double> pose(path[i]);
		std::cout << pose.size() << std::endl;
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
	cout << "Opening port: " << d_portname << endl;
	if (!d_port.open(d_portname.c_str()))
		throw StringException("Couldnt open rpc Server");
	if (!d_mover.open("/to_mover"))
		throw StringException("Couldnt open rpc mover Client");
	if (!d_mover.addOutput(d_mover_portname.c_str()))
		throw StringException("Couldnt connect the ports");
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
		  "con [name] [n]\t--\tconnect map [name] with n neirest neighbours\n"
		  "ran\t--\tshow all ranges\n"
		  "ran [name]\t--\tshow range of mape [name]\n"
		  "go [name] [workspace]\t--\tmove to point [workspace] of map [name]\n"
		  "go [name]\t--\tmove to closest position on map [name]\n"
		  "help\t--\tshow this help message\n"
		  "info\t--\tshow info about all maps\n";

		try {
		  switch ( command ) {
		  case VOCAB_GO:
			if (query.size() == 2 && query.get(1).isString()) { //go [name]
			  vector<vector<double> > path;
			  vector<double> source_conf = get_current_pose();
			  string mapname = query.get(1).asString().c_str();
			  vector<double> target_conf = d_path_planner->closest_configurationspace(mapname, source_conf);

			  path = d_path_planner->move_to_path(source_conf, target_conf);
			  follow_path(path);
			}

			if (query.size() == 3 && query.get(1).isString() && query.get(2).isList()) { //go [name] [workspace]
			  vector<vector<double> > path;

			  vector<double> source_conf = get_current_pose();
			  string mapname = query.get(1).asString().c_str();
			  vector<double> target_work = bottle_to_vector(query.get(2));
			  vector<double> target_conf = d_path_planner->closest_workspace(mapname, target_work);

			  cout << "source:" << endl;
			  print_vector(source_conf);
			  cout << "target:" << endl;
			  print_vector(target_work);
			  cout << "finding path:" << endl;

			  path = d_path_planner->find_path(source_conf, target_conf);
			  follow_path(path);
			}
			break;

		  case VOCAB_HELP:
			if (query.size() == 1) { //help
			  response.addVocab(Vocab::encode("many"));
			  response.addString(help_message);
			}
			break;

		  case VOCAB_LOAD:
			if (query.size() == 3 && query.get(1).isString() && query.get(2).isString()) {//load [name] [file]
			  std::string map_name(query.get(1).asString().c_str());
			  std::string map_file(query.get(2).asString().c_str());
			  d_path_planner->load_map(map_name, map_file);
			}
			break;

		  case VOCAB_CONNECT:
			if (query.size() == 2 && query.get(1).isInt()) { //con [n]
			  int number = query.get(1).asInt();
			  d_path_planner->connect_maps(number);
			}
			if (query.size() == 3 && query.get(1).isString() && query.get(2).isInt()) { //con [name] [n]
			  throw StringException("Not Implemented");
			}
			break;

		  case VOCAB_GET_RANGE:
			if (query.size() == 1) //ran
			  throw StringException("Not Implemented");
			if (query.size() == 2 && query.get(1).isString()) //ran [name]
			  throw StringException("Not Implemented");
			break;

		  case VOCAB_INFO:  //info
			if (query.size() == 1) //info
			  throw StringException("Not Implemented");
			break;
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


	/*


					cout << "GO command, size: " << query.size() << endl;
					if (!(query.size() >= 2)) {
						response.addString("FAIL: not enough parameters");
						break;
						}

					if (query.size() == 2) {
					cout << "size==2 get source from mover" << endl;
					source = get_current_pose();
						cout << "target::" << endl;
						target = bottle_to_vector(query.get(1));
						cout << "target::" << endl;
						print_vector(target);

						cout << "finding path:" << endl;

						assert(source.size() && target.size());
						cout << "finding path:" << endl;
						path = d_path_planner->find_configuration_workspace_path(source, target);
						cout << "done:" << endl;

					} else if (query.size() == 3 && query.get(1).isList()) {
						cout << "size==3, first is list" << endl;
						cout << "getting source" << endl;
						source = bottle_to_vector(query.get(1));
						target = bottle_to_vector(query.get(2));
						cout << "source:" << endl;
						print_vector(source);
						cout << "target:" << endl;
						print_vector(target);
						cout << "finding path:" << endl;
						path = d_path_planner->find_workspace_path(source, target);
					} else if (query.size() == 3 && query.get(1).isString()) {
						cout << "size==3 source, first is string" << endl;
						cout << "getting source" << endl;
						source = get_current_pose();
						target = bottle_to_vector(query.get(2));
						cout << "source:" << endl;
						print_vector(source);
						cout << "target:" << endl;
						print_vector(target);
						cout << "finding path:" << endl;
						string mapname(query.get(1).asString().c_str());
						if (!d_path_planner->hasMap(mapname))
						  throw StringException("Map doesnt exist");
						path = d_path_planner->find_configuration_workspace_path(source, target, mapname);
					} else if (query.size() == 4 && query.get(1).isString()) {
					 	cout << "size==3 source to target" << endl;
						cout << "getting source" << endl;
						source = bottle_to_vector(query.get(2));
						target = bottle_to_vector(query.get(3));
						cout << "source:" << endl;
						print_vector(source);
						cout << "target:" << endl;
						print_vector(target);
						cout << "finding path:" << endl;
						string mapname(query.get(1).asString().c_str());
						if (!d_path_planner->hasMap(mapname))
						  throw StringException("Map doesnt exist");
						path = d_path_planner->find_workspace_path(source, target, mapname);
					}

					cout << path.size() << endl;
					response.addString("path found");
					
					follow_path(path);
					response.addString("OK");

		//                        success           = (query.size() >= 1)  ?  mover.parseTrajBottle(query.get(1), thisTraj)  :  false;
		//                        thisDistThreshold = (query.size() <= 2)  ?  distThreshold  :  query.get(2).asDouble();
		//                        thisStepTimeout   = (query.size() <= 3)  ?  stepTimeout  :  query.get(3).asDouble();
		//                        success = success && mover.go(thisTraj, thisDistThreshold, thisStepTimeout);
		//                        if (success)
		//                                response.addString("OK");
		//                        else
		//                                response.addString("FAIL");
					break;
				case VOCAB_HELP:

				cout << "HELP command" << endl;
		//                        response.addVocab(Vocab::encode("many"));
						response.addVocab(Vocab::encode("many"));
						response.addString("iCub Path Planner:\n"
											"plan: go (source) (target)\n"
											"      go mapname (source) (target)\n"
											"load map: load map_name map_file \n"
											"connect map: con {n nearest neighbours}\n"
											"update edge weights: up\n"
											"get workspace range: ran {map_name}");
						break;
				case VOCAB_LOAD:
				  if (query.size() >= 3 && query.get(1).isString() && query.get(2).isString()) {
				        std::string map_name = query.get(1).asString().c_str();
						std::string map_file = query.get(2).asString().c_str();
						if (d_path_planner->load_map(map_name, map_file)) {
							response.addString("OK");
						} else {
							response.addString("FAIL");
						}
					} else
						response.addString("FAIL");
					break;
				case VOCAB_CONNECT:
					if (query.size() >= 2 && query.get(1).isInt()) {
					    int number = query.get(1).asInt();
						d_path_planner->connect_maps(number);
						response.addString("OK");
					} else
						response.addString("FAIL");
					break;
				case VOCAB_UPDATE:
					cout << "planner ptr: " << d_path_planner << endl;
					d_path_planner->update_maps();
					response.addString("OK");
					break;
				case VOCAB_GET_RANGE:
				{
				  std::string map_name("default");
				  if (query.size() >= 2 && query.get(1).isString())
					map_name = query.get(1).asString().c_str();
					if (d_path_planner->hasMap(map_name)) {

						ostringstream oss("range:");
						try {
							cout << "Calculating range of map [" << map_name << "]" << endl;
							pair<vector<float>, vector<float> > bbox = d_path_planner->roadmap(map_name).get_workspace_bounding_box();
					  
							oss << endl;
							oss << "[";
							for (size_t i(0); i < bbox.first.size(); ++i)
								oss << bbox.first[i] << " ";
							oss << "] [";
							for (size_t i(0); i < bbox.second.size(); ++i)
								oss << bbox.second[i] << " ";
							oss << "]" << endl;
							}
						catch (...) {
							oss << "Failed" << endl;
						}
						response.addVocab(Vocab::encode("many"));
						response.addString(oss.str().c_str());
					} else
						response.addString("FAIL");
						response.addString("map not found");
					break;
				}
				default:
					response.addString("OK");
					response.addString("Unknown Command. Type help for more information.");
					break;
			}
	*/


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

    cout << "before loop " << endl;
    for (size_t i(0); i < val_pointer->size(); ++i) {
    	if (val_pointer->get(i).isList()) {
    		Bottle* val2_pointer = val_pointer->get(i).asList();
    		for (size_t i(0); i < val2_pointer->size(); ++i)
        		values.push_back(val2_pointer->get(i).asDouble());
    	} else
    		values.push_back(val_pointer->get(i).asDouble());
    }
    cout << "after loop " << endl;
    return values;
}

