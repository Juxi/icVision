#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <QThread>
#include <QMutex>
#include <QObject>
#include <SOLID/solid.h>
#include "primitiveobject.h"
#include "robot.h"
#include "world.h"
#include "robotmodelexception.h"

namespace RobotModel {
	
	class Robot;
	class World;

/*! \brief Computes the current pose of the Robot and does collision detection using the FreeSOLID library
 *
 * To use this class:
 *   First prepare the object by calling the constructor, setRobot(Robot*) and setWorld(World*).
 *   If you want to publish collision information on the network via YARP, call setPortName(QString&) and openPort().
 *   Call/activate the slot computePose() when you suspect the robot's state has changed.
 *   When the pose has been computed and collision detection is finished, the signal newPoseReady() will be emitted.
 *   If one or more collisions have occurred, the signal crash() will also be emitted, provided that the CollisionDetector is armed.
 *	 Finally, use the slots armDetector() and disarmDetector() to control whether or not to emit the crash() signal if a collision has occurred.
 *   This is useful if the crash() signal triggers some kind of collision handling control code, as you can supress subsequent crash signals until the collision handling is complete.
 *
 *	 NOTE: Collision detection is only carried out between the robot and itself O(n^2) where n=|robot|, as well as between the robot and the world O(n) where n=|world|.
 *	 Thus the computational complexity of the collision detection grows linearly with the size of the world. 
 */

class Model : public QThread
{
	Q_OBJECT

public:
	Model( bool visualize = 0 );	//!< Constructs an armed collisionDetector object
	virtual ~Model();				//!< Just stops the YARP port if its running
	
	Robot	robot;			//!< The Robot whose pose we are updating
	World	world;			//!< All the other Objects not belonging to the Robot's body. (we just need this so we can change colors when things collide or not)

	bool computePose();					//!< Causes the Robot's pose to be computed in cartesian space and the collision detection to be run using FreeSOLID
	void run();
	void stop();
	void restart();
	
	/*! This callback is executed whenever FreeSOLID encounters a collision between Objects.
	 It sets a flag that collision(s) has/have occurred, and encodes the collision state into a YARP bottle.
	 */
	static void collisionHandler( void * client_data, DtObjectRef obj1, DtObjectRef obj2, const DtCollData *coll_data )
	{
		PrimitiveObject* object1 = (PrimitiveObject*)obj1;
		PrimitiveObject* object2 = (PrimitiveObject*)obj2;
		object1->setColliding();
		object2->setColliding();
		
		Model* detector = (Model*)client_data;
		
		//detector->crashed = true;
		detector->col_count++;
		
		detector->collisionHandlerAddendum( object1, object2, coll_data );
	}
	
	virtual void collisionHandlerAddendum( PrimitiveObject*, PrimitiveObject*, const DtCollData* ) = 0;
	
signals:
	
	void newStateReady();
	void collision();

private:
	
    SkinWindow	*skinWindow;
	bool		keepRunning;
	//bool		crashed;		//!< Whether or not the current configuration has at least one pair of colliding objects
	int			col_count;		//!< The number of collisions in the current robot/world configuration
};
}

#endif
