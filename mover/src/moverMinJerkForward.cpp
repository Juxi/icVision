#include "moverMinJerkForward.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;



bool MoverMinJerkForward::init(string& robot, vector<string>& parts ) {
	if (!MoverPosition::init(robot, parts)) {return false; }

	vels.clear(); vctrls.clear();
	vels.resize(nparts); vctrls.resize(nparts);
	
	for (int i=0; i<nparts; i++) {
		if (dd[i] && dd[i]->isValid() ) {
			IVelocityControl *vel;
			dd[i]->view(vel);
			vels[i] = vel;

			//vctrls[i] = new minJerkVelCtrlForIdealPlant(TS,nJoints[ipart]);
			vctrls[i] = new minJerkVelCtrl(TS,nJoints[i]);
		}
	}
	minTrajTime = 0.5;
	nForwardSteps = 25;
	maxSpeed = 10;
	moveMode = VOCAB_MODE_VELOCITY;

	return checkVelDrivers();
}


bool MoverMinJerkForward::checkVelDrivers() {
	for (int i=0; i<nparts; i++) {
		if ( !vels[i] ) { cout << "IVelocityControl part " << partnames[i] << " error" << endl; return false; }
	}
	return true;
}


void MoverMinJerkForward::close() {
	MoverPosition::close();

	for (int i=0; i<nparts; i++) {
		if (vctrls[i]) {
			delete vctrls[i];
		}
	}
}


bool MoverMinJerkForward::setRefSpeed(double spd) {
	MoverPosition::setRefSpeed(spd);
	maxSpeed = spd;
	return true;
}


bool MoverMinJerkForward::setRefAcceleration(double acc) {
	return MoverPosition::setRefAcceleration(acc);
}


bool MoverMinJerkForward::setFwdSteps(int s) {
	nForwardSteps = s;
	return true;
}

bool MoverMinJerkForward::setMinTrajTime(double m) {
	minTrajTime = m;
	return true;
}

bool MoverMinJerkForward::setMode(int m) {
	if ((moveMode == VOCAB_MODE_POSITION) || (moveMode == VOCAB_MODE_VELOCITY)) {
		moveMode = m;
		return true;
	} else
		return false;
}


bool MoverMinJerkForward::go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout) {
	stop = false;
	int dragFactor = 10;
	double minabsvel = 0.5;
	int nposes = (int) poses.size();
	int count;
	bool reached = false;
	double sssedist, maxdist, pmaxdist, trajTime, startTraj, nowTime, cntTime, waitTime;
	int currentIndex = 0, tForwardSteps, targetIndex;

	// set virtual skin waypoint at begin of trajectory
	setWayPoint();
	vector<vector<double > > lastVels;

	for (int ipart=0; ipart<nparts; ipart++) {
		vels[ipart]->setVelocityMode();
		vector<double> lastVel(nJoints[ipart], 0.0);
		lastVels.push_back(lastVel);
	}

	// reset minJerk controls
	for (int ipart=0; ipart<nparts; ipart++) {
		//vctrls[ipart]->reset(std2yarp(vector<double>(nJoints[ipart], 0.)));
	}
	
	// send start to move monitor
	Bottle &s = monitorPort.prepare(); s.clear();
	s.addVocab(VOCAB_STATUS_START);
	monitorPort.writeStrict();

	// moving loop
	count = 0;
	startTraj = Time::now();
	while (!reached && !stop) {
		// get encoder positions
		for (int ipart=0; ipart<nparts; ipart++) {
			encs[ipart]->getEncoders(&encvals[ipart][0]);
		}
			
		// match the robot's pose with poses between currentIndex and currentIndex+nForwardSteps
		tForwardSteps = max(0, min(nposes, currentIndex+nForwardSteps) - currentIndex); // number of available forward steps
		vector<double> fwdDistances(tForwardSteps, 0.0);
		for (int icp = 0; icp < tForwardSteps; icp++) {
			fwdDistances[icp] = rmse(encvals, poses[currentIndex+icp], mask);
		}

		vector<double>::iterator mine = std::min_element(fwdDistances.begin(), fwdDistances.end());
		int fwdStep = (int) (mine - fwdDistances.begin());
		//currentIndex = currentIndex + (int) (mine - fwdDistances.begin()); // current pose index
		currentIndex = currentIndex + min(fwdStep, max(1, tForwardSteps/dragFactor)); // increment current index to a maximum of scaled dragFactor
		targetIndex = min(nposes-1, currentIndex+nForwardSteps); // target pose index
		targetIndex = min(count*(nForwardSteps/dragFactor), targetIndex); // make sure the targetindex does not suddenly jump to nForwardSteps at the beginning of the movement

		// distances to the pose at targetIndex
		vector<vector<double> > diff = poses[targetIndex] - encvals;
		sssedist = rmse(encvals, poses[targetIndex], mask); 
		vector<vector<double> > absdiff = abs(diff);
		maxdist = max(absdiff, mask);

		// send to status port
		Bottle &r = monitorPort.prepare(); r.clear();
		r.addVocab(VOCAB_STATUS_ROBOT); pose2LinBottle(encvals, r.addList());
		r.addVocab(VOCAB_STATUS_TARGET); pose2LinBottle(poses[targetIndex], r.addList());
		r.addVocab(VOCAB_STATUS_TIME); r.addDouble(Time::now());
		monitorPort.write();
		

		// check if final pose is reached; both ssse distance and maximum distance should be smaller than distancethreshold
		if (targetIndex == (nposes-1)) {
			//cout << "targetting final pose; ssse: " << sssedist << " maxabse: " << maxdist << " threshold: " << finaldistancethreshold << endl;
			if ((maxdist < finaldistancethreshold) && (sssedist < finaldistancethreshold)) {
				reached = true; break;
			}
		}
			
		//if there is a collision, wait until the reflex ends, or until the total trajectory timeout
		bool colliding = isColliding();
		if (colliding) {
			monReflexing();
			cout << "Warning: collision while trying to reach pose " << targetIndex+1 << "." << endl << "Waiting for reflex..."; 
			while (colliding && !stop) {
				Time::delay(TS);
				if ((Time::now() - startTraj) >= trajtimeout) { break; }
				colliding = isColliding();
			}

			if (colliding)
				cout << " reflex unsuccesful." << endl;
			else
				cout << " reflex done." << endl;

			// reset minJerk controls
			for (int ipart=0; ipart<nparts; ipart++) {
				//vctrls[ipart]->reset(std2yarp(vector<double>(nJoints[ipart], 0.)));
			}

			break; // exit the while(!reached) loop
		}

		// send velocities
		//cout << "current index: " << currentIndex << " target index " << targetIndex << endl;
		//cout << "trajectory times :";
		for (int ipart=0; ipart<nparts; ipart++) {
			// dynamically adjust the trajectory time by the maximum distance in joint space:
			pmaxdist = max(absdiff[ipart], mask[ipart]);
			trajTime = max(minTrajTime, pmaxdist/maxSpeed); // damping can be achieved either by lower-capping the trajectory time, or by upper-capping the speed
			
			vector<double> q = yarp2std(vctrls[ipart]->computeCmd(trajTime, std2yarp(diff[ipart])));
			q = max(q, -maxSpeed); q = min(q, maxSpeed); // limit velocities
			// minabsvel = 0.5

			for (int iax=0; iax < nJoints[ipart]; iax++) {
				if (!mask[ipart][iax] && (moveMode == VOCAB_MODE_POSITION)) {
					poss[ipart]->positionMove(iax, poses[targetIndex][ipart][iax]);
				}
				if (!mask[ipart][iax] && (lastVels[ipart][iax] != q[iax]) && (moveMode == VOCAB_MODE_VELOCITY)) {
					// apply bang-bang control for unachievably low velocities
					if ((q[iax] > -minabsvel) && (q[iax] < minabsvel) && (q[iax]!=0.0)) {
						q[iax]=sign(diff[ipart][iax])*minabsvel;
					}

					// send velocities to robot
					vels[ipart]->velocityMove(iax, lastVels[ipart][iax]=q[iax]);
				}
			}

			/*for (int j=0;j<nJoints[ipart];j++) {
				cout << " " << q[j] << " ";
			}
			cout << endl;*/
			//cout << " " << trajTime;
		}
		//cout << endl;

		// timing
		count++;
		nowTime = Time::now();
		cntTime = startTraj + count * TS; // expected time according to counter
		waitTime = cntTime - nowTime;
		if (waitTime > 0.) // if expected time is before counter, wait a little bit
			Time::delay(waitTime);
		if ( (nowTime - startTraj) >= trajtimeout) {
			cout << "Warning: trajectory timeout while trying to reach pose " << targetIndex+1 << "." << endl;
			break;
		}
	}

	Time::delay(TS*5.); // hopefully this will fix reflex commands arriving in the wrong order...

	// send all velocities 0
	for (int ipart=0; ipart<nparts; ipart++) { vels[ipart]->stop(); }
	
	monDone(reached);

	Bottle &e = monitorPort.prepare(); e.clear();
	e.addVocab(VOCAB_STATUS_END);
	monitorPort.writeStrict();

	return reached;
}
