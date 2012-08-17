/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef MOVER_MIN_JERK_LINEAR_H
#define MOVER_MIN_JERK_LINEAR_H

#include <vector>
#include "vectormath.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <iCub/ctrl/minJerkCtrl.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace iCub::ctrl;

class MoverMinJerkLinear {
	
public:
	MoverMinJerkLinear();
	~MoverMinJerkLinear();
	
	bool init(string& robot, vector<string>& partnames);	//!< Connects to the remote device
	bool checkValidity();									//!< Checks if the remote device is ready and the interface is working
	bool connnectVSkin(string &robot, string &portRpc);
	bool connectFace(string& robot, string& rawFacePort);
	void getMask(Bottle &b);
	void getPose(Bottle &b);
	bool setRefSpeed(double spd);
	bool setRefAcceleration(double acc); 
	bool setMask(Value &v);
	bool go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout);
	bool parseTrajBottle(Value &v, vector<vector<vector<double> > >& poses);
	void setStop();											//!< Set stop flag on ongoing movement

	void maskFingers();
	void maskEyes();
	void maskHead();
	void maskOff();
	void blink();
	
private:
	void close();											//!< Closes the connection to the remote device
	void sendFace(vector<string> values);					//!< Sends string commands to low-level facial expressions interface
	void monDone(bool reached);								//!< Monitor motion done
	void monMoving();										//!< Monitor moving to next pose
	void monReflexing();									//!< Monitor reflexing behavior
	void setWayPoint();										//!< Set waypoint in Virtual Skin

	int nparts, monIndex;
	double delay, faceConnected;
	double TS;

	volatile bool stop;

	Network network;
	RpcClient vSkinRpcClient;
	Port facePort;
	vector<PolyDriver*> dd;
	vector<IEncoders*> encs;
	vector<IVelocityControl*> vels;
	vector<minJerkVelCtrl*> vctrls;
	vector<IControlLimits*> lims;
	vector<int> nJoints;
	vector<vector<bool> > mask;

	vector<string> partnames, monReflVal, monDoneVal, monFailVal;
	vector<vector<string> > monMoveVals, monBlinkVals;
	vector< vector<double> > encvals, limitsmin, limitsmax;
};
#endif
/** @} */
