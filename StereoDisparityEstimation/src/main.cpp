/* main.cpp
 *
 * Author: Juxi Leitner
 * 2007-2012, ISR/IST, IDSIA
 * Last update: 2012-Apr-20
 */

#include "stdafx.h"

#include <string>
#include <vector>
#include <cv.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include "DepthYARP.h"

int main(int argc, char* argv[])
{
	yarp::os::Network net;
	
	// high performance schedulding (on some OSes)
	// http://eris.liralab.it/yarpdoc/classyarp_1_1os_1_1Time.html#ad33aea9d32219844c36675e2a8462af2
	yarp::os::Time::turboBoost();

	// create the Depth Perception algorithm module
	DepthYARP module;
	module.setName("/DepthPerception");
	
	// run the module
	return module.runModule(argc, argv);
}
