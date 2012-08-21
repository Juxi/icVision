#include "moverMinJerkLinear.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;



bool MoverMinJerkLinear::init(string& robot, vector<string>& parts ) {
	MoverPosition::init(robot, parts);

	vels.clear(); vctrls.clear();
	
	for (int i=0; i<nparts; i++) {
		if (dd[i] && dd[i]->isValid() ) {
			IVelocityControl *vel;
			dd.back()->view(vel);
			vels.push_back(vel);
	
			//vctrls.push_back(new minJerkVelCtrlForIdealPlant(TS,nJoints[ipart]));
			vctrls.push_back(new minJerkVelCtrl(TS,nJoints[i]));
		}
	}

	return true;
}


bool MoverMinJerkLinear::checkVelDrivers() {
	for (int i=0; i<nparts; i++) {
		if ( !vels[i] ) { cout << "IVelocityControl part " << partnames[i] << " error" << endl; return false; }
	}
	return true;
}


void MoverMinJerkLinear::close() {
	MoverPosition::close();

	for (int i=0; i<nparts; i++) {
		if (vctrls[i]) {
			delete vctrls[i];
		}
	}
}


bool MoverMinJerkLinear::setRefSpeed(double spd) {
	//#TODO: implement some maximum velocity setting
	return true;
}


bool MoverMinJerkLinear::setRefAcceleration(double acc) {
	return true;
}


bool MoverMinJerkLinear::go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout) {
	stop  = false;
	size_t nposes = poses.size();
	int count;
	bool reached;
	double sssedist, maxdist, startStep, startTraj, nowTime, cntTime, waitTime;

	// set virtual skin waypoint at begin of trajectory
	setWayPoint();
	
	// reset minJerk controls
	for (int ipart=0; ipart<nparts; ipart++) {
		//vctrls[ipart]->reset(std2yarp(vector<double>(nJoints[ipart], 0.)));
	}

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
			vector<vector<double> > diff = poses[ipose] - encvals;
			vector<vector<double> > absdiff = abs(diff);
			maxdist = max(absdiff, mask);

			// threshold
			if ((maxdist < distancethreshold) && (sssedist < distancethreshold)) {reached = true; break;}
			
			//if there is a collision, wait until the reflex ends, or until the total trajectory timeout
			bool colliding = isColliding();
			if (colliding) {
				monReflexing();
				cout << "Warning: collision while trying to reach pose " << ipose+1 << "." << endl << "Waiting for reflex..."; 
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

				ipose = nposes; // this will exit poses loop
				break; // exit the while(!reached) loop
			}

			for (int ipart=0; ipart<nparts; ipart++) {
				vector<double> q = yarp2std(vctrls[ipart]->computeCmd(1, std2yarp(diff[ipart])));
				q = max(q, -10.); q = min(q, 10.); // limit velocities
				vels[ipart]->velocityMove(&q[0]);

				//for (int j=0;j<nJoints[ipart];j++) {
				//	cout << " " << q[j] << " ";
				//}
				//cout << endl;
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

	// send all velocities 0
	for (int ipart=0; ipart<nparts; ipart++) { vels[ipart]->stop(); }
	
	monDone(reached);

	return reached;
}
