#ifndef __YARP_GRASP_CONTROLLER__
#define __YARP_GRASP_CONTROLLER__

#include "pathplanner.h"
#include "exception.h"
#include <QApplication>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

int const VOCAB_GO = VOCAB2('g','o');        // run trajectory
int const VOCAB_HELP = VOCAB4('h','e','l','p');      // display help
int const VOCAB_LOAD = VOCAB4('l','o','a','d'); //load map
int const VOCAB_CONNECT = VOCAB3('c', 'o', 'n');
int const VOCAB_UPDATE = VOCAB2('u', 'p');


class YarpGraspController :  public QThread {
	Q_OBJECT
	PathPlanner *d_path_planner;

	yarp::os::Network d_yarp;
    yarp::os::RpcServer d_port;
    yarp::os::RpcClient d_mover;

    std::string d_portname, d_mover_portname;

public:
	YarpGraspController(int argc, char **argv) : d_path_planner(0)
	{
		load_config(argc, argv);
	}

	~YarpGraspController() {
		if (d_path_planner)
			delete d_path_planner;
	}

	void load_path_planner(KinematicModel::Model& model, KinematicModel::Robot& robot, std::string map_file);

	void load_config(int argc, char **argv);

	void follow_path(std::vector<std::vector<double> > &path);
	yarp::os::Bottle path_to_bottle(std::vector<std::vector<std::vector<double> > > &path);

	void run ();

	void stop() {
		std::cout << "yarp grasp stopping (stopping thread)" << std::endl;
	}

	yarp::os::Bottle poses_to_bottle(std::vector<std::vector<double> > &path);

	yarp::os::Bottle vector_to_bottle(std::vector<double> vec);
	std::vector<double> bottle_to_vector(yarp::os::Value &val);

	void plan_path(std::vector<double> workspace_source, std::vector<double> workspace_goal);

};

#endif
