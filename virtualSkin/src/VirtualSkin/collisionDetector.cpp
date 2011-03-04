#include "collisionDetector.h"
#include "constants.h"

using namespace VirtualSkin;

CollisionDetector::CollisionDetector() : robot(NULL), col_count(0), armed(true), crashed(false)
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
void CollisionDetector::arm()
{ 
	robot->notColliding();   // reset collision flags
	world->notColliding();
	
	armed = true;
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
	
	// publish the results of dtTest() via YARP
	if ( !crashed ) { bottle.addInt(0); }
	solidPort.setBottle(bottle);
	
	// if there was a collision emit the crash() signal (to RobotFilter)
	if ( armed && crashed ) {
		printf("COLLISION DETECTED!!!\n");
		return 0;
	}

	return 1;
}
