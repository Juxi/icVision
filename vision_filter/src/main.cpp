/*
 *  main.cpp
 *  Main class for the program
 *
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 */

#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>

#include <yarp/os/all.h>	
#include <yarp/sig/all.h>

#include "VisionModule.h"

using namespace yarp::os;


int main(int argc, char * argv[]) {
	
	Network yarp; // set up yarp
	
	std::string version = "0.01";
	std::string title = "Vision Filter";
	
	VisionFilterModule module;
//	module.setTitle(title);
//	module.setVersion(version);	

	std::cout << "Launching " + title + " (" + version + ")" << std::endl;
	
	
	// run the module:
	// runModule() calls configure first and, if successful, runs the module
	module.runModule(argc, argv);
	module.close();
	
	return 0;
	
}