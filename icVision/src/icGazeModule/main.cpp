// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "GazeModule.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;
	
	if( ! yarp.checkNetwork() ) {
		std::cout << "I am afraid but I could not find a YARP server!" << std::endl;
		return 1;
	}

	std::string version = "v0.2";
	printf("Launching icVision Gaze Control Module (%s)...\n", version.c_str());
	
    // iCubController ctrl; // default connects to iCub
    // ctrl.initializeRobot();
	GazeModule *module = new GazeModule();
	/* run the module: runModule() calls configure first and, if successful, it then runs */
	module->runModule(argc, argv);
	module->close();
	
	delete module;
	
	return 0;
}


