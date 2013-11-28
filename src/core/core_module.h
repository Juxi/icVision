// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_CORE_MODULE_H_
#define _ICVISION_CORE_MODULE_H_

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;

#include "../icFilterModule/icImage.h"
#include "icFilterPort.h"
#include "module_info.h"

class ModuleInfo;
class ModuleRPCThread;

class CoreModule : public yarp::os::Module
{
	friend class ModuleRPCThread;
protected:
//	ConstString name;	
	
	int nextModuleID;
	
	// module parameters
	string moduleName;
	string robotName;
	string handlerPortName;
	string rpcPortName;
	string outputPortName;
	bool isRunning;
	/*port name strings*/

	
	// YARP interfaces
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)
	yarp::os::Port moduleOutputPort;//!< The output port of the module
	
	yarp::os::RpcServer rpcPort; 	//!< The port to handle RPC messages (e.g. quit)	
	
	// needed for the overlay of all filters
	BufferedPort<ImageOf<PixelBgr> > leftInPort;		//!< The port to handle incoming left eye images
//	BufferedPort<ImageOf<Pixelbgr> > rightInPort;		//!< The port to handle incoming right eye images
	
	// should that not be non buffered?! TODO
	BufferedPort< ImageOf<PixelBgr> > outputPort;	//!< The port to provide the output (all filters overlayed)
	
	IplImage *outputImageToWrite;	
	
	bool	isStarted;	
	
	// methods
	void addModuleToList(int ID, const Bottle &cmd);
	void remModuleFromList(int thisModuleID);	
	
	icFilterPort* createAndConnectPort(int ID, const char *in_port_name);
	
	//  = CV_RGB( 255, 0, 0) or	CvScalar color = CV_RGB( rand()%256, rand()%256, rand()%256);
	void drawBoxesAndCaption(icImage *output, icImage *filter, std::string capt, CvScalar color = CV_RGB(255,0,0));
	
	ModuleRPCThread *rpcThread;
public:
	// list of started modules	
	vector<ModuleInfo> listOfModules;
	
	
	CoreModule();
	virtual ~CoreModule();
	
	void printDebug(const char* str);
	void printWarning(const char* str);
	
	// Module functions
	double getPeriod();
	bool updateModule();

	bool open			(yarp::os::Searchable& config);
	bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool configure		(yarp::os::Searchable& config);
	bool interruptModule();
	bool close			();
	
//	//	void runModule	(int argc, char *argv[]);//, bool skipFirst = true);
//	bool respond	(const Bottle& command, Bottle& reply);
//	bool configure	(yarp::os::Searchable& config);
//	bool interruptModule();
//	bool close			();

	
};


/**
 * We need a separate thread for the RPC commands. The main thread can retrieve for instance camera images,
 * while the module listens in the meanwhile to RPC commands. The thread contains the RFModuleRPC as public
 * field and is able to access its field because it is declared as a friend class.
 */
class ModuleRPCThread: public yarp::os::Thread {
public:
	//! Pointer to the RFModule
	CoreModule *visionModule;
	
	//! Constructor requires pointer to a RFModuleRPC instance
	ModuleRPCThread(CoreModule *mVis) {
		visionModule = mVis;
	}
	
	//! Empty destructor
	inline ~ModuleRPCThread() {}
	
	//! Default "run" command for this Thread derived class
	void run() {
		while (isStopping() != true) {
			yarp::os::Bottle command, replay;
			
			// do a blocking read from the rpc port
			if (visionModule->rpcPort.read(command, true))
			{
				yarp::os::Bottle reply;
				// pass the command to the responder
				bool ok = visionModule->respond(command, reply);
				visionModule->rpcPort.reply(reply);
				if(!ok) stop();
			}
		}
	}
		
};

#endif
