/* main.cpp
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-10
 * Defines the entry point for the console application.
 *
 */

#include "stdafx.h"

int _tmain(int argc, char* argv[])
{
	// high performance schedulding (on some OSes)
	// http://eris.liralab.it/yarpdoc/classyarp_1_1os_1_1Time.html#ad33aea9d32219844c36675e2a8462af2
	yarp::os::Time::turboBoost();

	// create the Depth Perception algorithm module
	DepthYARP module;
	module.setName("/DepthPerception");
	
	// run the module
	return module.runModule(argc, argv);
}
