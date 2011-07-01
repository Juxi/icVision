// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "reaching.h"


# define FIXED_OFFSET 0.20		// 20cm


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
		init();
	}	
}

void ReachingWorker::init() {
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
	
	
	// set policy TODO
	policy = ReachingWorker::FROM_ABOVE | ReachingWorker::STRAIGHT;
	//policy = ReachingWorker::FROM_RIGHT;
	
	initialized = true;

	std::cout << "ReachingWorker is initialized!!" << std::endl;

	// more stuff todo?
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
	
	xd[0] = x;
	xd[1] = y;
	xd[2] = z;
}

yarp::sig::Vector ReachingWorker::getPosition() {
	yarp::sig::Vector v;
	for(int i = 0; i < 3 ; i++)
		v.push_back(xd[i]);
	return v;
}

yarp::sig::Vector ReachingWorker::getCurrentPosition()
{
	yarp::sig::Vector r, o;
	arm->getPose(r, o);
	return r;	
}

bool ReachingWorker::isActive( void ) 
{
	return reachActive;
}

bool ReachingWorker::isMotionDone( void ) 
{
	bool b;
	arm->checkMotionDone(&b);
	return b;
}

bool ReachingWorker::waitForMotionDone(	const double period = 0.1, const double timeout = 3.0 ) 
{
	return arm->waitMotionDone(period, timeout);
}

// doing the reaching!
void ReachingWorker::reachPosition( ) 
{
	if(!initialized) {
		std::cout << "goToPosition: not ready yet!" << std::endl;
		return;
	}
	
	// do it only once changes
	bool done;
	if ( ! arm->checkMotionDone( &done ) ) return; // YARP Error thingie
	if( !done ) {
		// todo sanity check if we are there
		std::cout << "ERROR: something else is still active" << std::endl;
		return;
	}
	
	reachActive = true;
	
	std::cout << "Starting ... ";	
	// go to pre reach position	
	if( goToPreReach() ) {
		std::cout << "PreReach completed ... ";	
		std::cout.flush();
		
		if(doReaching()) {
			std::cout << "Reaching completed!" << std::endl;			
		} else {
			std::cout << "ERROR: Reach failed" << std::endl;
		}
	} else {
		std::cout << "ERROR: PREReach failed" << std::endl;
	}
	
	reachActive = false;
}

bool ReachingWorker::goToPreReach() {
	yarp::sig::Vector x0, o0;
	arm->getPose(x0, o0);
	
	std::cout << "Arm Previous Pose:" << x0[0] << ", "<< x0[1] << ", "<< x0[2] <<  std::endl;
	
	// we keep the orientation of the left arm constant:
	// we want the middle finger to point forward (end-effector x-axis)
	// with the palm turned down (end-effector y-axis points leftward);
	// to achieve that it is enough to rotate the root frame of pi around z-axis
	
	od[0]=0.0;
	od[1]=0.0;
	od[2]=M_PI;
	od[3]=0.0;
	
	yarp::sig::Vector preReachPos = calculatePreReachPosition();
	yarp::sig::Vector preReachOrient = od;	
	
	std::cout << "Going to pre reach position:" << preReachPos[0] << ", "<< preReachPos[1] << ", "<< preReachPos[2] <<  std::endl;

	bool done = false;
	arm->goToPosition(preReachPos);
	
	while(! done ) {
		arm->waitMotionDone(0.1, 1.0);
		// check tolerance ?! maybe
		arm->checkMotionDone(&done);
		// if( collision ) return false
	}
	
	return true;
//	arm->goToPose(preReachPos, preReachOrient);
//	return arm->waitMotionDone(0.1, 3.0);
}

yarp::sig::Vector ReachingWorker::calculatePreReachPosition() {
	yarp::sig::Vector preReachPos;
	if( policy & ReachingWorker::FROM_ABOVE ) {
		
			preReachPos.push_back(xd[0]);
			preReachPos.push_back(xd[1]);	
			preReachPos.push_back(xd[2] + FIXED_OFFSET);
		
	} else if( policy & ReachingWorker::FROM_BELOW ) {
			
			preReachPos.push_back(xd[0]);
			preReachPos.push_back(xd[1]);	
			preReachPos.push_back(xd[2] - FIXED_OFFSET);
			
	} else if( policy & ReachingWorker::FROM_RIGHT ) {
			
			preReachPos.push_back(xd[0]);
			preReachPos.push_back(xd[1] + FIXED_OFFSET);	
			preReachPos.push_back(xd[2]);
		
	} else if( policy & ReachingWorker::FROM_LEFT ) {
		
		preReachPos.push_back(xd[0]);
		preReachPos.push_back(xd[1] - FIXED_OFFSET);	
		preReachPos.push_back(xd[2]);
		
	} else
		std::cout << "NOT YET IMPlEMENTED" << std::endl; 

	return preReachPos;
}

bool ReachingWorker::doReaching() {
	yarp::sig::Vector x0, o0;
	arm->getPose(x0, o0);
	std::cout << "Arm Previous Pose:" << x0[0] << ", "<< x0[1] << ", "<< x0[2] <<  std::endl;

	yarp::sig::Vector intermediatePos(3);
	if( policy & ReachingWorker::STRAIGHT ) {
		for(int i=0;i < 3;i++) intermediatePos[i] = (xd[i] - x0[i]) * 0.5 + x0[i];
	}
	
	bool done = false;
	std::cout << "Going to intermediate:" << intermediatePos[0] << ", "<< intermediatePos[1] << ", "<< intermediatePos[2] <<  std::endl;	
	arm->goToPosition(intermediatePos, 1.0);
	arm->waitMotionDone(0.1, 0.5);
	
	std::cout << "Going to reaching:" << xd[0] << ", "<< xd[1] << ", "<< xd[2] <<  std::endl;	
	arm->goToPosition(xd);
	while(! done ) {
		arm->waitMotionDone(0.1, 1.0);
		// check tolerance ?! maybe
		arm->checkMotionDone(&done);
		// if( collision ) return false
	}
	
	return true;
}
	


void ReachingWorker::stopMotion( ) 
{
	if(!initialized) {
		std::cout << "goToPosition: not ready yet!" << std::endl;
		return;
	}
	
	arm->stopControl();
	
	reachActive = false;
}
