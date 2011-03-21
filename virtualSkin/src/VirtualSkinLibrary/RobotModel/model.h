/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <QThread>
#include <QMutex>
#include <QObject>

#include <SOLID/solid.h>

#include "skinWindow.h"
#include "primitiveobject.h"
#include "robot.h"
#include "world.h"
#include "robotmodelexception.h"

namespace RobotModel
{
	class Robot;
	class World;
	class Model;
}

/*! \brief Computes the current pose of the Robot and does collision detection using the FreeSOLID library
 *
 *  Can be either in a thread or not
 *	 NOTE: Collision detection is only carried out between the robot and itself O(n^2) where n=|robot|, as well as between the robot and the world O(n) where n=|world|.
 *	 Thus the computational complexity of the collision detection grows linearly with the size of the world. 
 */

class RobotModel::Model : public QThread
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
		
		detector->col_count++;
		
		detector->collisionHandlerAddendum( object1, object2, coll_data );
	}
	
	virtual void computePosePrefix() = 0;	
	virtual void computePoseSuffix() = 0;
	virtual void collisionHandlerAddendum( PrimitiveObject*, PrimitiveObject*, const DtCollData* ) = 0;
	
signals:
	
	void newStateReady();
	void collision();

protected:
	
	RobotModel::SkinWindow	*skinWindow;
	
	bool keepRunning;
	int	 col_count;		//!< The number of collisions in the current robot/world configuration
};

#endif

/** @} */