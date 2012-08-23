#include "mover.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


Mover::Mover( ) {
	delay = 20; // delay in millis
	TS = delay/1000.;

	// face expressions monitor
	monIndex = 0;
	vector<string> monVal;
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R00"); monVal.push_back("L01"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R00"); monVal.push_back("L02"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R00"); monVal.push_back("L04"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R00"); monVal.push_back("L08"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R01"); monVal.push_back("L00"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R02"); monVal.push_back("L00"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R04"); monVal.push_back("L00"); monMoveVals.push_back(monVal);
	monVal.clear(); monVal.push_back("M0F"); monVal.push_back("R08"); monVal.push_back("L00"); monMoveVals.push_back(monVal);
	
	monReflVal.push_back("M38"); monReflVal.push_back("R04"); monReflVal.push_back("L04");
	monDoneVal.push_back("M0B"); monDoneVal.push_back("R02"); monDoneVal.push_back("L02");
	monFailVal.push_back("M08"); monDoneVal.push_back("R02"); monDoneVal.push_back("L02");

	vector<string> blinkVal;
	blinkVal.clear(); blinkVal.push_back("S38"); monBlinkVals.push_back(blinkVal); monBlinkVals.back() = blinkVal;
	blinkVal.clear(); blinkVal.push_back("S70"); monBlinkVals.push_back(blinkVal); monBlinkVals.back() = blinkVal;
}


Mover::~Mover() {
	close();
}


bool Mover::init(string& r, vector<string>& p ) {
	if ( !network.checkNetwork() ) { cout << "Cannot find YARP network." << endl; return false;}

	partnames = p;
	nparts = (int) partnames.size();
	robot = r;
	
	string part;
	Property options;

	// create drivers and controls for all parts
	dd.clear(); encs.clear();
	
	for (int i=0; i<nparts; i++) {
		options.clear();	
		part = partnames[i];
		options.put( "robot", robot.c_str() );
		options.put( "device", "remote_controlboard" );
		string localPort = "/mover/" + robot + "/" + part;
		options.put("local", localPort.c_str());
		string remotePort = "/" + robot + "/" + part;
		options.put("remote", remotePort.c_str());
		
		dd.push_back(new PolyDriver(options));
		if (dd.back() && dd.back()->isValid() ) {
			IEncoders *enc;
			dd.back()->view(enc);
			encs.push_back(enc);

			IControlLimits *lim;
			dd.back()->view(lim);
			lims.push_back(lim);			
		}
	}
	
	if (!checkValidity()) { return false; }

	// get and set some useful values
	nJoints.assign(nparts, 0);
	limitsmin.clear(); limitsmax.clear(); encvals.clear();
	for (int ipart=0; ipart<nparts; ipart++) {
		encs[ipart]->getAxes(&nJoints[ipart]);
		limitsmin.push_back(vector<double>(nJoints[ipart]));
		limitsmax.push_back(vector<double>(nJoints[ipart]));
		encvals.push_back(vector<double>(nJoints[ipart]));
		mask.push_back(vector<bool>(nJoints[ipart]));
		for (int j=0; j<nJoints[ipart]; j++) {
			lims[ipart]->getLimits(j, &(limitsmin[ipart][j]), &(limitsmax[ipart][j]));
		}
	}

	return true;
}


bool Mover::checkValidity() {
	for (int i=0; i<nparts; i++) {
		if ( !dd[i] ) { cout << "Error: PolyDriver for part " << partnames[i] << " was not created." << endl; return false; }
		if ( !dd[i]->isValid() ) { cout << "Error: PolyDriver for part " << partnames[i] << " is invalid." << endl; return false; }
		if ( !encs[i] ) { cout << "IEncoders part " << partnames[i] << " error" << endl; return false; }
		if ( !lims[i] ) { cout << "IControlLimits part " << partnames[i] << " error" << endl; return false; }
	}
	return true;
}


bool Mover::connectFace(string& rawFacePort) {
	// connect to emotions interface, if available
	string localPort = "/mover/" + robot + "/face/rpc";
	facePort.open(localPort.c_str());
	faceConnected = facePort.addOutput(rawFacePort.c_str());
	if (faceConnected) {
		blink(); monDone(true);
	} else
		cout << "Could not connect to facial expressions on: " << rawFacePort << endl;
	return faceConnected;
}


bool Mover::connnectVSkin(string& portRpc, string& statusPort) {
	bool success = true;

	// connect to Virtual Skin
	string vSkinRpcClientName = "/mover/" + robot + "/vskinrpc";
	success = success && vSkinRpcClient.open(vSkinRpcClientName.c_str());
	success = success && vSkinRpcClient.addOutput(portRpc.c_str());
	
	string vSkinStatusClientName = "/mover/" + robot + "/vskinstatus";
	success = success && vSkinStatus.open(vSkinStatusClientName.c_str());
	success = success && network.connect(statusPort.c_str(), vSkinStatusClientName.c_str());

	return success;
}


void Mover::close() {
	for (int i=0; i<nparts; i++) {
		if (dd[i]) {
			dd[i]->close();
			delete dd[i];
		}
	}
}


void Mover::getPose(Bottle &b) {
	Bottle yParts;
	for (int ipart=0; ipart<nparts; ipart++) {
		encs[ipart]->getEncoders(&encvals[ipart][0]);
		Bottle yPart;
		for (int j=0; j<nJoints[ipart]; j++) {
			yPart.addDouble(encvals[ipart][j]);
		}
		yParts.addList() = yPart;
	}
	b.clear();
	b.addList() = yParts;
}


void Mover::getMask(Bottle &b) {
	Bottle yParts;
	for (int ipart=0; ipart<nparts; ipart++) {
		Bottle yPart;
		for (int j=0; j<nJoints[ipart]; j++) {
			yPart.addInt(mask[ipart][j]);
		}
		yParts.addList() = yPart;
	}
	b.clear();
	b.addList() = yParts;
}


void Mover::maskFingers() {
	for (int ipart=0; ipart<nparts; ipart++) {
		if ((partnames[ipart].find("arm") >= 0) && (nJoints[ipart] == 16)) {
			for (int j=7; j<16; j++) {
				mask[ipart][j] = true;
			}
		}
	}
}


void Mover::maskEyes() {
	for (int ipart=0; ipart<nparts; ipart++) {
		if ((partnames[ipart].find("head") >= 0) && (nJoints[ipart] == 6)) {
			for (int j=3; j<6; j++) {
				mask[ipart][j] = true;
			}
		}
	}
}


void Mover::maskHead() {
	for (int ipart=0; ipart<nparts; ipart++) {
		if ((partnames[ipart].find("head") >= 0) && (nJoints[ipart] == 6)) {
			for (int j=0; j<6; j++) {
				mask[ipart][j] = true;
			}
		}
	}
}


void Mover::maskOff() {
	mask.clear();
	for (int ipart=0; ipart<nparts; ipart++) {
		mask.push_back(vector<bool>(nJoints[ipart]));
	}
}


bool Mover::setMask(Value &v) {
	Bottle* yParts;
	Bottle* yPart;
	vector<vector<bool> > newmask;

	if (!v.isList() || v.asList()->size() != nparts) { 
		cout << "Error: incorrect number of parts in mask." << endl;
		return false;
	}
	for (int ipart=0; ipart<nparts; ipart++) {
		yParts = v.asList();
		if (!yParts->get(ipart).isList() || yParts->get(ipart).asList()->size() != nJoints[ipart]) { 
			cout << "Error: incorrect number of joints in mask part " << ipart+1 << "." << endl;
			return false;
		}
		yPart = yParts->get(ipart).asList();
		newmask.push_back(vector<bool>(nJoints[ipart]));
		for (int j=0; j<nJoints[ipart]; j++) {
			newmask[ipart][j] = (yPart->get(j).asInt() != 0); // cast all nonzeros to true and 0 to false
		}
	}

	mask = newmask;
	return true;
}


bool Mover::parseTrajBottle(Value &v, vector<vector<vector<double> > >& poses) {
	poses.clear();
	if (!v.isList()) { return false; }
	Bottle* yPoses = v.asList();
	Bottle* yPose;
	Bottle* yPos;
	
	int nposes = yPoses->size();
	for (int ipose=0; ipose<nposes; ipose++) {
		if (!yPoses->get(ipose).isList() || yPoses->get(ipose).asList()->size() != nparts) { 
			cout << "Error: incorrect number of parts in pose " << ipose+1 << "." << endl;
			return false;
		}
		
		vector<vector<double> > vPose;
		for (int ipart=0; ipart<nparts; ipart++) {
			yPose = yPoses->get(ipose).asList();
			if (!yPose->get(ipart).isList() || yPose->get(ipart).asList()->size() != nJoints[ipart]) { 
				cout << "Error: incorrect number of joints in pose " << ipose+1 << " for part " << ipart+1 << "." << endl;
				return false;
			}
			yPos = yPose->get(ipart).asList();
			vPose.push_back(vector<double>(nJoints[ipart]));
			for (int j=0; j<nJoints[ipart]; j++) {
				vPose[ipart][j] = yPos->get(j).asDouble();
			}

			// set within limits
			vPose[ipart] = max(vPose[ipart], limitsmin[ipart]);
			vPose[ipart] = min(vPose[ipart], limitsmax[ipart]);
		}

		poses.push_back(vPose);
	}
	
	return true;
}

void Mover::pose2LinBottle(vector<vector<double> > &pose, Bottle &b) {
	for (int i = 0; i < pose.size(); i++) {
		for (int j = 0; j < pose[i].size(); j++) {
			b.addDouble(pose[i][j]);
		}
	}
}

void Mover::setWayPoint() {
	// set waypoint in VirtualSkin
	if (vSkinRpcClient.getOutputCount() > 0) {
		Bottle cmd, reply;
		cmd.addString("setWP");
		vSkinRpcClient.write(cmd, reply);
		cout << "VSkinClient response: " << reply.toString() << endl;
	}
}


void Mover::sendFace(vector<string> values) {
	// send low-level facial expression commands
	if (faceConnected) {
		Bottle query, reply;
		for (int i = 0; i<values.size(); i++) {
			query.fromString(values[i].c_str());
			facePort.write(query, false);
		}
	}
}


void Mover::monDone(bool reached) {
	monIndex = 0;
	if (reached)
		sendFace(monDoneVal);
	else
		sendFace(monFailVal);
}


void Mover::monMoving(){
	sendFace(monMoveVals[monIndex]);
	monIndex++;
	if (monIndex > (monMoveVals.size()-1)) { monIndex = 0; }
}


void Mover::monReflexing(){
	monIndex = 0;
	blink();
	sendFace(monReflVal);
}


void Mover::blink(){
	for (int i = 0; i<monBlinkVals.size(); i++) {
		sendFace(monBlinkVals[i]);
		Time::delay(0.1);
	}
}


void Mover::setStop() {
	stop = true;
}


bool Mover::isColliding() {
	Bottle* b;
	if (vSkinStatus.getInputCount() > 0) {
		b = vSkinStatus.read(false);
		return (b != NULL && (b->get(0).asInt() != 1));
	} else {
		return true;
	}
}


bool Mover::go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout) {
	printf("Not implemented\n");
	return false;
}
