/*******************************************************************
 ***        Copyright (C) 2011 Mikhail Frank, Leo Pape           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef PORTMONITOR_H
#define PORTMONITOR_H

#include <vector>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

class PortMonitor : public Thread { //!< Monitors cartesian and gaze controller interfaces, and connects when they come online
	
public:
	PortMonitor();
	~PortMonitor();
	
	void run();
	void checkAndAddConnections(); //!< Monitors cartesian and gaze controller interfaces, and connects when they come online
	void checkConnections(vector<bool> &crtPortsConnected, vector<bool> &gzePortsConnected); //!< Check whether there are any outgoing connections to the cartesian and gaze interfaces
	void init(string& robot, vector<string>& crtnames, vector<string>& gzenames, vector<RpcClient*> cartports, vector<RpcClient*> gazeports); //!< Initialize with robot names, cartesian controller names, gaze controller names, local cartesian rpc port objects and local gaze rpc port objects

private:
	int ncarts, ngazes;
	double portCheckInterval, delay;
	
	string robotname;
	vector<string> cartnames, gazenames;
	Network network;
	Semaphore mutex;

	vector<RpcClient*> crtports;
	vector<RpcClient*> gzeports;
};
#endif
/** @} */
