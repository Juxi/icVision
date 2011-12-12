// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _IDSIA_IMCLEVER_ICFILTERMODULE_H_
#define _IDSIA_IMCLEVER_ICFILTERMODULE_H_

#include <string>
#include <cv.h>
//#include <highgui.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include "GpImage.h"
#include "boolList.h"

#define HEAD_JOINTS 6


using namespace yarp::os;
using namespace yarp::sig;

class icFilterModule : public yarp::os::Module {
	
protected:
	// module parameters
	std::string moduleName;
	std::string robotName;
	std::string handlerPortName;
		
	/*port name strings*/
	std::string outputPortName;
	std::string rawOutputPortName;
	
	
	// read one file from hdd
	bool isReadingFileFromHDD;
	std::string fileName;	
	
	// for debug and stats
	clock_t start, end;

	
	// YARP interfaces
	
	yarp::os::RpcClient port;
	
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)
	
	BufferedPort< ImageOf<PixelBgr> > leftInPort;		//!< The port to handle incoming left eye images
	BufferedPort< ImageOf<PixelBgr> > rightInPort; 	//!< The port to handle incoming right eye images

	BufferedPort< ImageOf<PixelBgr> > rawOutputPort;
	BufferedPort< ImageOf<PixelBgr> > outputPort_Image;
	
	bool	isStarted;
	bool	inDebugMode;
	bool	streamRawFilterOutput;
	bool	streamProcessedFilterOutput;	
	double  scalingFactor;
	
	int ImageWidth;
	int ImageHeight;
	
	
	// HACK
	yarp::os::Port head_port;
	yarp::os::Port torso_port;
	double headjnt_pos[HEAD_JOINTS], torsojnt_pos[3];
	
	bool runOnLeft;
	bool runOnRight;
	
	
public:
	// Constructor
	icFilterModule();
	
	// yarp::Module heritage
	double getPeriod();
	
	bool open	   (yarp::os::Searchable& config);
	bool configure (yarp::os::Searchable& config);
	
	
	// defining to run on both or one eye
	// TODO multi-thread this
	static const int LEFT_IMAGE = 1;
	static const int RIGHT_IMAGE = 0;
	void runOnOneImage(int leftOrRight);
	void runOnBothImages();	

	bool setWorldPositionOfObject(double x, double y, double z, const char *objName);
};

#endif