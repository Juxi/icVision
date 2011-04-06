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

/** \brief Computes the current pose of the Robot and does collision detection using the FreeSOLID library
 *
 * To use this class:
 *   First prepare the object by calling the constructor, setRobot(Robot*) and setWorld(World*).
 *   Call/activate the slot computePose() when you suspect the robot's state has changed.
 *	 Alternatively, call start() to begin doing collision detection computations periodically in a thread.
 *   When the pose has been computed and collision detection is finished, the signal newStateReady() will be emitted.
 *   If one or more collisions have occurred, the signal collision() will also be emitted.
 *
 *	 \note Although it can be used as-is, this class is intended to be extended via computePosePrefix(), computePoseSuffix() and
 *	 collisionHandlerAddendum(  PrimitiveObject*, PrimitiveObject*, const DtCollData* ). For an example of this, see VirtualSkin::YarpModel.
 *	 Also, a note on computational complexity: Collision detection is only carried out between the robot and itself O(n^2) where n=|robot|, 
 *	 as well as between the robot and the world O(n) where n=|world|. Thus the computational complexity of the collision detection grows 
 *	 linearly with the size of the world. 
 */

class RobotModel::Model : public QThread
{
	Q_OBJECT

public:
	Model( bool visualize = 0 );	//!< Sets the default collision response for FreeSOLID to DT_WITNESSED_RESPONSE
									/**< \param visualize If true, a SkinWindow (with its associated GLWidget) is constructed and its signals and slots are
										 connected to Robot and World (both of which are RenderLists) as described in the docs for GLWidget */
	virtual ~Model();				//!< Nothing special to do here
	
	SkinWindow	*skinWindow;	//! The visualization
	Robot	robot;			//!< The Robot whose pose we are updating
	World	world;			//!< All the other Objects not belonging to the Robot's body.

	bool computePose();		//!< Causes the Robot's pose to be computed in cartesian space and the collision detection to be run using FreeSOLID
							/**< Call this function directly if you want to be in control of which poses are computed when */
	
	void start() { QThread::start(); }	//!< Starts a thread to do collision detection periodically
	void stop();						//!< Stops the collision thread
	
	
	/*! This callback is executed whenever FreeSOLID encounters a collision between Objects.
	 *  It refreshes the timestamp on the objects indicating when the last collision has occurred.
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
	
	virtual void computePosePrefix() {}																	//!< This is executed by computePose() just before forward kinematics is computed 
	virtual void computePoseSuffix() {}																	//!< This is executed by computePose() just after collision detection is computed
	virtual void collisionHandlerAddendum( PrimitiveObject*, PrimitiveObject*, const DtCollData* ) {}	//!< This is executed by collisionHandler() just before it returns
	
signals:
	
	void newStateReady();	//! Indicates that a new state has been computed (useful when running collision detection in a separate thread)
	void collision();		//! Indicates that a collision has occurred (useful when running collision detection in a separate thread)

protected:
	
	
	
	bool keepRunning;	//!< Facilitates stopping and restarting the thread
	int	 col_count;		//!< The number of (pairwise) collisions in the current robot/world configuration
	
private:
	
	void run();				//!< Allows a thread to call computePose() periodically
							/**< \note IMPORTANT: Call start() not run() !!! */
};

#endif

/** @} */