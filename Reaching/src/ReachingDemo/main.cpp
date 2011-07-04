// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdio>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/CartesianControl.h>

// for iKinCartesian
YARP_DECLARE_DEVICES(icubmod)

#include "reaching_module.h"

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[]) {
	// for iKinCartesian
    YARP_REGISTER_DEVICES(icubmod)

	Network yarp;
	ReachingModule module;

	printf("Launching IM-CLeVeR Reaching Module ...\n");
	/* run the module: runModule() calls configure first and, if successful, it then runs */
	module.runModule(argc, argv);
	module.close();
	
	return 0;
}


