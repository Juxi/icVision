// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _REACHING_H_
#define _REACHING_H_

#include <iostream> 
#include <cstdlib>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>

class ReachingWorker {
public:
	ReachingWorker(std::string robotName, std::string partName);
	~ReachingWorker();
	
	void setPosition(double, double, double);
	void setPosition(yarp::sig::Vector p);
	yarp::sig::Vector getPosition();			// the position set to be reached
	
	yarp::sig::Vector getCurrentPosition();		// the position from the ikin
	
	
	void setPolicy(int p);
	void setOffset(double o);

	void reachPosition();
	bool waitForMotionDone(double period, double timeout);
	void stopMotion();
	
	bool isActive();	
	bool isMotionDone();	

	
	// policies (maybe extend later)
	static const int FROM_ABOVE = 0x01;
	static const int FROM_LEFT	= 0x02;
	static const int FROM_RIGHT = 0x04;	
	static const int FROM_BELOW = 0x08;

	static const int STRAIGHT = 0x10;
	static const int CURVED	  = 0x20;
	
protected:
	bool initialized;
	bool reachActive;
	
	double defined_offset;

	int policy;
	
	yarp::dev::PolyDriver client;
	yarp::dev::ICartesianControl *arm;
	
	yarp::sig::Vector xd;
	yarp::sig::Vector orientationFromAbove, orientationFromSide; // orientation vectors;
	
    int startup_context_id;
	
    double t;
    double t0;
    double t1;
	
	void init();
	yarp::sig::Vector calculatePreReachPosition();		
	bool goToPreReach();
	bool doReaching();	

	void callGraspController(const std::string msg);
	
};

#endif 
