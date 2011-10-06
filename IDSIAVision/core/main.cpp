// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "vision_module.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	Network yarp;

	VisionModule module;

	printf("Launching IM-CLeVeR Vision Module ...\n");

	/* run the module: runModule() calls configure first and, if successful, it then runs */
	module.runModule(argc, argv);
	module.close();
	
	return 0;
}


