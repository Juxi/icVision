#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <string>

#include "yarpgrasp.h"

using namespace KinematicModel;
using namespace std;

int main(int argc, char *argv[])
{
	assert(argc == 4);
	bool visualize = true;
	
	//vector<vector<float> > some_poses = read_poses("poses.save")["CFGSPACE"];

	QApplication app(argc, argv, visualize);	// create the QT application
	
	Model model( visualize, false );

	model.start();	/* if we want display lists to be created automatically,
					   the model must be started prior to appending objects */
	
	printf("loading robot file: %s\n", argv[1]);
	Robot &robot = *model.loadRobot(QString(argv[1]), false);

	printf("loading world file: %s\n", argv[2]);
	model.loadWorld( QString(argv[2]), false );

	string map_file(argv[3]);
	cout << "loading map file:" << map_file << endl;

	YarpGrasp yarp_grasp(model, robot, map_file);


	cout << "starting grasp thread" << endl;
	yarp_grasp.start();
	cout << "starting app" << endl;
	int result = app.exec();
	cout << "stopping grasp thread" << endl;
	yarp_grasp.stop();

	return result;
}
