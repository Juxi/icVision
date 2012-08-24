#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

#include "demothreads.h"

#include "poses_reader.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>

using namespace KinematicModel;
using namespace std;



int map_build_test(Model &model, Robot &robot, QApplication &app) {
	std::cout << "map build test" << std::endl;
	MapThread pose_thread(model, robot);

	pose_thread.start();

	int result = app.exec();						// run the Qt application

	pose_thread.stop();
	model.stop();

	return result;
}

/*
int path_test(Model &model, Robot &robot, QApplication &app) {
	PathThread path_thread(model, robot);

	path_thread.start();

	int result = app.exec();						// run the Qt application

	path_thread.stop();
	model.stop();

	return result;
}
*/

int online_test(Model &model, Robot &robot, QApplication &app) {
	OnlineThread online_thread(model, robot);

	online_thread.start();

	int result = app.exec();						// run the Qt application

	online_thread.stop();
	model.stop();

	return result;
}

int main(int argc, char *argv[])
{
	if (argc != 4) {
		cout << "not enough arguments" << endl;
		return 1;
	}
	bool visualize = true;
	
	//vector<vector<float> > some_poses = read_poses("poses.save")["CFGSPACE"];

	QApplication app(argc, argv, visualize);	// create the QT application
	
	Model model( visualize, false );
	model.setStopOnFirstCollision(true); // stop checking for collisions after the first collision in a pose
	model.start();	/* if we want display lists to be created automatically,
					   the model must be started prior to appending objects */
	
	printf("loading robot file: %s\n", argv[1]);
	Robot &robot = *model.loadRobot(QString(argv[1]), false);

	printf("loading world file: %s\n", argv[2]);
	model.loadWorld( QString(argv[2]), false );


	if (string(argv[3]) == "map") {
		return map_build_test(model, robot, app);
	}

	//if (string(argv[3]) == "path") {
	//	return path_test(model, robot, app);
	//}

	if (string(argv[3]) == "online") {
		return online_test(model, robot, app);
	}
}
