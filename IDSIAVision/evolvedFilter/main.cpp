// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "evolved_filter_module.h"
#include "evolved_filters/blue_detector.h"
#include "evolved_filters/redbox_detector.h"
#include "evolved_filters/teabox.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;

	std::string version = "v0.02";
	printf("Launching IM-CLeVeR Evolved Filter (%s) Module [vision]...\n", version.c_str());
	
	EvolvedFilterModule* module = new RedBoxDetector();
//	EvolvedFilterModule* module = new BlueCupDetector();	
//	EvolvedFilterModule* module = new EvolvedFilterModule();	
//	EvolvedFilterModule* module = new TeaBoxDetector();	
	
	/* run the module: runModule() calls configure first and, if successful, it then runs */
	module->runModule(argc, argv);
	module->close();
	
	delete module;
	
	return 0;
}


