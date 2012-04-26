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
class ModuleRPCThread;

class ThreeDModule : public Module {
	friend class ModuleRPCThread;
private:
	std::string icVisionPortName;		//!< usually "icVision" for use in other ports
	std::string posPortName;	
	
	ModuleRPCThread *rpcThread;
	
	// YARP interfaces	
	RpcServer posPort;//!< The port to handle localising request
	iCubController *iCubCtrl;
	
	// more stuff
	int		headIdx;
	Bottle  headState[LIST_LENGTH];
	Stamp   headStamp[LIST_LENGTH];

	int		torsoIdx;
	Bottle  torsoState[LIST_LENGTH];
	Stamp   torsoStamp[LIST_LENGTH];
	
	Semaphore mutex;
	
	// methods
	bool readEncoders();
	bool checkTS(double TSLeft, double TSRight, double th=0.08);	
	
	bool respond(const yarp::os::Bottle& in, yarp::os::Bottle& out, yarp::os::Stamp stamp);
	
	// HACK 
	void calcuatePositionUsingSimonsMethod(double *,double *,double *, int, int, int, int, Stamp);
	void getEncoderPositions(double *headjnt_pos, double *torsojnt_pos, Stamp stamp);
	
	DataProcessor *processor;
	
public:
	ThreeDModule(iCubController *);
	virtual ~ThreeDModule();

	virtual bool updateModule();
	bool configure		(yarp::os::Searchable& config);
	Bottle& calculatePosition(const Bottle &b, Stamp &stamp);
	
};

class DataProcessor : public PortReader {
	
	virtual bool read(ConnectionReader& connection);
	
	ThreeDModule *module;

public:
	DataProcessor(ThreeDModule *mod) {
		module = mod;
	}
	
};


/**
 * We need a separate thread for the RPC commands. The main thread can retrieve for instance camera images,
 * while the module listens in the meanwhile to RPC commands. The thread contains the RFModuleRPC as public
 * field and is able to access its field because it is declared as a friend class.
 */
class ModuleRPCThread : public yarp::os::Thread {
public:
	//! Pointer to the RFModule
	ThreeDModule *module;
	
	//! Constructor requires pointer to a RFModuleRPC instance
	ModuleRPCThread(ThreeDModule *mVis) {
		module = mVis;
	}
	
	//! Empty destructor
	inline ~ModuleRPCThread() {}
	
	//! Default "run" command for this Thread derived class
	void run() {
		while (isStopping() != true) {
			yarp::os::Bottle command, replay;
			
			// do a blocking read from the rpc port
			if (module->posPort.read(command, true))
			{
				yarp::os::Bottle reply;
				yarp::os::Stamp stamp;
				module->posPort.getEnvelope(stamp);
				// pass the command to the responder
				bool ok = module->respond(command, reply, stamp);
				module->posPort.reply(reply);
				if(!ok) stop();
			}
		}
	}
	
};



#endif
