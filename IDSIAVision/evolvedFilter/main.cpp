// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "evolved_filter_module.h"
#include "evolved_filters/blue_detector.h"
#include "evolved_filters/glass_detector.h"
#include "evolved_filters/hand_detector.h"
#include "evolved_filters/redbox_detector.h"
#include "evolved_filters/teabox.h"
#include "evolved_filters/icub_finger.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;
	
	if( ! yarp.checkNetwork() ) {
		std::cout << "I am afraid but I could not find a YARP server!" << std::endl;
		return 1;
	}

	std::string version = "v0.04";
	printf("Launching IM-CLeVeR Evolved Filter (%s) Module [vision]...\n", version.c_str());
	
//	EvolvedFilterModule* module = new RedBoxDetector();
//	EvolvedFilterModule* module = new GlassDetector();		
//	EvolvedFilterModule* module = new BlueCupDetector();	
//	EvolvedFilterModule* module = new EvolvedFilterModule();
	EvolvedFilterModule* module = new TeaBoxDetector();	
//	EvolvedFilterModule* module = new HandDetector();	
//	EvolvedFilterModule* module = new iCubFingerFilter();	

	// for reading from file
//	module->runOnOneImage(EvolvedFilterModule::RIGHT_IMAGE);
	module->runOnBothImages();
	module->putInVirtualSkin(false);
	module->notifyGazeCtrl(false);	
	
	/* run the module: runModule() calls configure first and, if successful, it then runs */
	module->runModule(argc, argv);
	module->close();
	
	delete module;
	
	return 0;
}


