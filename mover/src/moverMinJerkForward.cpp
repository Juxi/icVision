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
	MoverPosition::init(robot, parts);

	poss.clear();
	vels.clear(); vctrls.clear();
	
	for (int i=0; i<nparts; i++) {
		if (dd[i] && dd[i]->isValid() ) {
			IVelocityControl *vel;
			dd[i]->view(vel);
			vels.push_back(vel);
	
			IPositionControl *pos;
			dd[i]->view(pos);
			poss.push_back(pos);

			//vctrls.push_back(new minJerkVelCtrlForIdealPlant(TS,nJoints[ipart]));
			vctrls.push_back(new minJerkVelCtrl(TS,nJoints[i]));
		}
	}
	nForwardSteps = 25;
	maxSpeed = 10;

	return true;
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
	maxSpeed = spd;
	return true;
}


bool MoverMinJerkForward::setRefAcceleration(double acc) {
	return true;
}


bool MoverMinJerkForward::setFwdSteps(int s) {
	nForwardSteps = s;
	return true;
}


bool MoverMinJerkForward::go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout) {
	stop = false;
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
		currentIndex = currentIndex + (int) (mine - fwdDistances.begin()); // current pose index
		targetIndex = min(nposes-1, currentIndex+nForwardSteps); // target pose index

		// distances to the pose at targetIndex
		vector<vector<double> > diff = poses[targetIndex] - encvals;
		sssedist = rmse(encvals, poses[targetIndex], mask); 
		vector<vector<double> > absdiff = abs(diff);
		maxdist = max(absdiff, mask);

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
			trajTime = max(0.33, pmaxdist/maxSpeed); // damping can be achieved either by lower-capping the trajectory time, or by upper-capping the speed
			
			vector<double> q = yarp2std(vctrls[ipart]->computeCmd(trajTime, std2yarp(diff[ipart])));
			q = max(q, -maxSpeed); q = min(q, maxSpeed); // limit velocities
			// minabsvel = 0.5

			for (int j=0; j < nJoints[ipart]; j++) {
				/*if (!mask[ipart][j]) {
					poss[ipart]->positionMove(j, poses[targetIndex][ipart][j]);
				}*/
				if (!mask[ipart][j]  && lastVels[ipart][j] != q[j]) {
					vels[ipart]->velocityMove(j, lastVels[ipart][j]=q[j]);
				}
			}
			//vels[ipart]->velocityMove(&q[0]);

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

	return reached;
}
