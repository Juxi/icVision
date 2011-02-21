#include "collisionDetector.h"
#include "constants.h"

using namespace RobotModel;

CollisionDetector::CollisionDetector() : robot(NULL), col_count(0), isArmed(true), crashed(false)
{
	dtSetDefaultResponse( collisionHandler, DT_WITNESSED_RESPONSE, (void*) this);
}
CollisionDetector::~CollisionDetector()
{
	if ( solidPort.isRunning() ) { solidPort.stop(); }
}

void CollisionDetector::openPort()
{
	solidPort.start();
}
void CollisionDetector::armDetector()
{ 
	robot->notColliding();   // reset collision flags
	world->notColliding();
	
	isArmed = true;
}

bool CollisionDetector::computePose()
{
	if ( !robot )
	{
		printf("COLLISION DETECTOR ERROR: the robot has not been set...  call setRobot(Robot&) before computePose()\n");
		return 0;
	}

	// do forward kinematics and update the pose of the robot in cartesian space
	robot->updatePose();
	
	// reset the collision flag
	crashed = false;
	
	// empty the bottle that contains collision information
	bottle.clear();
	
	// reset the collision counter
	col_count = 0;
	
	// do collision detection
	dtTest();
	
	// if there was a collision emit the crash() signal (to RobotFilter)
	if ( isArmed && crashed ) {
		printf("COLLISION DETECTED!!!\n");
		emit crash();	
	}
	else bottle.addInt(0);					// never try to send a null bottle
	
	solidPort.setBottle(bottle);			// publish the results of dtTest() via YARP

	// emit a signal to render (to GLWidget)
	emit newPoseReady();
	
	//if (col_count > 0) printf("Number of collisions: %d\n", col_count);
	
	return 1;
}
