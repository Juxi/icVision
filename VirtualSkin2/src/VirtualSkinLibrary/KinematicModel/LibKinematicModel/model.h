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

#include "robot.h"
#include "primitiveobject.h"
#include "modelexception.h"
#include "modelconstants.h"

namespace KinematicModel
{
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
	Model( bool visualize = 0, bool verbse = true );	//!< Sets the default collision response for SOLID to DT_WITNESSED_RESPONSE
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
	void appendObject( CompositeObject* );	
	CompositeObject*	removeWorldObject( CompositeObject* );
	void clearTheWorld();
	
	QVector< QString > listWorldObjects();
	CompositeObject* getObject( const QString& name );
	
	Robot*	loadRobot( const QString& fileName, bool verbose = true );
	void	loadWorld( const QString& fileName, bool verbose = true );
	
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
	
	void renderModel();
	
signals:
	
	void addedPrimitive( PrimitiveObject* );		// when this is emitted, we need to call primitive.setListPending(true)
	void removedPrimitive( GL_DisplayList );
	void computedState( int collisions );	//! Indicates that a state has been computed and reports the number of collisions
	
protected:
	
	bool keepRunning;		//!< Facilitates stopping and restarting the thread
	int	 col_count;			//!< The number of (pairwise) collisions in the current robot/world configuration
	bool encObstacle;
	bool verbose;
	
	QVector<DT_RespTableHandle> responseTables;
	
	QVector<Robot*> robots;
	QVector<CompositeObject*> world;
	
	void cleanTheWorld();
	void updateWorldState();
	void fwdKin();
	//void emitRobotStates();
	
	//Robot*				removeRobot( Robot* );
	
	void run();				//!< Allows a thread to call computePose() periodically
							/**< \note IMPORTANT: Call start() not run() !!! */
	
	
	//virtual void onStartUp() {}
	virtual void computePosePrefix() {}								//!< This is executed by computePose() just before forward kinematics is computed 
	virtual void computePoseSuffix();								//!< This is executed by computePose() just after collision detection is computed
	virtual void collisionHandlerAddendum( PrimitiveObject*,		//!< This is executed by collisionHandler() just before it returns
										   PrimitiveObject*,
										   const DT_CollData* ) {}	
	
private:
	
	ModelWindow	*modelWindow;	//! The visualization
	
	DT_SceneHandle				scene;
	
	//DT_ResponseClass worldCriticalClass;		//!< triggers reflex response against all other objects
	DT_ResponseClass obstacleClass;		//!< objects in this response class trigger reflexes
	DT_ResponseClass targetClass;		//!< these don't
	//DT_ResponseClass robotCriticalClass;		//!< triggers reflex response against all other objects
	DT_ResponseClass robotClass;		//!< these belong to the robot's body
	
	uint numObjects, numPrimitives;
	
	QMutex mutex;

	
	/*************************
	 *** COLLISION HANDLING ***
	 *************************/
	static DT_Bool collisionHandler( void* client_data, void* obj1, void* obj2, const DT_CollData *coll_data )
	{
		PrimitiveObject* prim1 = (PrimitiveObject*)obj1;
		prim1->setColliding();
		CompositeObject* comp1 = prim1->getCompositeObject();
		KinTreeNode* node1 = dynamic_cast<KinTreeNode*>(comp1);
		if ( node1 ) { node1->robot()->addCollision(); }
		
		PrimitiveObject* prim2 = (PrimitiveObject*)obj2;
		prim2->setColliding();
		CompositeObject* comp2 = prim2->getCompositeObject();
		KinTreeNode* node2 = dynamic_cast<KinTreeNode*>(comp2);
		if ( node2 ) { node2->robot()->addCollision(); }

		Model* detector = (Model*)client_data;
		detector->col_count++;
		
		detector->collisionHandlerAddendum( prim1, prim2, coll_data );
		
		return DT_CONTINUE;
	}
	
	
	static DT_Bool reflexTrigger( void* client_data, void* obj1, void* obj2, const DT_CollData *coll_data )
	{
		collisionHandler( client_data, obj1, obj2, coll_data );
		
		PrimitiveObject* prim1 = (PrimitiveObject*)obj1;
		CompositeObject* comp1 = prim1->getCompositeObject();
		KinTreeNode* node1 = dynamic_cast<KinTreeNode*>(comp1);
		if ( node1 ) { node1->robot()->addReflexCollision(); }
		
		PrimitiveObject* prim2 = (PrimitiveObject*)obj2;
		CompositeObject* comp2 = prim2->getCompositeObject();
		KinTreeNode* node2 = dynamic_cast<KinTreeNode*>(comp2);
		if ( node2 ) { node2->robot()->addReflexCollision(); }
		
		//return DT_DONE;
		return DT_CONTINUE;
	}
};

#endif

/** @} */