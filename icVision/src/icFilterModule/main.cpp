// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <iostream>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "redFilterModule.h"
#include "testModule.h"

#include "../evolvedFilters/blue_detector.h"
//#include "evolved_filters/glass_detector.h"
#include "../evolvedFilters/redblock_detector.h"



using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;
	
	if( ! yarp.checkNetwork() ) {
		std::cout << "I am afraid but I could not find a YARP server!" << std::endl;
		return 1;
	}

	std::string version = "v0.6";
	printf("Launching icVision Test Module (%s)...\n\t", version.c_str());
	printf("using OpenCV %d.%d!\n\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);
	
	
	
//	RedBlockDetector *module = new RedBlockDetector();
//	TeaBoxDetector *module = new TeaBoxDetector();	
//	module->runOnOneImage(icFilterModule::LEFT_IMAGE);
//	module->localiseInThreeD(false);

	//RedFilterModule* module = new RedFilterModule();		

	
	// for localisation
	BlueCupDetector *module = new BlueCupDetector();	
	module->runOnBothImages();
	module->localiseInThreeD(true);
	
	//	module->runOnOneImage(icFilterModule::LEFT_IMAGE);

	// TODO create possibility
	// module->useThisForGazeCtrl(true);
	
//	module->runOnOneImage(EvolvedFilterModule::RIGHT_IMAGE);
	/* run the module: runModule() calls configure first and, if successful, it then runs */
	
//	FaceDetector *module = new FaceDetector();
//	module->runOnOneImage(EvolvedFilterModule::LEFT_IMAGE);

	
	
//	MarsDetector *module = new MarsDetector();
//	module->runOnOneImage(EvolvedFilterModule::LEFT_IMAGE);

	module->runModule(argc, argv);
	module->interrupt();
	module->close();
	
	delete module;
	
	return 0;
}


