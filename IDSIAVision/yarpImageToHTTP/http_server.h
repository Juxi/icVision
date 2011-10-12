// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _REDLOCATOR_MODULE_H_
#define _REDLOCATOR_MODULE_H_

#include <string>
using namespace std;

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;

class RedLocatorModule : public yarp::os::Module
{
private:
	// module parameters
	string moduleName;
	string robotName;
	string partName;	
	string handlerPortName;

	/*port name strings*/
	string outputPortName;
	
	// YARP interfaces
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)
	
	BufferedPort<ImageOf<PixelRgb> > leftInPort;		//!< The port to handle incoming left eye images
//	BufferedPort<ImageOf<PixelRgb> > rightInPort; 	//!< The port to handle incoming right eye images
	
	BufferedPort<Vector> outputPort_ImagePosition;
	
	bool	isStarted;	
	
public:
	RedLocatorModule();
	virtual ~RedLocatorModule();
	
	double getPeriod();
	bool updateModule();
	
	bool open			(yarp::os::Searchable& config);
	bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool configure		(yarp::os::Searchable& config);
	bool interruptModule();
	bool close			();
	
};

#endif
