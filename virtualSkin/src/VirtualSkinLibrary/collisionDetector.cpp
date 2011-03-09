#include "collisionDetector.h"
#include "constants.h"

using namespace VirtualSkin;

CollisionDetector::CollisionDetector() : robot(NULL), col_count(0), armed(false), crashed(false)
{
	dtSetDefaultResponse( collisionHandler, DT_WITNESSED_RESPONSE, (void*) this);
	solidPort.setBottle(bottle);
}
CollisionDetector::~CollisionDetector()
{
	if ( solidPort.isRunning() ) { solidPort.stop(); }
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
	
	//robot->notColliding();   // reset collision flags
	//world->notColliding();
	
	// reset the collision flag
	crashed = false;
	
	// empty the bottle that contains collision information
	bottle.clear();
	
	// reset the collision counter
	col_count = 0;

	//mutex.lock();
		robot->updatePose();	// do forward kinematics and update the pose of the robot in cartesian space
		dtTest();				// do collision detection
	//mutex.unlock();
	
	// publish the results of dtTest() via YARP
	if ( !crashed ) { bottle.addInt(0); }
	solidPort.setBottle(bottle);
	
	// if there was a collision emit the crash() signal (to RobotFilter)
	if ( /*armed &&*/ crashed ) {
		printf("%i COLLISION(S) DETECTED!!!\n", col_count);
		if ( armed ) { return 0; }
	}

	return 1;
}
