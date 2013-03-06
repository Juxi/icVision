/*
 *  main.cpp
 *  Main class for the program
 *
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 */

#include <QApplication>

#include "window.h"
#include "iCub_controller.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>


int main(int argc, char *argv[]) {
	QString version = "v0.21";
	// first version on the real robot
	QString title = "LearningObjLocalization";

	QApplication app(argc, argv);

	srand ( time(NULL) );
	
	QThread ctrl_t;

	yarp::os::Property config;
        config.fromCommand(argc, argv);

	bool isSimulation = false;
        if ( !config.check("robot") )   // the name of the robot to connect to
        {
                printf("Please specify a robot name using '--robot yourRobotName'.\nNow using icubSim.\n");
		isSimulation = true;
        } else {
		if(strcmp(config.find("robot").asString().c_str(), "icubSim")) isSimulation = true;
	}


	// Objects
	iCubController ctrl(config.find("robot").asString().c_str(), isSimulation);
	ctrl.moveToThread(&ctrl_t);
	ctrl_t.start();
	
	// Main Window
	Window window(title, version, &ctrl);
	window.setFocusPolicy(Qt::StrongFocus);
	window.setupSignalsAndSlots();
	window.show();
	
	int ret = app.exec();	

	// cleanup
	ctrl_t.exit();
	return ret;
}
