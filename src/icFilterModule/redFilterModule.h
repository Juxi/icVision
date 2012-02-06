// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _REDLOCATOR_MODULE_H_
#define _REDLOCATOR_MODULE_H_

#include <string>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "icFilterModule.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

class RedFilterModule : public icFilterModule {
private:
	BufferedPort<Vector> outputPort_ImagePosition;
	
public:
	RedFilterModule();
	virtual ~RedFilterModule();
	
	// TODO check this on the iCub/Sim
	virtual bool updateModule();
		
//	bool open			(yarp::os::Searchable& config);
//	bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool configure		(yarp::os::Searchable& config);
//	bool interruptModule();
//	bool close			();
	
};

#endif
