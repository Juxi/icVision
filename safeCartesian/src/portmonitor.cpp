#include "portmonitor.h"
#include <sstream>
#include <cstring>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

PortMonitor::PortMonitor( ) {
	delay = 20;
	portCheckInterval = 5;
}


PortMonitor::~PortMonitor() {	
}


void PortMonitor::init(string& robot, vector<string>& crtnames, vector<string>& gzenames, vector<RpcClient*> cartports, vector<RpcClient*> gazeports) 
{
	robotname = robot;
	cartnames = crtnames;
	gazenames = gzenames;
	ncarts = cartnames.size();
	ngazes = gazenames.size();
	
	crtports = cartports;
	gzeports = gazeports;
}


void PortMonitor::checkAndAddConnections() {

	// create cartesian controller drivers and views
	for (int i=0; i<ncarts; i++) {
		if (crtports[i]->getOutputCount() < 1) {
			string remotePort = "/" + robotname + "/cartesianController/" + cartnames[i] + "/rpc:i";
			if (network.exists(remotePort.c_str())) {
				mutex.wait();
				crtports[i]->addOutput(remotePort.c_str());
				mutex.post();
				cout << "Cartesian Controller for part " << cartnames[i] << " found. Connection added." << endl;
			}
		}
	}
		
	// create ikin gaze drivers and views
	for (int i=0; i<ngazes; i++) {
		if (gzeports[i]->getOutputCount() < 1) {
			string remotePort = "/iKinGazeCtrl/" + gazenames[i] + "/rpc";
			if (network.exists(remotePort.c_str())) {
				mutex.wait();
				gzeports[i]->addOutput(remotePort.c_str());
				mutex.post();
				cout << "iKin Gaze Controller for part " << gazenames[i] << " found. Connection added." << endl;
			}
		}
	}
}


void PortMonitor::checkConnections(vector<bool> &crtPortsConnected, vector<bool> &gzePortsConnected) {
	mutex.wait();
	for (int i = 0; i < crtports.size(); i++)
		crtPortsConnected[i] = crtports[i]->getOutputCount() >= 1;

	for (int i = 0; i < gzeports.size(); i++)
		gzePortsConnected[i] = gzeports[i]->getOutputCount() >= 1;
	mutex.post();
}


void PortMonitor::run() {
	double lastPortCheckTime = Time::now();
	while (!isStopping()) {
		if ((Time::now() - lastPortCheckTime) >= portCheckInterval) { checkAndAddConnections(); lastPortCheckTime = Time::now(); }
		Time::delay(delay/1000.);
	}
}
