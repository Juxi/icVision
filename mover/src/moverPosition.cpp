#include "moverPosition.h"
#include <sstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


bool MoverPosition::init(string& robot, vector<string>& parts ) {
	Mover::init(robot, parts);
	poss.clear();
	for (int i=0; i<nparts; i++) {
		if (dd[i] && dd[i]->isValid() ) {
			IPositionControl *pos;
			dd[i]->view(pos);
			poss.push_back(pos);
		}
	}
	
	if (!checkPosDrivers()) { return false; }
		
	return true;
}


bool MoverPosition::checkPosDrivers() {
	Mover::checkValidity();
	for (int i=0; i<nparts; i++) {
		if ( !poss[i] ) { cout << "IPositionControl part " << partnames[i] << " error" << endl; return false; }
	}
	return true;
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
