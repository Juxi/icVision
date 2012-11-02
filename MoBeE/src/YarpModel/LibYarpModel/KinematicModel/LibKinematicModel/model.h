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
#include <QReadWriteLock>
#include <SOLID.h>

#include "robot.h"
#include "primitiveobject.h"
#include "modelexception.h"
#include "modelconstants.h"
#include "objectmover.h"

namespace KinematicModel
{
	class Model;
	class ModelWindow;
	class ObjectMover;
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
 *	 collisionHandlerAddendum(  PrimitiveObject*, PrimitiveObject*, const DtCollData* ). For an example of this, see MoBeE::YarpModel.
 *	 Also, a note on computational complexity: Collision detection is only carried out between the robot and itself O(n^2) where n=|robot|, 
 *	 as well as between the robot and the world O(n) where n=|world|. Thus the computational complexity of the collision detection grows 
 *	 linearly with the size of the world. 
 */

class KinematicModel::Model : public QThread
{
	Q_OBJECT

public:
	Model( bool visualize = 0, bool verbse = false );	//!< Sets the default collision response for SOLID to DT_WITNESSED_RESPONSE
									/**< \param visualize If true, a SkinWindow (with its associated GLWidget) is constructed and its signals and slots are
										 connected to Robot and World (both of which are RenderLists) as described in the docs for GLWidget */
	virtual ~Model();				//!< Nothing special to do here
	
	int computePose();						//!< Causes the Robot's pose to be computed in cartesian space and the collision detection to be run using SOLID
											/**< Call this function directly if you want to be in control of which poses are computed when */
	void stop();
	
	void grabObject( CompositeObject* object, Robot* robot, int markerIndex );
	void appendObject( KinTreeNode* );
	void appendObject( CompositeObject* );	
	
	void clearTheWorld();
	void clearWorldObject( CompositeObject* );
	void setStopOnFirstCollision(bool b) { stopOnFirstCollision = b ? DT_DONE : DT_CONTINUE; }
	DT_Bool getStopOnFirstCollision() { return stopOnFirstCollision; };
	void setSyncGraphics(bool s) { syncGraphics = s; };
	bool getSyncGraphics() { return syncGraphics; };

	QVector< QString > listWorldObjects();
	CompositeObject* getObject( const QString& name );
	Robot* getRobot( const QString& name );
	
	Robot*	loadRobot( const QString& fileName, bool verbose = true );
	void	loadWorld( const QString& fileName, bool verbose = true );
	
	DT_SceneHandle		getScene() const { return scene; }
	//DT_RespTableHandle	getResponseTable( int i ) const { return responseTables.at(i); }
	
	// collision response classes for the world table
	DT_ResponseClass	OBSTACLE() const { return obstacleClass; }
	DT_ResponseClass	TARGET() const { return targetClass; }
	DT_ResponseClass	GHOST() const { return ghostClass; }
	DT_ResponseClass	FIELD() const { return fieldClass; }
	
public slots:
	
	void renderModel();
	
signals:
	
	void addedPrimitive( PrimitiveObject* );		// when this is emitted, we need to call primitive.setListPending(true)
	void removedPrimitive( GL_DisplayList );
	void computedState( int collisions );	//! Indicates that a state has been computed and reports the number of collisions
	
protected:

	DT_RespTableHandle newRobotTable();
    DT_RespTableHandle newRobotFieldTable();
	DT_ResponseClass newResponseClass( DT_RespTableHandle );
	
	bool removeWorldObject( CompositeObject* );

	void removeReflexResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 );
	void removeVisualResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 );
    void removeForceResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 );
	void removeAllResponses( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 );
	//void setVisualResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 );

	void cleanTheWorld();
	void updateWorldState();
	void updateSolid();
	void evaluateRobotConstraints();
	void computeCollisions();

	void run();				//!< Allows a thread to call computePose() periodically
							/**< \note IMPORTANT: Call start() not run() !!! */
	
	//virtual void onStartUp() {}
	virtual void computePosePrefix() {}								//!< This is executed by computePose() just before forward kinematics is computed 
	virtual void computePoseSuffix();								//!< This is executed by computePose() just after collision detection is computed
	virtual void collisionHandlerAddendum( PrimitiveObject*,		//!< This is executed by collisionHandler() just before it returns
										   PrimitiveObject*,
										   const DT_CollData* ) {}	
	
	bool keepRunning;		//!< Facilitates stopping and restarting the thread
	int	 col_count;			//!< The number of (pairwise) collisions in the current robot/world configuration
	int	 reflex_col_count;
	bool encObstacle;
	bool verbose;
	bool syncGraphics;      //!< When true, the graphics and the model thread wait for each other. This makes the model thread slow, because it has to wait until the graphics thread finishes drawing. When false, the model thread runs without waiting for the graphics thread, which might cause the graphics thread to show dislocated joints.

    DT_RespTableHandle worldTable;
	//QVector<DT_RespTableHandle> responseTables;		//!< Table 0 describes each robot w.r.t the world and the other robots. The rest are for robots' self-collision
	QVector<DT_ResponseClass> robotResponseClasses;
	//QVector<DT_ResponseClass> fieldResponseClasses;
	//QVector<DT_ResponseClass> robotBaseClasses;
	
	QVector<Robot*> robots;
	QVector<CompositeObject*> world;
	
	ModelWindow	*modelWindow;	//! The visualization
	
	DT_SceneHandle		scene;
	DT_ResponseClass	obstacleClass;	//!< objects in this response class trigger reflexes
	DT_ResponseClass	targetClass;	//!< these don't
	DT_ResponseClass	ghostClass;		//!< these are left out of collision detection computations
    DT_ResponseClass    fieldClass;
	
	uint numObjects, numPrimitives;
	
	QReadWriteLock mutex;
	
	ObjectMover *objectMover;			//!< Object mover

	friend class Robot;
	friend class KinTreeNode;

	DT_Bool stopOnFirstCollision;
	
	
	/*************************
	 *** COLLISION HANDLING ***
	 *************************/
    
    // handles the changing of colors to indicate collission status
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
		
		return detector->getStopOnFirstCollision();
	}
	
    // triggers robot reflexes
	static DT_Bool reflexTrigger( void* client_data, void* obj1, void* obj2, const DT_CollData *coll_data )
	{
		PrimitiveObject* prim1 = (PrimitiveObject*)obj1;
		CompositeObject* comp1 = prim1->getCompositeObject();
		KinTreeNode* node1 = dynamic_cast<KinTreeNode*>(comp1);
		if ( node1 ) { node1->robot()->addReflexCollision(); }
		
		PrimitiveObject* prim2 = (PrimitiveObject*)obj2;
		CompositeObject* comp2 = prim2->getCompositeObject();
		KinTreeNode* node2 = dynamic_cast<KinTreeNode*>(comp2);
		if ( node2 ) { node2->robot()->addReflexCollision(); }
		
		Model* detector = (Model*)client_data;
		detector->reflex_col_count++;
		
		//return DT_DONE;
		return detector->getStopOnFirstCollision();
	}
    
    // creates repulsive forces to prevent unwanted collisions
    static DT_Bool repel( void* client_data, void* obj1, void* obj2, const DT_CollData *coll_data )
	{
        //printf("The Callback is calling!!!\n");
		// compute repuslive fictitous force and project it into the joint space
     
        if (!coll_data) return DT_CONTINUE;
        
        QVector3D p1( coll_data->point1[0], coll_data->point1[1], coll_data->point1[2]);
        QVector3D p2( coll_data->point2[0], coll_data->point2[1], coll_data->point2[2]);
        QVector3D F2( coll_data->normal[0], coll_data->normal[1], coll_data->normal[2]);
        
        F2 *= F2.length()*F2.length() * 50;
        QVector3D F1 = -F2;
        QVector3D T1 = QVector3D::crossProduct(p1, F1);
        QVector3D T2 = QVector3D::crossProduct(p2, F2);
        
        PrimitiveObject* prim1 = (PrimitiveObject*)obj1;
		CompositeObject* comp1 = prim1->getCompositeObject();
		KinTreeNode* node1 = dynamic_cast<KinTreeNode*>(comp1);
		if ( node1 )
            node1->computeJacobian(F1,T1);
		
		PrimitiveObject* prim2 = (PrimitiveObject*)obj2;
		CompositeObject* comp2 = prim2->getCompositeObject();
		KinTreeNode* node2 = dynamic_cast<KinTreeNode*>(comp2);
		if ( node2 )
            node2->computeJacobian(F2,T2);
        
		return DT_CONTINUE;
	}
};

#endif

/** @} */