// Copyright: (C) 2012-2013 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _SIMULATED_ROBOT_H_
#define _SIMULATED_ROBOT_H_

#include <string>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

class icSimulatedRobot : public yarp::os::Module { //: public icFilterModule {
private:

public:
	icSimulatedRobot();
	virtual ~icSimulatedRobot();
    
    
    ////////////////////////////////////
    // module parameters
	string robotName;
	string outputPortName;
    
    string fileName;
    
//	bool runOnLeft;
//	bool runOnRight;
//  void runOnBothImages();
    
    //	bool isRunning;
    
    
    ////////////////////////////////////
    // YARP interfaces
	yarp::os::Network yarp;			//!< Identifies the yarp network
	yarp::os::Port handlerPort; 	//!< The port to handle messages (e.g. quit)
	yarp::os::Port moduleOutputPort;//!< The output port of the module
	
	
	// needed for the overlay of all filters
	BufferedPort<ImageOf<PixelBgr> > leftOutPort;		//!< The port to handle the left eye images
    BufferedPort<ImageOf<PixelBgr> > rightOutPort;		//!< The port to handle the right eye images


    ////////////////////////////////////
    bool open(yarp::os::Searchable& config);
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool configure(yarp::os::Searchable& config);
    bool interrupt();
    bool close();
	double getPeriod();
	virtual bool updateModule();
    
    
    

};

#endif
