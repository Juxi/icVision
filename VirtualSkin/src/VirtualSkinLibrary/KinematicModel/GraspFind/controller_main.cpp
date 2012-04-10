#include <iostream>
#include <vector>
#include <cassert>
#include <string>

#include "yarpgraspcontroller.h"

using namespace std;

int main(int argc, char *argv[])
{
	bool visualize = true;
	QApplication app(argc, argv, visualize);	// create the QT application

	YarpGraspController yarp_grasp_controller(argc, argv);

	cout << "starting grasp thread" << endl;
	yarp_grasp_controller.start();

	cout << "starting app" << endl;
	int result = app.exec();

	cout << "stopping grasp thread" << endl;
	yarp_grasp_controller.stop();

	return result;
}
