// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _EVOLVEDFILTERMODULE_MODULE_H_
#define _EVOLVEDFILTERMODULE_MODULE_H_

#include <string>
using namespace std;

#include <cv.h>
#include <highgui.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include "GpImage.h"
#include "boolList.h"

using namespace yarp::os;
using namespace yarp::sig;

class EvolvedFilterModule : public yarp::os::Module
{
protected:
	// module parameters
	string moduleName;
	string robotName;
	string partName;	
	string handlerPortName;
	
	// read one file from hdd
	string fileName;	
	bool isReadingFileFromHDD;

	/*port name strings*/
	string outputPortName;
	
	// YARP interfaces
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)
	
	BufferedPort< ImageOf<PixelBgr> > leftInPort;		//!< The port to handle incoming left eye images
//	BufferedPort< ImageOf<PixelBgr> > rightInPort; 	//!< The port to handle incoming right eye images
	
	BufferedPort< ImageOf<PixelBgr> > outputPort_Image;
	
	bool	isStarted;	
	bool	inDebugMode;
	double  scalingFactor;
	
	int ImageWidth;
	int ImageHeight;
	
	IplImage* gray;
	IplImage* red ;
	IplImage* green;
	IplImage* blue;
	IplImage* h   ;
	IplImage* s   ;
	IplImage* v   ;
	
	
	boolList UsedInputs;
	virtual void setUsedInputs();	
	
	std::vector<GpImage*> InputImages;
	void createInputImages(IplImage *in);
	
public:
	EvolvedFilterModule();
	virtual ~EvolvedFilterModule();
	
	double getPeriod();
	bool updateModule();
	
	virtual GpImage* runFilter();
	
	bool open			(yarp::os::Searchable& config);
	bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool configure		(yarp::os::Searchable& config);
	bool interruptModule();
	bool close			();
	
};

#endif
