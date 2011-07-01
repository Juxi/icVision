// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "reaching.h"

ReachingWorker::ReachingWorker(std::string robotName) : reachActive(false) {
	initialized = false;
	
	yarp::os::Property option("(device cartesiancontrollerclient)");
	std::string remoteLocation = "/" + robotName + "/cartesianController/right_arm";
	option.put("remote", remoteLocation.c_str());
	option.put("local", "/client/right_arm");
	
	if (!client.open(option)) {
		std::cout << "not initialized!!" << std::endl;
		exit(1);
	} else {
		// open the view
		client.view(arm);
		
		// latch the controller context in order to preserve
		// it after closing the module
		// the context contains the dofs status, the tracking mode,
		// the resting positions, the limits and so on.
		arm->storeContext(&startup_context_id);
		
		// set trajectory time
		arm->setTrajTime(1.0);
		
		// get the torso dofs
		yarp::sig::Vector newDof, curDof;
		arm->getDOF(curDof);
		newDof = curDof;
		
		// enable the torso yaw and pitch
		// disable the torso roll
		newDof[0]=0;
		newDof[1]=0;
		newDof[2]=0;
		
		// impose some restriction on the torso pitch
		//limitTorsoPitch();
		
		// send the request for dofs reconfiguration
		arm->setDOF(newDof,curDof);
		
		xd.resize(3);
		od.resize(4);
		
		/* init hand orientation
		yarp::sig::Vector x0, o0;
		arm->getPose(x0, o0);
		
		od[0]=0.0;
		od[1]=0.0;
		od[2]=0.0;
		od[3]=M_PI;
		
		// go to the target :)
		arm->goToPose(x0, od);		
*/
		initialized = true;
		std::cout << "!!initialized!!" << std::endl;		
	}	
}

ReachingWorker::~ReachingWorker() {
	std::cout << "Cleaning up the Worker... " << std::endl;
	// we require an immediate stop
	// before closing the client for safety reason
	arm->stopControl();
	
	// it's a good rule to restore the controller
	// context as it was before opening the module
	arm->restoreContext(startup_context_id);
	
	client.close();	
}

void ReachingWorker::setPosition(yarp::sig::Vector p)
{
	setPosition(p[0], p[1], p[2]);
}

void ReachingWorker::setPosition(double x, double y, double z)
{
	if(! initialized) {
		std::cout << "setPosition: not ready yet!" << std::endl;
		return;
	}

	if(xd[0] == x && xd[1] == y && xd[2] == z)
		return;
	
	std::cout << "ReachingWorker:setPosition to " << x  << ", " << y << ", " << z << "!" << xd.size() << std::endl;

	xd[0] = x;
	xd[1] = y;
	xd[2] = z;
	reachActive = false;
}

yarp::sig::Vector ReachingWorker::getPosition()
{
	yarp::sig::Vector r, o;
	arm->getPose(r, o);
	return r;	
}

void ReachingWorker::goToPosition( ) 
{
	if(!initialized) {
		std::cout << "goToPosition: not ready yet!" << std::endl;
		return;
	}

// do it only once changes
//	if(reachActive) {
//		// todo sanity check if we are there
//		std::cout << "still active" << std::endl;
//		return;
//	}
		
	yarp::sig::Vector x0, o0;
	arm->getPose(x0, o0);
	
	std::cout << "Arm Previous Pose:" << x0[0] << ", "<< x0[1] << ", "<< x0[2] <<  std::endl;

	// translational target part: a circular trajectory
	// in the yz plane centered in [-0.3,-0.1,0.1] with radius=0.1 m
	// and frequency 0.1 Hz
//	xd[0]= -0.3;	// x is backwards
//	xd[1]= 0.5;		// y is rightward
//	xd[2]=+0.5;//+0.2*sin(0.1*M_PI*0.1*(t-t0));             				// z is upwards
	
	// we keep the orientation of the left arm constant:
	// we want the middle finger to point forward (end-effector x-axis)
	// with the palm turned down (end-effector y-axis points leftward);
	// to achieve that it is enough to rotate the root frame of pi around z-axis
	
	od[0]=0.0;
	od[1]=0.0;
	od[2]=M_PI;
	od[3]=0.0;
	
	// go to the target :)
	// (in streaming)
	arm->goToPose(xd, od);	
	
	reachActive = true;
	std::cout << "Going to position:" << xd[0] << ", "<< xd[1] << ", "<< xd[2] <<  std::endl;
}

void ReachingWorker::stopMotion( ) 
{
	if(!initialized) {
		std::cout << "goToPosition: not ready yet!" << std::endl;
		return;
	}
	if(!reachActive)
		// no motion in action
		return;
	
	arm->stopControl();
	
	reachActive = false;
}
