#include "moverPosition.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


MoverPosition::MoverPosition( ) {
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


MoverPosition::~MoverPosition() {
	close();
}


bool MoverPosition::init(string& robot, vector<string>& parts ) {
	if ( !network.checkNetwork() ) { throw "Cannot find YARP network."; }
	string part;
	Property options;

	// create drivers and controls for all parts
	dd.clear(); encs.clear(); lims.clear(); amps.clear(); pids.clear(); poss.clear();
	partnames = parts;
	nparts = partnames.size();
	
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

			IAmplifierControl *amp;
			dd.back()->view(amp);
			amps.push_back(amp);

			IPidControl *pid;
			dd.back()->view(pid);
			pids.push_back(pid);

			IPositionControl *pos;
			dd.back()->view(pos);
			poss.push_back(pos);
		}
	}
	
	if (!checkValidity()) { return false; }

	// get and set some useful values, and turn on the amplifiers
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

			// switch on amplifiers before the pids, otherwise the amplifiers are switched off!
			//amps[ipart]->enableAmp(j);
			//pids[ipart]->enablePid(j);
		}
	}

	return true;
}


bool MoverPosition::connectFace(string& robot, string& rawFacePort) {
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


bool MoverPosition::connnectVSkin(string& robot, string& portRpc) {
	bool success = true;

	// connect to Virtual Skin
	string vSkinRpcClientName = "/mover/" + robot + "/vskinrpc";
	success = success && vSkinRpcClient.open(vSkinRpcClientName.c_str());
	success = success && vSkinRpcClient.addOutput(portRpc.c_str());

	return success;
}


bool MoverPosition::checkValidity() {
	for (int i=0; i<nparts; i++) {
		if ( !dd[i] ) { cout << "Error: PolyDriver for part " << partnames[i] << " was not created." << endl; return false; }
		if ( !dd[i]->isValid() ) { cout << "Error: PolyDriver for part " << partnames[i] << " is invalid." << endl; return false; }
		if ( !poss[i] ) { cout << "IPositionControl part " << partnames[i] << " error" << endl; return false; }
		if ( !encs[i] ) { cout << "IEncoders part " << partnames[i] << " error" << endl; return false; }
		if ( !amps[i] ) { cout << "IAmplifierControl part " << partnames[i] << " error" << endl; return false; }
		if ( !lims[i] ) { cout << "IControlLimits part " << partnames[i] << " error" << endl; return false; }
	}
	return true;
}


void MoverPosition::close() {
	//if (vel || pos) { stop(); }

	for (int i=0; i<nparts; i++) {
		if (dd[i]) {
			dd[i]->close();
			delete dd[i];
		}
	}
}


bool MoverPosition::setRefSpeed(double spd) {
	bool success = true;
	for (int ipart=0; ipart<nparts; ipart++) {
		vector<double> spds(nJoints[ipart], spd);
		success = success && poss[ipart]->setRefSpeeds(&spds[0]);
	}
	return success;
}


bool MoverPosition::setRefAcceleration(double acc) {
	bool success = true;
	for (int ipart=0; ipart<nparts; ipart++) {
		vector<double> accs(nJoints[ipart], acc);
		success = success && poss[ipart]->setRefAccelerations(&accs[0]);
	}
	return success;
}


void MoverPosition::getMask(Bottle &b) {
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


void MoverPosition::getPose(Bottle &b) {
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


void MoverPosition::maskFingers() {
	for (int ipart=0; ipart<nparts; ipart++) {
		if ((partnames[ipart].find("arm") >= 0) && (nJoints[ipart] == 16)) {
			for (int j=7; j<16; j++) {
				mask[ipart][j] = true;
			}
		}
	}
}


void MoverPosition::maskEyes() {
	for (int ipart=0; ipart<nparts; ipart++) {
		if ((partnames[ipart].find("head") >= 0) && (nJoints[ipart] == 6)) {
			for (int j=3; j<6; j++) {
				mask[ipart][j] = true;
			}
		}
	}
}


void MoverPosition::maskHead() {
	for (int ipart=0; ipart<nparts; ipart++) {
		if ((partnames[ipart].find("head") >= 0) && (nJoints[ipart] == 6)) {
			for (int j=0; j<6; j++) {
				mask[ipart][j] = true;
			}
		}
	}
}


void MoverPosition::maskOff() {
	mask.clear();
	for (int ipart=0; ipart<nparts; ipart++) {
		mask.push_back(vector<bool>(nJoints[ipart]));
	}
}


bool MoverPosition::setMask(Value &v) {
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


bool MoverPosition::parseTrajBottle(Value &v, vector<vector<vector<double> > >& poses) {
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
		}
		poses.push_back(vPose);
	}
	
	return true;
}


void MoverPosition::setWayPoint() {
	// set waypoint in VirtualSkin
	if (vSkinRpcClient.getOutputCount() > 0) {
		Bottle cmd, reply;
		cmd.addString("setWP");
		vSkinRpcClient.write(cmd, reply);
		cout << "VSkinClient response: " << reply.toString() << endl;
	}
}


void MoverPosition::sendFace(vector<string> values) {
	// send low-level facial expression commands
	if (faceConnected) {
		Bottle query, reply;
		for (int i = 0; i<values.size(); i++) {
			query.fromString(values[i].c_str());
			facePort.write(query, false);
		}
	}
}


void MoverPosition::monDone(bool reached) {
	monIndex = 0;
	if (reached)
		sendFace(monDoneVal);
	else
		sendFace(monFailVal);
}


void MoverPosition::monMoving(){
	sendFace(monMoveVals[monIndex]);
	monIndex++;
	if (monIndex > (monMoveVals.size()-1)) { monIndex = 0; }
}


void MoverPosition::monReflexing(){
	monIndex = 0;
	blink();
	sendFace(monReflVal);
}


void MoverPosition::blink(){
	for (int i = 0; i<monBlinkVals.size(); i++) {
		sendFace(monBlinkVals[i]);
		Time::delay(0.1);
	}
}


void MoverPosition::setStop() {
	stop = true;
}


bool MoverPosition::go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout) {
	stop = false;
	size_t nposes = poses.size();
	int count;
	bool reached;
	double sssedist, maxdist, startStep, startTraj, nowTime, cntTime, waitTime;

	// set virtual skin waypoint at begin of trajectory
	setWayPoint();
	
	// cycle through poses
	startTraj = Time::now();
	for (int ipose=0; ((ipose<nposes) && !stop); ipose++) {
		// uncomment line below to set waypoint at each pose in the pose buffer
		// setWayPoint();

		cout << "Moving the robot to pose " << ipose+1<< " / " << nposes << "." << endl;
		
		if (poses[ipose].size() != nparts) { 
			cout << "Error: incorrect number of parts in pose " << ipose+1 << "." << endl;
			return false;
		}

		for (int ipart=0; ((ipart<nparts) && !stop); ipart++) {
			if (poses[ipose][ipart].size() != nJoints[ipart]) {
				cout << "Error: incorrect number of joints in pose " << ipose+1 << " for part " << ipart+1 << "." << endl;
				return false;
			}
		
			// set the values within joint limits range
			poses[ipose][ipart] = max(poses[ipose][ipart], limitsmin[ipart]);
			poses[ipose][ipart] = min(poses[ipose][ipart], limitsmax[ipart]);
			
			// set masked parts to current encoder position
			encs[ipart]->getEncoders(&encvals[ipart][0]);
			for (int j=0; j < nJoints[ipart]; j++) {
				if (mask[ipart][j]) {
					poses[ipose][ipart][j] = encvals[ipart][j];
				}
			}

			// send part positions
			poss[ipart]->positionMove(&poses[ipose][ipart][0]);
		}
		
		monMoving(); // monitor moving command

		// for the final pose, use a bit more accuracy. This also prevents reflexes after the final pose is within range, while the position controller is not yet finished.
		if (ipose == (nposes-1))
			distancethreshold = finaldistancethreshold;

		// wait until distance is within distancethreshold, or until timeout
		reached = false;
		startStep = Time::now();
		count = 0;
		while (!reached && !stop) {
			// get encoder positions
			for (int ipart=0; ipart<nparts; ipart++) {
				encs[ipart]->getEncoders(&encvals[ipart][0]);
			}

			// distances
			sssedist = rmse(encvals, poses[ipose], mask);
			vector<vector<double> > diff = encvals - poses[ipose];
			diff = abs(diff);
			maxdist = max(diff, mask);

//for (int ipart=0; ipart<nparts; ipart++) {
//	for (int j=0; j < nJoints[ipart]; j++) {
//		cout << diff[ipart][j] <<  " " << endl;
//	}
//	cout << endl;
//}


			// threshold
			if ((maxdist < distancethreshold) && (sssedist < distancethreshold)) {reached = true; break;}
			
			//if there is a collision, wait until the reflex ends, or until the total trajectory timeout
			//Bottle* b = vSkinCollisions.read(false);
			//if (b != NULL && !b->get(0).isDouble())...
			// this is a workaround that just checks if rpc communication is possible; future version should read the collisionn status port directly
			double temp;
			bool colliding = ((encs.size() > 0) && !encs[0]->getEncoder(0, &temp));
			if (colliding) {
				monReflexing();
				cout << "Warning: collision while trying to reach pose " << ipose+1 << "." << endl << "Waiting for reflex..."; 
				while (colliding && !stop) {
					Time::delay(TS);
					if ((Time::now() - startTraj) >= trajtimeout) { break; }
					colliding = ((encs.size() > 0) && !encs[0]->getEncoder(0, &temp));
				}

				if (colliding)
					cout << " reflex unsuccesful." << endl;
				else
					cout << " reflex done." << endl;

				ipose = nposes; // this will exit poses loop
				break; // exit the while(!reached) loop
			}

			// timing
			count++;
			nowTime = Time::now();
			cntTime = startStep + count * TS; // expected time according to counter
			waitTime = cntTime - nowTime;
			nowTime = Time::now();
			if ( (nowTime - startTraj) >= trajtimeout)
				break;
			if (waitTime > 0.) // if expected time is before counter, wait a little bit
				Time::delay(waitTime);
			if ((cntTime - startStep) >= steptimeout) {
				cout << "Warning: step timeout while trying to reach pose " << ipose+1 << "." << endl;
				break;
			}
		}
		
		if ( (nowTime - startTraj) >= trajtimeout) {
			cout << "Warning: trajectory timeout while trying to reach pose " << ipose+1 << "." << endl;
			break;
		}
	}

	Time::delay(TS*5.); // hopefully this will fix reflex commands arriving in the wrong order...
	monDone(reached);

	return reached;
}
