// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "redFilterModule.h"
#include "testModule.h"

//#include "evolved_filter_module.h"
#include "../evolvedFilters/blue_detector.h"
//#include "evolved_filters/glass_detector.h"
#include "../evolvedFilters/redblock_detector.h"
//#include "evolved_filters/teabox.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;
	
	if( ! yarp.checkNetwork() ) {
		std::cout << "I am afraid but I could not find a YARP server!" << std::endl;
		return 1;
	}

	std::string version = "v0.1";
	printf("Launching icVision Test Module (%s)...\n", version.c_str());
	
	//TestModule *module = new TestModule();
	
	// TODO remove that, this is just testing
	RedBlockDetector *module = new RedBlockDetector();
	//BlueCupDetector *module = new BlueCupDetector();
	
//	RedFilterModule* module = new RedFilterModule();		
//	module->runOnBothImages();
	
//	module->runOnOneImage(EvolvedFilterModule::RIGHT_IMAGE);
	/* run the module: runModule() calls configure first and, if successful, it then runs */
	module->runModule(argc, argv);
	module->interrupt();
	module->close();
	
	delete module;
	
	return 0;
}


