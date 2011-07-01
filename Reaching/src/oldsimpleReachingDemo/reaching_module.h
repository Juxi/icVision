// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _REACHING_MODULE_H_
#define _REACHING_MODULE_H_

#include <string>
using namespace std;

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

#include "reaching.h"


class ReachingModule : public yarp::os::Module
{
private:
	// module parameters
	string moduleName;
	string robotName;
	string handlerPortName;

	/*port name strings*/
	string inputPortName;
	string outputPortName;
	
	// YARP interfaces
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::RpcClient port;		//!< The port we are wrapping
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)				
	yarp::os::Port moduleOutputPort;//!< The output port of the module
	
	
	string lastTgtObjName;

public:
	ReachingModule();
	virtual ~ReachingModule();
	
	ReachingWorker *reach;
	
	double getPeriod();
	bool updateModule();
	
	bool open			(yarp::os::Searchable& config);
	bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool configure		(yarp::os::Searchable& config);
	bool interruptModule();
	bool close			();
};

#endif
