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
	if (!MoverPosition::init(robot, parts)) { return false; }

	vels.clear(); vctrls.clear();
	vels.resize(nparts); vctrls.resize(nparts);

	ResourceFinder rf;
	rf.setVerbose(true);
	rf.setDefaultContext("mover/conf");
	rf.setDefault("robot","icubSim");
	int argc = 0; char *argv[1];
	rf.configure("ICUB_ROOT",argc,argv);

	for (int ipart=0; ipart<nparts; ipart++) {
		if (dd[ipart] && dd[ipart]->isValid() ) {
			IVelocityControl *vel;
			dd[ipart]->view(vel);
			vels[ipart] = vel;
#ifdef NEWYARP			
			//vctrls[ipart] = new minJerkVelCtrlForIdealPlant(TS,nJoints[ipart]);
			vctrls[ipart] = new minJerkVelCtrlForNonIdealPlant(TS,nJoints[ipart]);
	
			Property config, plantprop, getp;
			string keyname = "key_" + partnames[ipart];
			string filename = "plant_model_" + partnames[ipart] + ".ini";
			rf.setDefault(keyname.c_str(), filename.c_str());
			
			config.fromConfigFile(rf.findFile(keyname.c_str()),false);
			Bottle &optPlantModel=config.findGroup("PLANT_MODEL");
			if (!optPlantModel.isNull()) {
				fprintf(stdout,"PLANT_MODEL group detected\n");
				plantprop.fromString(optPlantModel.toString().c_str());
				
				if (plantprop.check("plant_compensator",Value("off")).asString()=="on") {
					dynamic_cast<minJerkVelCtrlForNonIdealPlant*>(vctrls[ipart])->setPlantParameters(plantprop, "joint");
					dynamic_cast<minJerkVelCtrlForNonIdealPlant*>(vctrls[ipart])->getPlantParameters(getp, "joint");
					cout << endl << endl << plantprop.toString() << endl << endl;
					cout << endl << endl << getp.toString() << endl << endl;
				}
			}
#else
			vctrls[ipart] = new minJerkVelCtrl(TS,nJoints[ipart]);
#endif
			
		}
	}

	maxSpeed = 10;

	return checkVelDrivers();
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
	MoverPosition::setRefSpeed(spd);
	maxSpeed = spd;
	return true;
}


bool MoverMinJerkLinear::setRefAcceleration(double acc) {
	return MoverPosition::setRefAcceleration(acc);
}


bool MoverMinJerkLinear::go(vector<vector<vector<double> > > &poses, double distancethreshold, double finaldistancethreshold, double steptimeout, double trajtimeout) {
	stop = false;
	int nposes = (int) poses.size();
	int count;
	bool reached;
	double rmsedist, maxdist, startStep, startTraj, nowTime, cntTime, waitTime;
	
	// set virtual skin waypoint at begin of trajectory
	setWayPoint();
	vector<vector<double > > lastVels;

	for (int ipart=0; ipart<nparts; ipart++) {
		//vels[ipart]->setVelocityMode();
		vector<double> lastVel(nJoints[ipart], 0.0);
		lastVels.push_back(lastVel);
	}

#ifdef NEWYARP
	// reset minJerk controls
	for (int ipart=0; ipart<nparts; ipart++) {
		vctrls[ipart]->reset(std2yarp(vector<double>(nJoints[ipart], 0.)));
	}
#endif

	// send start to move monitor
	Bottle &s = monitorPort.prepare(); s.clear();
	s.addVocab(VOCAB_STATUS_START);
	monitorPort.writeStrict();

	// cycle through poses
	startTraj = Time::now();
	for (int ipose=0; ((ipose<nposes) && !stop); ipose++) {
		// uncomment line below to set waypoint at each pose in the pose buffer
		// setWayPoint();

		cout << "Moving the robot to pose " << ipose+1<< " / " << nposes << "." << endl;
		
		

		for (int ipart=0; ((ipart<nparts) && !stop); ipart++) {
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

			// send to status port
			Bottle &r = monitorPort.prepare(); r.clear();
			r.addVocab(VOCAB_STATUS_ROBOT); pose2LinBottle(encvals, r.addList());
			r.addVocab(VOCAB_STATUS_TARGET); pose2LinBottle(poses[ipose], r.addList());
			r.addVocab(VOCAB_STATUS_TIME); r.addDouble(Time::now());
			monitorPort.write();

			// distances
			rmsedist = rmse(encvals, poses[ipose], mask);
			vector<vector<double> > diff = poses[ipose] - encvals;
			vector<vector<double> > absdiff = abs(diff);
			maxdist = max(absdiff, mask);

			// threshold
			if ((maxdist < distancethreshold) && (rmsedist < distancethreshold)) {reached = true; break;}
			
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

#ifdef NEWYARP
				// reset minJerk controls
				for (int ipart=0; ipart<nparts; ipart++) {
					vctrls[ipart]->reset(std2yarp(vector<double>(nJoints[ipart], 0.)));
				}
#endif

				ipose = nposes; // this will exit poses loop
				break; // exit the while(!reached) loop
			}

			for (int ipart=0; ipart<nparts; ipart++) {
				//vector<double> q = yarp2std(vctrls[ipart]->computeCmd(1, std2yarp(diff[ipart])));
				vector<double> q = yarp2std(vctrls[ipart]->computeCmd(0.33, std2yarp(diff[ipart])));
				q = max(q, -maxSpeed); q = min(q, maxSpeed); // limit velocities
				
				for (int j=0; j < nJoints[ipart]; j++) {
					if (!mask[ipart][j]  && lastVels[ipart][j] != q[j]) {
						vels[ipart]->velocityMove(j, lastVels[ipart][j]=q[j]);
					}
				}
				
				//vels[ipart]->velocityMove(&q[0]);

				/*for (int j=0;j<nJoints[ipart];j++) {
					cout << " " << q[j] << " ";
				}
				cout << endl;*/
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
	
	Bottle &e = monitorPort.prepare(); e.clear();
	e.addVocab(VOCAB_STATUS_END);
	monitorPort.writeStrict();

	return reached;
}