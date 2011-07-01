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
	ReachingWorker(std::string robotName);
	~ReachingWorker();

	void setPosition(double, double, double);
	void setPosition(yarp::sig::Vector p);
	yarp::sig::Vector getPosition();

	void goToPosition();
	void stopMotion();

protected:
	bool initialized;
	bool reachActive;
	
	yarp::dev::PolyDriver client;
	yarp::dev::ICartesianControl *arm;
	
	yarp::sig::Vector xd;
	yarp::sig::Vector od;
	
    int startup_context_id;
	
    double t;
    double t0;
    double t1;
	
};

#endif 
