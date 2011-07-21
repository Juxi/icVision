/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef KINEMATICMODEL_H
#define KINEMATICMODEL_H

#include <QThread>
#include <QMutex>
#include <SOLID.h>

#include "compositeobject.h"
#include "primitiveobject.h"
#include "modelexception.h"
#include "modelconstants.h"

namespace KinematicModel
{
	class Robot;
	class KinTreeNode;
	//class World;
	class Model;
	class ModelWindow;
}

/** \brief Computes the current pose of the Robot and does collision detection using the SOLID library
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

class KinematicModel::Model : public QThread
{
	Q_OBJECT

public:
	Model( bool visualize = 0, bool v = true );	//!< Sets the default collision response for SOLID to DT_WITNESSED_RESPONSE
									/**< \param visualize If true, a SkinWindow (with its associated GLWidget) is constructed and its signals and slots are
										 connected to Robot and World (both of which are RenderLists) as described in the docs for GLWidget */
	virtual ~Model();				//!< Nothing special to do here

	DT_RespTableHandle newRobotTable();
	DT_ResponseClass newResponseClass( DT_RespTableHandle );
	void removePairResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 );
	
	int computePose();						//!< Causes the Robot's pose to be computed in cartesian space and the collision detection to be run using SOLID
											/**< Call this function directly if you want to be in control of which poses are computed when */
	void stop();
	
	//void appendRobotObject( CompositeObject* );
	void appendObject( KinTreeNode* );
	void appendObject( CompositeObject* );	// to remove objects use CompositeObject.kill()
	void loadRobot( const QString& fileName, bool verbose = true );
	
	DT_SceneHandle		getScene() const { return scene; }
	DT_RespTableHandle	getResponseTable( int i ) const { return responseTables.at(i); }
	//DT_RespTableHandle	getRobotTable() const { return robotTable; }
	
	// collision response classes for the world table
	//DT_ResponseClass	WORLD_CRITICAL() const { return worldCriticalClass; }
	DT_ResponseClass	OBSTACLE() const { return obstacleClass; }
	DT_ResponseClass	TARGET() const { return targetClass; }
	DT_ResponseClass	ROBOT() const { return robotClass; }
	//DT_ResponseClass	ROBOT_CRITICAL() const { return robotCriticalClass; }
	
	
	//DT_RespTableHandle	getRobotTable() { return robotTable; }
	//DT_ResponseClass	body_partResponseClass() { return BODY_PART; }
	
public slots:
	
	//void displayListArrived( PrimitiveObject* );
	void renderWorld();
	
signals:
	
	void addedPrimitive( PrimitiveObject* );		// when this is emitted, we need to call primitive.setListPending(true)
	void removedPrimitive( GL_DisplayList );
	
	void computedState( int collisions );	//! Indicates that a state has been computed and reports the number of collisions
	void encounteredObstacle();
	
protected:
	
	bool keepRunning;		//!< Facilitates stopping and restarting the thread
	int	 col_count;			//!< The number of (pairwise) collisions in the current robot/world configuration
	bool encObstacle;
	bool verbose;
	
	QVector<Robot*> robots;
	QVector<CompositeObject*> world;
	
	void cleanTheWorld();
	void updateWorldState();
	void fwdKin();
	
	//Robot*				removeRobot( Robot* );
	CompositeObject*	removeWorldObject( CompositeObject* );
	void run();				//!< Allows a thread to call computePose() periodically
							/**< \note IMPORTANT: Call start() not run() !!! */
	
	
	virtual void computePosePrefix() {}								//!< This is executed by computePose() just before forward kinematics is computed 
	virtual void computePoseSuffix() {}								//!< This is executed by computePose() just after collision detection is computed
	virtual void collisionHandlerAddendum( PrimitiveObject*,		//!< This is executed by collisionHandler() just before it returns
										   PrimitiveObject*,
										   const DT_CollData* ) {}	
	
private:
	
	ModelWindow	*modelWindow;	//! The visualization
	
	DT_SceneHandle				scene;
	QVector<DT_RespTableHandle> responseTables;
	
	//DT_ResponseClass worldCriticalClass;		//!< triggers reflex response against all other objects
	DT_ResponseClass obstacleClass;		//!< objects in this response class trigger reflexes
	DT_ResponseClass targetClass;		//!< these don't
	//DT_ResponseClass robotCriticalClass;		//!< triggers reflex response against all other objects
	DT_ResponseClass robotClass;		//!< these belong to the robot's body
	
	QMutex mutex;

	
	/*************************
	 *** COLLISION HANDLING ***
	 *************************/
	static DT_Bool collisionHandler( void* client_data, void* obj1, void* obj2, const DT_CollData *coll_data )
	{
		PrimitiveObject* object1 = (PrimitiveObject*)obj1;
		PrimitiveObject* object2 = (PrimitiveObject*)obj2;
		object1->setColliding();
		object2->setColliding();
		
		Model* detector = (Model*)client_data;
		detector->col_count++;
		detector->collisionHandlerAddendum( object1, object2, coll_data );
		
		return DT_CONTINUE;
	}
	
	
	static DT_Bool reflexTrigger( void* client_data, void* obj1, void* obj2, const DT_CollData *coll_data )
	{
		collisionHandler( client_data, obj1, obj2, coll_data );
		Model* detector = (Model*)client_data;
		detector->encObstacle = true;
		
		//return DT_DONE;
		return DT_CONTINUE;
	}
};

#endif

/** @} */