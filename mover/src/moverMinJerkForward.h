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

#define VOCAB_MODE_POSITION VOCAB3('p','o','s')
#define VOCAB_MODE_VELOCITY VOCAB3('v','e','l')

class MoverMinJerkForward : public MoverPosition {
	
public:
	virtual bool init(string& robot, vector<string>& partnames);	//!< Connects to the remote device
	virtual bool go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout);
	virtual bool setRefSpeed(double spd);
	virtual bool setRefAcceleration(double acc);
	virtual bool setFwdSteps(int steps);
	virtual bool setMinTrajTime(double m);
	virtual bool setMode(int mode);
	virtual bool setMinAbsVel(double v);

protected:
	virtual void close();									//!< Closes the connection to the remote device
	bool checkVelDrivers();
	vector<IVelocityControl*> vels;
	vector<minJerkVelCtrl*> vctrls;
	double maxSpeed, minTrajTime, minabsvel;
	int nForwardSteps, moveMode;

};
#endif
/** @} */
