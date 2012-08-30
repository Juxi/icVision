/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef MOVER_VELOCITY_FORWARD_H
#define MOVER_VELOCITY_FORWARD_H

#include "mover.h"
#include "moverPosition.h"
#include <vector>
#include "vectormath.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>


using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


#define VOCAB_MODE_POSITION VOCAB3('p','o','s')
#define VOCAB_MODE_VELOCITY VOCAB3('v','e','l')
#define MAX_ACCELERATION 1e9 // we handle acceleration limits ourselves, and therefore don't use the acceleration limit of the velocity controllers

class MoverVelocityForward : public MoverPosition {
	
public:
	virtual bool init(string& robot, vector<string>& partnames);	//!< Connects to the remote device
	virtual bool go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout);
	virtual bool setRefSpeed(double spd);
	virtual bool setRefAcceleration(double acc);
	virtual bool setFwdSteps(int steps);
	virtual bool setKp(double kp);
	virtual bool setMode(int mode);

protected:
							//!< Closes the connection to the remote device
	bool checkVelDrivers();
	vector<IVelocityControl*> vels;
	double maxSpeed, maxAcceleration, Kp;
	int nForwardSteps, moveMode;

};
#endif
/** @} */
