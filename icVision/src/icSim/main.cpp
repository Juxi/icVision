// Copyright: (C) 2012-2013 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "simulatedRobot.h"

using namespace yarp::os;
//using namespace yarp::sig;

int main(int argc, char * argv[]) {
    Network::init();
	
	if( ! Network::checkNetwork() ) {
		std::cout << "I am afraid but I could not find a YARP server!" << std::endl;
		return 1;
	}

	std::string version = "v0.1";
	printf("Launching icVision Simulated Robot (%s)...\n", version.c_str());
	
	icSimulatedRobot module;


	/* run the module: runModule() calls configure first and, 
	   if successful, it then runs the module*/
	module.runModule(argc, argv);

	//module.interrupt();
	module.close();
	
	return 0;
}


