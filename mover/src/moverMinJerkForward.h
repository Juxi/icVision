/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef MOVER_MIN_JERK_FORWARD_H
#define MOVER_MIN_JERK_FORWARD_H

#include "mover.h"
#include "moverPosition.h"
#include <vector>
#include "vectormath.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <iCub/ctrl/minJerkCtrl.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace iCub::ctrl;

class MoverMinJerkForward : public MoverPosition {
	
public:
	virtual bool init(string& robot, vector<string>& partnames);	//!< Connects to the remote device
	virtual bool go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout);
	virtual bool setRefSpeed(double spd);
	virtual bool setRefAcceleration(double acc);
	virtual bool setFwdSteps(int steps);

protected:
	virtual void close();									//!< Closes the connection to the remote device
	bool checkVelDrivers();
	vector<IVelocityControl*> vels;
	vector<IPositionControl*> poss;
	vector<minJerkVelCtrl*> vctrls;
	double maxSpeed;
	int nForwardSteps;

};
#endif
/** @} */
