/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef MOVER_POSITION_H
#define MOVER_POSITION_H

#include "mover.h"
#include <vector>
#include "vectormath.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>


using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


class MoverPosition : public Mover {
	
public:
	virtual bool init(string& robot, vector<string>& partnames);	//!< Connects to the remote device
	virtual bool go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout);
	virtual bool setRefSpeed(double spd);
	virtual bool setRefAcceleration(double acc);

protected:
	bool checkPosDrivers();
	vector<IPositionControl*> poss;
};
#endif
/** @} */
