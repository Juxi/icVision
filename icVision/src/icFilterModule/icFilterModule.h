// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICFILTERMODULE_H_
#define _ICFILTERMODULE_H_

#include <string>
#include <vector>
#include <cv.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include "icImage.h"

#define HEAD_JOINTS 6


using namespace yarp::os;
using namespace yarp::sig;

class icFilterModule : public yarp::os::Module {
	
protected:
	// module parameters
	std::string moduleName;
	std::string robotName;
	
	/*port name strings*/
	std::string handlerPortName;
	std::string outputPortName;
	std::string posOutputPortName;	
	std::string rawOutputPortName;
	std::string icVisionPortName;	//!< the name of the port running the icVision core
	

	std::string icVisionName;		//!< usually "icVision" for use in other ports

	// read one file from hdd
	bool isReadingFileFromHDD;
	std::string fileName;	
	
	// YARP interfaces	
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::RpcClient icVisionPort;//!< The port to handle RPC comunication with the core
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)

	// for ikin3d localization .. crap :0
//	BufferedPort<Vector> *portIKinIn;
//	BufferedPort<Bottle> *portIKinOut;
	
	
	BufferedPort< ImageOf<PixelBgr> > leftInPort;		//!< The port to handle incoming left eye images
	BufferedPort< ImageOf<PixelBgr> > rightInPort; 	//!< The port to handle incoming right eye images

	// output ports
	BufferedPort< ImageOf<PixelBgr> > rawOutputPort;
	BufferedPort< ImageOf<PixelBgr> > imgOutputPort;
	BufferedPort<Vector>			  posOutputPort;	
	
	
	
	// Variables	
	bool	isRunning;
	bool	inDebugMode;

	bool	shallNotifyingGazeCtrl;	
	bool	shallLocaliseInThreeD;
	
	bool	streamRawFilterOutput;
	bool	streamProcessedFilterOutput;	
	double  scalingFactor;
	
	int ImageWidth;
	int ImageHeight;
	
	bool runOnLeft;
	bool runOnRight;
	
	int moduleID;	//<! Received ID from the icVision core
	
	
	// Private Methods
	bool icVisionCoreIsAvailable();
	bool registerModuleWithCore();
	bool deregisterModuleWithCore();
	bool get3DPosition(Vector &v);
	//bool setWorldPositionOfObject(double x, double y, double z, const char *objName);
	
	void printDebug(const char* str);
	
public:
	// Constructor
	icFilterModule();
	
	// yarp::Module heritage
	double getPeriod();
	virtual bool open  (yarp::os::Searchable& config);
	virtual bool close ();	
	virtual bool configure (yarp::os::Searchable& config);
	virtual bool respond   (const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	virtual bool interrupt ();
	virtual bool updateModule();
	
	// defining to run on both or one eye
	// TODO multi-thread this
	static const int LEFT_IMAGE = 1;
	static const int RIGHT_IMAGE = 0;
	void runOnOneImage(int leftOrRight);
	void runOnBothImages();	
	
	// defining 3d localisation connection
	void localiseInThreeD(bool b) { 	shallLocaliseInThreeD = b;  }

	// heritage (change, maybe?)
	void useThisForGazeCtrl(bool b) {
		shallNotifyingGazeCtrl = b;
	}
	
	
};

#endif