#ifndef __YARP_POSE_CONTROLLER__
#define __YARP_POSE_CONTROLLER__

#include "yarprobot.h"
#include "yarpModel.h"

#include "pathplanner.h"
#include "exception.h"
#include <QApplication>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

int const VOCAB_GO = VOCAB2('g','o');        // run trajectory
int const VOCAB_HELP = VOCAB4('h','e','l','p');      // display help
int const VOCAB_LOAD = VOCAB4('l','o','a','d'); //load map
int const VOCAB_CONNECT = VOCAB3('c', 'o', 'n'); //connect map
int const VOCAB_UPDATE = VOCAB2('u', 'p');
int const VOCAB_GET_RANGE = VOCAB3('r', 'a', 'n');
int const VOCAB_INFO = VOCAB4('i', 'n', 'f', 'o');
int const VOCAB_CLEAR = VOCAB3('c', 'l', 'r');
int const VOCAB_TRY = VOCAB3('t', 'r', 'y');


class YarpPoseController :  public QThread {
	Q_OBJECT
	PathPlanner *d_path_planner;
	VirtualSkin::YarpModel *d_model;
	KinematicModel::Robot *d_robot;

	yarp::os::Network d_yarp;
    yarp::os::RpcServer d_port;
    yarp::os::RpcClient d_mover;

    std::string d_mover_portname;
	std::string d_portname;

public:
	YarpPoseController(int argc, char **argv) : d_path_planner(0), d_model(0)
	{
		load_config(argc, argv);
	}

	~YarpPoseController() {
		if (d_path_planner)
			delete d_path_planner;
	}

	void load_path_planner(KinematicModel::Model& model, KinematicModel::Robot& robot);

	void load_config(int argc, char **argv);

	void follow_path(PathPlanner::path_t &path);
	yarp::os::Bottle path_to_bottle(std::vector<std::vector<std::vector<double> > > &path);

	void run ();

	void stop() {
		std::cout << "yarp grasp stopping (stopping thread)" << std::endl;
	}

	yarp::os::Bottle poses_to_bottle(std::vector<std::vector<double> > &path);

	yarp::os::Bottle vector_to_bottle(std::vector<double> vec);
	std::vector<double> bottle_to_vector(yarp::os::Value &val);

	std::vector<double> get_current_pose();

	
};

#endif
