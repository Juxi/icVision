// Copyright: (C) 2011-2013 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "core_module.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;

	//icVision::CoreModule
	CoreModule module;
    string version = "1.0";

	printf("Launching icVision Core Module (v%s) ...\n", version.c_str());

	module.runModule(argc, argv);
	module.close();
	
	return 0;
}


