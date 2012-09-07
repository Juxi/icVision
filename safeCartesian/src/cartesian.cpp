#include "cartesian.h"
#include "portmonitor.h"
#include <sstream>
#include <cstring>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

Cartesian::Cartesian( ) {
	delay = 20; // delay in millis
	reflextime = 10; // maximum reflex timeout in seconds
	wpinterval = 2; // waypoint interval
}

Cartesian::~Cartesian() {
	close();
}

bool Cartesian::init(string& robot, vector<string>& carts, vector<string>& gazes ) {
	if ( !network.checkNetwork() ) { cout << "Cannot find YARP network." << endl; return false; }
	Property options;
	string localPort, remotePort;
	crtports.clear(); gzeports.clear();
	ncarts = carts.size(); ngazes = gazes.size();
	cartnames = carts; gazenames = gazes; robotname = robot;

	// create local cartesian ports
	crtports.assign(ncarts, NULL);
	crtWasTracking.assign(ncarts, false);
	for (int i=0; i<ncarts; i++) {
		localPort = "/cartesian/" + robotname + "/cartesianController/" + cartnames[i] + "/rpc";
		crtports[i] = new RpcClient();
		crtports[i]->open(localPort.c_str());
	}
	
	// create local ikin gaze ports
	gzeports.assign(ngazes, NULL);
	for (int i=0; i<ngazes; i++) {
		localPort = "/cartesian/" + robotname + "/iKinGazeCtrl/" + gazenames[i] + "/rpc";
		gzeports[i] = new RpcClient();
		gzeports[i]->open(localPort.c_str());
	}

	// connect to emotions interface, if available
	localPort = "/cartesian/" + robotname + "/face/rpc";
	remotePort = "/" + robot + "/face/emotions/in";
	faceRpcClient.open(localPort.c_str());
	//faceRpcClient.addOutput(remotePort.c_str());
	cout << endl << "Warning: face connection disabled!!!" << endl;

	// run thread that connects to the cartesian controllers when they come online
	monitor.init(robotname, cartnames, gazenames, crtports, gzeports); 
	monitor.checkAndAddConnections();
	monitor.start();

	// module successfully started: blink and look happy
	blink();
	monOK();

	return true;
}


bool Cartesian::connnectVSkin(string robot, string portRpc, string portStatus) {
	// connect to virtual skin rpc port
	bool success = true;

	// connect to Virtual Skin
	string vSkinRpcClientName = "/cartesian/" + robot + "/vskinRpc";
	success = success && vSkinRpcClient.open(vSkinRpcClientName.c_str());
	success = success && vSkinRpcClient.addOutput(portRpc.c_str());

	string vSkinStatusName = "/cartesian/" + robot + "/vskinStatus";
	success = success && vSkinStatus.open(vSkinStatusName.c_str());
	success = success && network.connect(portStatus.c_str(), vSkinStatusName.c_str());
	return success;
}


void Cartesian::close() {
	// stop threads, and close drivers and connections
	monitor.stop();
	stop();
	stopControllers(false);

	for (int i=0; i<crtports.size(); i++) {
		if (crtports[i]) {
			crtports[i]->close();
			delete crtports[i];
		}
	}
	for (int i=0; i<gzeports.size(); i++) {
		if (gzeports[i]) {
			gzeports[i]->close();
			delete gzeports[i];
		}
	}
}


void Cartesian::setWayPoint() {
	// set waypoint in VirtualSkin
	if (vSkinRpcClient.getOutputCount() > 0) {
		Bottle cmd, reply;
		cmd.addString("setWP");
		vSkinRpcClient.write(cmd, reply);
		cout << "VSkinClient response: " << reply.toString() << endl;
	}
}


bool Cartesian::setReflexTimeout(double t) {
	// set timeout value for tracking reflex behavior
	mutex.wait();
	reflextime = t;
	mutex.post();
	return true;
}


bool Cartesian::setWaypointInterval(double p) {
	// set time interval at which waypoints are sent to Virtual Skin in seconds
	mutex.wait();
	wpinterval = p;
	mutex.post();
	return true;
}


bool Cartesian::stopControllers(bool saveState) {
	// stop all connected cartesian and gaze controllers
	bool stopped = true;
	double start = Time::now();

	// check which ports are connected
	vector<bool> crtPortsConnected(ncarts, false);
	vector<bool> gzePortsConnected(ngazes, false);
	monitor.checkConnections(crtPortsConnected, gzePortsConnected);
	
	// initialize
	Bottle command, reply;
	vector<bool> wasTracking(ncarts, false);

	// query current tracking mode of cartesian controllers
	command.fromString("get"); command.addString("mode");
	for (int i = 0; i < crtports.size(); i++) {
		if (crtPortsConnected[i]) {
			wasTracking[i] = crtports[i]->write(command,reply);
			wasTracking[i] = wasTracking[i] && (reply.get(1).asVocab() == VOCAB_MODE_TRACK);
		}
	}

	// disable tracking mode for cartesian controllers
	command.fromString("set"); command.addString("mode"); command.addVocab(VOCAB_MODE_SINGLE);
	for (int i = 0; i < crtports.size(); i++) {
		if (crtPortsConnected[i] && wasTracking[i])
			stopped = stopped & (crtports[i]->write(command,reply)); // NOTE: no short-ciruit AND, because we want to send stop commands always!
	}
	
	// stop cartesian controllers stop with "stop" command
	command.fromString("stop");
	for (int i = 0; i < crtports.size(); i++) {
		if (crtPortsConnected[i])
			stopped = stopped & (crtports[i]->write(command,reply)); // NOTE: no short-ciruit AND, because we want to send stop commands always!
	}

	// gaze controllers stop with "susp" command, not with "stop"
	command.fromString("susp");
	for (int i = 0; i < gzeports.size(); i++) {
		if (gzePortsConnected[i]) {
			stopped = stopped & (gzeports[i]->write(command,reply)); // NOTE: no short-ciruit AND, because we want to send susp commands always!
		}
	}

	// save tracking state
	if (saveState) { crtWasTracking = wasTracking; }

	if (stopped) { cout << "Cartesian and Gaze controllers successfully stopped in " << Time::now()-start << " seconds." << endl; }
	else { cout << "Failure! Failure! Failure! Danger! Cartesian or Gaze controllers refused to stop after " << Time::now()-start << " seconds!" << endl; }

	monReflexing(); // set monitor

	return stopped;
}


bool Cartesian::isColliding() {
	// query Virtual Skin for collisions
	bool ok = true;
	Bottle* b = NULL;
	ok = ok && (vSkinStatus.getInputCount() > 0);
	b = vSkinStatus.read(true);
	ok = ok && (b != NULL);
	ok = ok && (b->get(0).asInt() != 0);
	return !ok;
}


bool Cartesian::resumeControllers() {
	bool resumed = true;
	vector<bool> crtPortsConnected(ncarts, false);
	vector<bool> gzePortsConnected(ngazes, false);
	monitor.checkConnections(crtPortsConnected, gzePortsConnected);
	Bottle command, reply;

	// resume tracking state of cartesian controllers
	command.fromString("set"); command.addString("mode"); command.addVocab(VOCAB_MODE_TRACK);
	for (int i = 0; i < crtports.size(); i++) {
		if (crtPortsConnected[i] && crtWasTracking[i]) {
			resumed = resumed & crtports[i]->write(command,reply);
		}
	}

	// resume gaze controllers
	for (int i = 0; i < gzeports.size(); i++) {
		if (gzePortsConnected[i]) {
			// write run first, because writing anything during suspension has no effect
			command.fromString("run");
			resumed = resumed & (gzeports[i]->write(command,reply));

			// write stop
			command.fromString("stop");
			gzeports[i]->write(command,reply);
		}
	}

	if (resumed) cout << "Cartesian and Gaze controllers successfully resumed." << endl;
	else cout << "Gaze controllers failed to resume." << endl;
	
	monOK(); // set monitor

	return resumed;
}


bool Cartesian::checkControllers() {
	// check which ports are connected
	vector<bool> crtPortsConnected(ncarts, false);
	vector<bool> gzePortsConnected(ngazes, false);
	monitor.checkConnections(crtPortsConnected, gzePortsConnected);
	bool done = true;
	Bottle command, reply;

	// query getmotiondone
	command.fromString("get"); command.addString("done");
	for (int i = 0; i < crtports.size(); i++) {
		if (crtPortsConnected[i]) {
			done = done && (crtports[i]->write(command,reply)); 
			done = done && (reply.get(1).asVocab() == VOCAB_TRUE);
		}
	}

	// check whether tracking is still off
	command.fromString("get"); command.addString("mode");
	for (int i = 0; i < crtports.size(); i++) {
		if (crtPortsConnected[i]) {
			done = done && crtports[i]->write(command,reply);
			done = done && reply.get(1).asVocab() == VOCAB_MODE_SINGLE;
		}
	}

	if (!done)
		cout << "Warning: Cartesian controllers still busy." << endl;

	// Gaze controllers have to be explicitly resumed, and cannot be accidentally reactivated during
	// a reflex (they can be activated by explicitly calling "run", though, but the user should 
	// not be THAT stupid). Anyway, there is no way to check whether a gaze controller is currently
	// driving the robot.

	return done;
}


void Cartesian::monOK(){
	Bottle query, reply;
	query.fromString("set"); query.addString("all"); query.addString("hap");
	if (faceRpcClient.getOutputCount() > 0)
		faceRpcClient.write(query, reply);
}


void Cartesian::monReflexing(){
	blink();
	Bottle query, reply;
	query.fromString("set"); query.addString("all"); query.addString("shy");
	if (faceRpcClient.getOutputCount() > 0)
		faceRpcClient.write(query, reply);
}


void Cartesian::blink(){
	Bottle query, reply;
	query.fromString("set"); query.addString("eli"); query.addString("sad");
	if (faceRpcClient.getOutputCount() > 0)
		faceRpcClient.write(query, reply);
	query.fromString("set"); query.addString("eli"); query.addString("hap");
	Time::delay(0.1);
	if (faceRpcClient.getOutputCount() > 0)
		faceRpcClient.write(query, reply);
}


void Cartesian::run() {
	//if there is a collision, stop the cartesian controllers immediately
	double start;
	double lastWPtime = Time::now();
	bool resumed = true;

	while (!isStopping()) {

		//DEBUGGING: set a waypoint every WAYPOINTINTERVAL second, as long as no collisions occur
		//if ((Time::now() - lastWPtime) >= wpinterval) { setWayPoint(); lastWPtime = Time::now(); }
		
		colliding = isColliding();
		if (colliding) {
			start = Time::now();
			cout << "Collision occured" << endl;
			stopControllers(resumed);
			resumed = false;
			while (colliding) { // wait a little bit for the relfex to stop witout re-sending stop commands to the controllers
				Time::delay(delay/1000.);
				if (!checkControllers()) { stopControllers(resumed); }// if the user is so stupid to issue a new cartesian or gaze command DURING a reflex, we have to immediately stop the controllers again!

				// check for timeout
				if ((Time::now() - start) >= reflextime) { break; }
				colliding = isColliding();
			}
			if (colliding)
				cout << "Warning: reflex unsuccesful!" << endl;
			else
				cout << "Reflex done." << endl;
		} else if (!resumed) { // resume the controllers once no more collisions are reported
			resumeControllers();
			resumed = true;
		}
		Time::delay(delay/1000.);
	}
}
