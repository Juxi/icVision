/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef CARTESIAN_H
#define CARTESIAN_H

#define VOCAB_TRUE VOCAB4('t','r','u','e')
#define VOCAB_FALSE VOCAB4('f','a','l','s')
#define VOCAB_MODE_TRACK VOCAB4('c','o','n','t')
#define VOCAB_MODE_SINGLE VOCAB4('s','h','o','t')

#include "portmonitor.h"
#include <vector>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

class Cartesian : public Thread {
	
public:
	Cartesian();
	~Cartesian();
	
	bool init(string& robot, vector<string>& cartnames, vector<string>& gazenames);	//!< Connects to the remote device
	void close();																	//!< Closes the connection to the remote device
	bool setReflexTimeout(double t);
	bool setWaypointInterval(double p);
	bool connnectVSkin(string robot, string portRpc, string portStatus);
	void run();
	bool stopControllers(bool saveState);
	bool checkControllers();
	bool isColliding();
	bool resumeControllers();
	void blink();
	void monOK();
	void monReflexing();

private:
	bool colliding;
	int ncarts, ngazes;
	double delay, reflextime, wpinterval;
	string robotname;

	RpcClient vSkinRpcClient;
	RpcClient faceRpcClient;
	BufferedPort<Bottle> vSkinStatus;
	void setWayPoint();

	Semaphore mutex;
	Network network;
	PortMonitor monitor;

	vector<RpcClient*> crtports;
	vector<RpcClient*> gzeports;

	vector<string> cartnames, gazenames;
	vector<bool> crtWasTracking;
};
#endif
/** @} */
