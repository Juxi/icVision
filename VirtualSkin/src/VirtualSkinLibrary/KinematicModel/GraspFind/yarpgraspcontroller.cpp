#include "yarpgraspcontroller.h"

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

void YarpGraspController::load_config(int argc, char **argv) {
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
    cout << "map file: " << map_file << endl;

    if ( settings.check("port") )  { d_portname = settings.find("port").asString().c_str(); }
    if ( command_config.check("port") )  { d_portname = command_config.find("port").asString().c_str(); }

    if ( settings.check("mover_port") )  { d_mover_portname = settings.find("mover_port").asString().c_str(); }
    if ( command_config.check("mover_port") )  { d_mover_portname = command_config.find("mover_port").asString().c_str(); }

    if (!robot_file.size() || !world_file.size() || !map_file.size() || !d_portname.size()) {
    	cout << robot_file << endl << world_file << endl << map_file << endl << d_portname << endl;
    	throw StringException("One of the files/names not defined [robot/world/map/port]");
    }

    bool visualize(true);
    if (d_model) {
    	delete d_model;
    	d_model = 0;
    }
	d_model = new Model( visualize, false );
	Model &model(*d_model);

	model.start();	/* if we want display lists to be created automatically,
					   the model must be started prior to appending objects */

	Robot &robot = *model.loadRobot(QString(robot_file.c_str()), false);
    d_portname = "/" + robot.getName().toStdString() + "/" + d_portname; // the name of the RPC server

	printf("loading world file: %s\n", argv[2]);
	model.loadWorld( QString(world_file.c_str()), false );

    load_path_planner(model, robot, map_file);

}

void YarpGraspController::load_path_planner(KinematicModel::Model& model, KinematicModel::Robot& robot, std::string map_file) {
	cout << "loading path planner" << endl;
	if (d_path_planner) {
		delete d_path_planner;
		d_path_planner = 0;
	}
	d_path_planner = new PathPlanner(model, robot, map_file);
	cout << "loaded " << endl;
}

Bottle YarpGraspController::path_to_bottle(vector<vector<vector<double> > > &path) {
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

void YarpGraspController::follow_path(vector<vector<double> > &path) {
	vector<vector<vector<double> > > crazy_path;
	for (size_t i(0); i < path.size(); ++i) {
		vector<double> pose(path[i]);
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

void YarpGraspController::run () {
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

		vector<vector<double> > path;
		vector<double> source, target;
		try {
			switch ( command ) {
				case VOCAB_GO:
					cout << "GO command, size: " << query.size() << endl;
					if (!(query.size() >= 2)) {
						response.addString("FAIL: not enough parameters");
						break;
					}

					cout << "getting source" << endl;
					source = bottle_to_vector(query.get(1));
					cout << "getting target" << endl;
					target = bottle_to_vector(query.get(2));
					cout << "planning path" << endl;
					print_vector(source);
					print_vector(target);
					path = d_path_planner->find_workspace_path(source, target);
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
						response.addString("iCub Path Planner: \n");
						response.addString("go: ((source) (target)).\n");
						break;
				case VOCAB_LOAD:
					if (query.size() >= 2 && query.get(1).isString()) {
						std::string map_file = query.get(1).asString().c_str();
						d_path_planner->load_map(map_file);
						response.addString("OK");
					} else
						response.addString("FAIL");
					break;
				case VOCAB_CONNECT:
					if (query.size() >= 2 && query.get(1).isInt()) {
						int number = query.get(1).asInt();
						d_path_planner->connect_map(number);
						response.addString("OK");
					} else
						response.addString("FAIL");
					break;
				case VOCAB_UPDATE:
					cout << "planner ptr: " << d_path_planner << endl;
					d_path_planner->update_map();
					response.addString("OK");
					break;
				default:
					response.addString("OK");
					response.addString("Unknown Command. Type help for more information.");
					break;
			}
		}
		catch (StringException &error) {
			response.addString("FAIL");
			response.addString(error.what());
		}
		d_port.reply(response);
	}
}

//		cout << "yarp grasp running" << endl;
//		if (!i_cub.open("icubSimF"))
//			throw StringException("Couldnt open port to icubsimf");
//
//		load_map(d_map_file);
//		connect_map(5);
//		add_bullshit();
//		update_map();
//		vector<vector<double> > the_path = find_path();
//	//
//		size_t counter(0);
//
//		while (true) {
//			vector<double> q(the_path[counter]);
//			vector<double> q_norm = d_graspfinder.simulator().real_to_normal_motors(q);
//			for (size_t i(0); i < q.size(); ++i)
//				cout << q_norm[i] << " ";
//			cout << endl;
//			d_graspfinder.simulator().set_motors(q_norm);
//			cout << "N COLLISIONS: " << d_graspfinder.simulator().computePose() << endl;
//
//			int velocity(40);
//			i_cub.setVelocity( velocity );
//			if (!i_cub.positionMove(q))
//				cout << "Position move error" << endl;
//			bool motionInterrupted = !waitForMotion();
//
//			msleep(500);
//			counter = (counter + 1) % the_path.size();
//		}

void YarpGraspController::plan_path(vector<double> workspace_source, vector<double> workspace_goal){
	vector<vector<double> > path = d_path_planner->find_workspace_path(workspace_source, workspace_goal);
}

Bottle YarpGraspController::poses_to_bottle(vector<vector<double> > &path) {
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

std::vector<double> YarpGraspController::bottle_to_vector(yarp::os::Value &val) {
	std::vector<double> values;
    if (!val.isList()) { throw StringException("Value not a list"); }
    Bottle* val_pointer = val.asList();

    for (size_t i(0); i < val_pointer->size(); ++i)
    	values.push_back(val_pointer->get(i).asDouble());
    return values;
}

