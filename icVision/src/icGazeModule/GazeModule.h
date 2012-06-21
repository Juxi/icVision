// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _GAZE_MODULE_H_
#define _GAZE_MODULE_H_

#include <string>
#include <yarp/os/all.h>
//#include <yarp/os/Bottle.h>
//#include <yarp/os/Stamp.h>
#include <yarp/sig/all.h>

//#include "../core/icFilterModule.h"
//#include "../core/icEvolvedFilterModule.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

//class ModuleRPCThread;

class GazeModule : public Module {
	friend class ModuleRPCThread;

private:
	std::string icVisionPortName;		//!< usually "icVision" for use in other ports
	std::string gazePortName;	
	std::string filterPortName, filterServerName;

	// YARP interfaces	
	yarp::os::Network yarp;			//!< Identifies the yarp network
	//RpcClient gazePort;                  //!< The port to connect to iKinGazeCtrl
	BufferedPort<Bottle > gazePort;
	BufferedPort<Bottle > filterPort;
	
	
//	ModuleRPCThread *rpcThread;
	
	
//	Semaphore mutex;
	
	double m_gazePeriod;			//!< defines the refresh period of the module
	
	// methods
	bool checkTS(double TSLeft, double TSRight, double th=0.08);	
	// bool respond(const yarp::os::Bottle& in, yarp::os::Bottle& out, yarp::os::Stamp stamp);
	
	int prev[4];
	
public:
	GazeModule();
	virtual ~GazeModule();

	virtual bool updateModule();
	virtual double getPeriod();
	virtual bool configure		(yarp::os::Searchable& config);
	virtual bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	virtual bool interrupt ();	
	
};


// /**
//  * We need a separate thread for the RPC commands. The main thread can retrieve for instance camera images,
//  * while the module listens in the meanwhile to RPC commands. The thread contains the RFModuleRPC as public
//  * field and is able to access its field because it is declared as a friend class.
//  */
// class ModuleRPCThread : public yarp::os::Thread {
// public:
//  //! Pointer to the RFModule
//  ThreeDModule *module;
//  
//  //! Constructor requires pointer to a RFModuleRPC instance
//  ModuleRPCThread(ThreeDModule *mVis) {
//      module = mVis;
//  }
//  
//  //! Empty destructor
//  inline ~ModuleRPCThread() {}
//  
//  //! Default "run" command for this Thread derived class
//  void run() {
//      while (isStopping() != true) {
//          yarp::os::Bottle command, replay;
//          
//          // do a blocking read from the rpc port
//          if (module->posPort.read(command, true))
//          {
//              yarp::os::Bottle reply;
//              yarp::os::Stamp stamp;
//              module->posPort.getEnvelope(stamp);
//              // pass the command to the responder
//              bool ok = module->respond(command, reply, stamp);
//              module->posPort.reply(reply);
//              if(!ok) stop();
//          }
//      }
//  }
//  
// };



#endif
