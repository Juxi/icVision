// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "reaching.h"


ReachingWorker::ReachingWorker(std::string robotName, std::string partName) : reachActive(false) {
	initialized = false;
	
	yarp::os::Property option("(device cartesiancontrollerclient)");
	std::string remoteLocation = "/" + robotName + "/cartesianController/" + partName;
	std::string localLocation  = "/client/" + partName;
	option.put("remote", remoteLocation.c_str());
	option.put("local",  localLocation.c_str());
	
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
	arm->setTrajTime(3.0);
	
	// get the torso dofs
	yarp::sig::Vector newDof, curDof;
	arm->getDOF(curDof);
	newDof = curDof;
	
	// enable the torso yaw and pitch
	// disable the torso roll
	newDof[0]=0;
	newDof[1]=0;
	newDof[2]=0;

	newDof[0]=1;
        newDof[1]=0;
        newDof[2]=1;


	// impose some restriction on the torso pitch
	//limitTorsoPitch();
// void limitTorsoPitch()
  //  {
        int axis=0; // pitch joint
        double min, max;

        // sometimes it may be helpful to reduce
        // the range of variability of the joints;
        // for example here we don't want the torso
        // to lean out more than 30 degrees forward

        // we keep the lower limit
        arm->getLimits(axis,&min,&max);
        arm->setLimits(axis,min, 30.0);	  // [deg]
    //}
	
	// send the request for dofs reconfiguration
	arm->setDOF(newDof, curDof);
	
	// tolerance?
	double tol;
	arm->getInTargetTol(&tol);
	std::cout << "Tolerance: " << tol << std::endl;

	xd.resize(3);
		
	// init hand orientation
	// from Alex' and cartesian interface tutorial!
	orientationFromSide.resize(4);
	orientationFromSide[0] =  0.0; 
	orientationFromSide[1] = -1.0; 
	orientationFromSide[2] =  1.0; 
	orientationFromSide[3] = M_PI;

	orientationFromAbove.resize(4);
	orientationFromAbove[0] =  0.0; 
	orientationFromAbove[1] =  0.0; 
	orientationFromAbove[2] =  1.0; 
	orientationFromAbove[3] = M_PI;

	// tweaking for left arm demo!!
//	orientationFromAbove[0] = -0.15;
//	orientationFromAbove[1] =  0.23; 
//	orientationFromAbove[2] = -0.96; 
//	orientationFromAbove[3] =  3.14;
	
	// set policy TODO
	setPolicy( ReachingWorker::FROM_LEFT | ReachingWorker::STRAIGHT );

	initialized = true;

	std::cout << "ReachingWorker is initialized!!" << std::endl;
	
	// yarp::sig::Vector x0, o0;
	// arm->getPose(x0, o0);
	// go to the target :)
	// arm->goToPose(x0, od);
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

void ReachingWorker::setPolicy(int p) {
	policy = p;
	std::cout << "Reaching policy is now set as: " << policy << std::endl;
}
void ReachingWorker::setOffset(double o)
{
	defined_offset = o;
	std::cout << "The offset is now set to: " << defined_offset << std::endl;
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

void ReachingWorker::printStatus()
{        
	yarp::sig::Vector x,o,xdhat,odhat,qdhat;
		
		// we get the current arm pose in the
	// operational space
	arm->getPose(x,o);
	
	// we get the final destination of the arm
	// as found by the solver: it differs a bit
	// from the desired pose according to the tolerances
	arm->getDesired(xdhat,odhat,qdhat);
	
//	double e_x=yarp::math::norm(xdhat-x);
//	double e_o=yarp::math::norm(odhat-o);
	
	fprintf(stdout,"\n\n+++++++++\n");
	fprintf(stdout,"xd          [m] = %s\n",xd.toString().c_str());
	fprintf(stdout,"xdhat       [m] = %s\n",xdhat.toString().c_str());
	fprintf(stdout,"x           [m] = %s\n",x.toString().c_str());
	fprintf(stdout,"od        [rad] = %s\n",orientationFromAbove.toString().c_str());
	fprintf(stdout,"odhat     [rad] = %s\n",odhat.toString().c_str());
	fprintf(stdout,"o         [rad] = %s\n",o.toString().c_str());
	fprintf(stdout,"---------\n\n");
	
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

		printStatus();
		yarp::os::Time::delay(0.2);

		if(doReaching()) {
			std::cout << "Reaching completed!" << std::endl;			
			printStatus();
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
		
	yarp::sig::Vector preReachPos = calculatePreReachPosition();
	yarp::sig::Vector preReachOrient;	// keep constant!
	
	if( policy & FROM_ABOVE || policy & FROM_BELOW) {
		preReachOrient = orientationFromAbove;
	}
	if( policy & FROM_LEFT || policy & FROM_RIGHT) {
		preReachOrient = orientationFromSide;
	}
	
	std::cout << "Going to pre reach position:" << preReachPos[0] << ", "<< preReachPos[1] << ", "<< preReachPos[2] <<  std::endl;

	bool done = false;

	arm->goToPose(preReachPos, preReachOrient);

	while(! done ) {
		arm->waitMotionDone(0.1, 0.5);
		// check tolerance ?! maybe
		arm->checkMotionDone(&done);
		// if( collision ) return false
		std::cout << "waiting ...";
	}
	std::cout <<  std::endl;
//	arm->getPose(x0, o0);	
//		if(! (o0[0] < -0.1 && o0[0] > -0.15) ) {
//			printStatus();
//			done = false;
//			std::cout << "reissuing command to go to position" << std::endl;
//		}
//	}while(!done);
	
	return true;
//	arm->goToPose(preReachPos, preReachOrient);
//	return arm->waitMotionDone(0.1, 3.0);
}

yarp::sig::Vector ReachingWorker::calculatePreReachPosition() {
	yarp::sig::Vector preReachPos;
	if( policy & ReachingWorker::FROM_ABOVE ) {
		
			preReachPos.push_back(xd[0]);
			preReachPos.push_back(xd[1] - 0.05);	
			preReachPos.push_back(xd[2] + defined_offset);
		
	} else if( policy & ReachingWorker::FROM_BELOW ) {
			
			preReachPos.push_back(xd[0]);
			preReachPos.push_back(xd[1]);	
			preReachPos.push_back(xd[2] - defined_offset);
			
	} else if( policy & ReachingWorker::FROM_RIGHT ) {
			
			preReachPos.push_back(xd[0]);
			preReachPos.push_back(xd[1] + defined_offset);	
			preReachPos.push_back(xd[2]);
		
	} else if( policy & ReachingWorker::FROM_LEFT ) {
		
		preReachPos.push_back(xd[0] + 0.05);
		preReachPos.push_back(xd[1] - defined_offset);	
		preReachPos.push_back(xd[2] + 0.05); //0.1needed but why?);
		
	} else
		std::cout << "NOT YET IMPlEMENTED" << std::endl; 

	return preReachPos;
}

bool ReachingWorker::doReaching() {
	if( ! reachActive ) return false;

	yarp::sig::Vector x, x0, o0;
	arm->getPose(x0, o0);
//	std::cout << "Arm Previous Pose:" << x0[0] << ", "<< x0[1] << ", "<< x0[2] <<  std::endl;

	double reaching_offset = 0.05;	// sphere RADIUS + eps?
	x = xd;

	yarp::sig::Vector orientation;
	if( policy & FROM_ABOVE || policy & FROM_BELOW) {
		orientation = orientationFromAbove;
		std::cout << "from above!!" << std::endl;
		if( policy & FROM_ABOVE) x[2] += reaching_offset;
		else x[2] -= reaching_offset;
		x[1] -= 0.02;
	}
	if( policy & FROM_LEFT || policy & FROM_RIGHT) {
		orientation = orientationFromSide;
		if( policy & FROM_RIGHT) x[1] += reaching_offset;
		else x[1] -= reaching_offset;

	// real robot testing
		//x[0] += 0.02;
	}

	// offset needed?!!?!
//	x[2] += 0.05;


	
	// setup hand
	callGraspController("pre");

	bool done = false;
	
/* no intermediate for now
	const int MAX_STEPS = 2;
	double fact = 1.0 / MAX_STEPS;
//	for(int steps = 1; steps < MAX_STEPS; steps++) {

		yarp::sig::Vector intermediatePos(3);
		if( policy & ReachingWorker::STRAIGHT ) {
			for(int i=0;i < 3;i++) intermediatePos[i] = (x[i] - x0[i]) * fact + x0[i];
		}
		
		std::cout << "Going to intermediate:" << intermediatePos[0] << ", "<< intermediatePos[1] << ", "<< intermediatePos[2] <<  std::endl;	
		
		// move arm	
//		arm->goToPose(intermediatePos, orientation, fact);
		arm->goToPose(intermediatePos, orientation);	
		arm->waitMotionDone(0.1, fact);
//	}
*/
	
	// setup hand, close now
	callGraspController("cls");
		
	std::cout << "Going to reaching:" << x[0] << ", "<< x[1] << ", "<< x[2] <<  std::endl;	

	// move arm
//	arm->goToPose(xd, orientation, 1.0);
	arm->goToPose(x, orientation); // testing in the lab!!, 1.0);
	arm->waitMotionDone(0.1, 0.5/*fact*/);

	while(! done ) {
		arm->waitMotionDone(0.1, 1.0);
		// check tolerance ?! maybe
		// if( collision ) return false
		arm->checkMotionDone(&done);		
	}
	
	return true;
}
	

void ReachingWorker::callGraspController(const std::string msg) {
	yarp::os::Network yarp;
	yarp::os::RpcClient port;

	std::string inputPortName = "/graspController";
	std::string clientPortName = "graspClient";
	if(! port.open( clientPortName.c_str() )){
		std::cout << std::endl << "ERROR: Could not open port: " << clientPortName.c_str()  << std::endl << std::endl;
		return;
	}
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << std::endl << "ERROR: Could not connect to port: " << clientPortName.c_str() << std::endl << std::endl;		
		return;
	}
	
	
	// sending command and receiving response
	yarp::os::Bottle cmd, response;
	
	cmd.addString(msg.c_str());		// start closing of hand... cls, pre is for pregrasping ...
	port.write(cmd, response);	
		
	if( response.toString() == "OK" ) {
		std::cout << "ERROR: did not receive ok! Rcvd: " << response.toString() << std::endl;
	}
	
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
