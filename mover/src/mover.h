/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef MOVER_H
#define MOVER_H

#include <iostream>
#include <vector>
#include "vectormath.h"
#include <yarp/os/all.h>
#include <yarp/dev/all.h>


using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


class Mover {
	
public:
	Mover();
    ~Mover();
	
	virtual bool init(string& robot, vector<string>& partnames);	//!< Connects to the remote device
	virtual bool go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout);
	virtual bool setRefSpeed(double spd) { cout << "SetRefSpeed not implemented" << endl; return false; };
	virtual bool setRefAcceleration(double acc) {cout << "SetRefAcceleration not implemented" << endl; return false;};

	bool connnectVSkin(string &portRpc, string& statusPort);
	bool connectFace(string& rawFacePort);
	void getMask(Bottle &b);
	bool setMask(Value &v);
	bool parseTrajBottle(Value &v, vector<vector<vector<double> > >& poses);
	void setStop();											//!< Set stop flag on ongoing movement
	void getPose(Bottle &b);
	void maskFingers();
	void maskEyes();
	void maskHead();
	void maskOff();
	void blink();
	bool isColliding();
	

protected:
	virtual void close();									//!< Closes the connection to the remote device
	bool checkValidity();									//!< Checks whether the drivers came up properly
	void sendFace(vector<string> values);					//!< Sends string commands to low-level facial expressions interface
	void monDone(bool reached);								//!< Monitor motion done
	void monMoving();										//!< Monitor moving to next pose
	void monReflexing();									//!< Monitor reflexing behavior
	void setWayPoint();										//!< Set waypoint in Virtual Skin
	
	int monIndex, nparts;
	double delay, faceConnected, TS;
	volatile bool stop;
	string robot;

	Network network;
	RpcClient vSkinRpcClient;
	BufferedPort<Bottle> vSkinStatus;
	Port facePort;
	vector<int> nJoints;
	vector<vector<bool> > mask;

	vector<PolyDriver*> dd;
	vector<IEncoders*> encs;
	vector<IControlLimits*> lims;

	vector<string> partnames, monReflVal, monDoneVal, monFailVal;
	vector<vector<string> > monMoveVals, monBlinkVals;
	vector< vector<double> > encvals, limitsmin, limitsmax;
};
#endif
/** @} */
