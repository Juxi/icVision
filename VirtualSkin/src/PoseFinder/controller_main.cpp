#include <iostream>
#include <vector>
#include <cassert>
#include <string>

#include "yarpposecontroller.h"

using namespace std;

int main(int argc, char *argv[])
{
	bool visualize = true;
	QApplication app(argc, argv, visualize);	// create the QT application

	YarpPoseController yarp_pose_controller(argc, argv);

	cout << "starting pose thread" << endl;
	yarp_pose_controller.start();

	cout << "starting app" << endl;
	int result = app.exec();

	cout << "stopping pose thread" << endl;
	yarp_pose_controller.stop();

	return result;
}
