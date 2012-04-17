// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ThreeD_MODULE_H_
#define _ThreeD_MODULE_H_

#define LIST_LENGTH 50
#include <string>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/all.h>

#include "iCub_controller.h"
//#include "../core/icFilterModule.h"
//#include "../core/icEvolvedFilterModule.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

class DataProcessor;

class ThreeDModule : public Module {
private:
	std::string icVisionPortName;		//!< usually "icVision" for use in other ports
	std::string posPortName;	
	
	// YARP interfaces	
	Port posPort;//!< The port to handle localising request
	iCubController *iCubCtrl;
	
	// more stuff
	int		headIdx;
	Bottle  headState[LIST_LENGTH];
	Stamp   headStamp[LIST_LENGTH];

	int		torsoIdx;
	Bottle  torsoState[LIST_LENGTH];
	Stamp   torsoStamp[LIST_LENGTH];
	
	// methods
	bool readEncoders();
	bool checkTS(double TSLeft, double TSRight, double th=0.08);	
	
	
	DataProcessor *processor;
	
public:
	ThreeDModule(iCubController *);
	virtual ~ThreeDModule();

	virtual bool updateModule();
	bool configure		(yarp::os::Searchable& config);
	Bottle& calculatePosition(Bottle &b);
	
};

class DataProcessor : public PortReader {
	
	virtual bool read(ConnectionReader& connection) {
		Bottle in, out;
		bool ok = in.read(connection);
		if (!ok) return false;
		
		// process data "in", prepare "out"
		out.append(in);
		out.addList() = module->calculatePosition(in);
		
		std::cout << "DataProcessor: " << in.toString() << std::endl;
		
		// reply
		ConnectionWriter *returnToSender = connection.getWriter();
		if (returnToSender!=NULL) {
			out.write(*returnToSender);
		}
		return true;
	}
	
	ThreeDModule *module;

public:
	DataProcessor(ThreeDModule *mod) {
		module = mod;
	}
	
};




#endif
