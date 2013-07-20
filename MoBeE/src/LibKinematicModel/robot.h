/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef ROBOT_H
#define ROBOT_H

#include <QMutex>
//#include "renderList.h"
#include "bodypart.h"
#include "motor.h"
#include "link.h"
#include "marker.h"
#include "robotobservation.h"
#include "modelexception.h"


namespace KinematicModel
{ 
	class Model;
	class Robot;
}
	
/*! \brief A kinematic model of a robot.
 *
 * The robot model consists of a tree of KinTreeNode objects of type RevoluteJoint, PrismaticJoint and Link, as well as a list of
 * Motor objects and a list of BodyPart objects. A Motor is bound to one or more joints and control the joints' position
 * according to a linear mapping. A BodyPart is a groups of motors, the positions of which can be set all at once. 
 */
class KinematicModel::Robot : public QObject
{
	Q_OBJECT

public:
	
	Robot( Model* m,
          DT_RespTableHandle roobtTable,
          DT_RespTableHandle fieldTable,
          DT_ResponseClass robotClass,
          DT_ResponseClass baseClass,
          DT_ResponseClass robotField,
          DT_ResponseClass robotBaseField //,bool openWithField = true
          );
	~Robot();														
	
	//bool Robot::isColliding() const;
	
	void open(const QString& fileName, bool verbose = true) throw(KinematicModelException);	//!< Parse the XML config file and construct BodyParts, Motors, Links and RevoluteJoints to build up the robot model
	void appendMarkersToModel();
	bool isOpen() const	{ return isConfigured; }											//!< Returns whether or not open( const QString& ) has been called (and has succeeded)
	void close();																			//!< Delete the BodyParts, Motors, Links and RevoluteJoints, returning the Robot to the state it was in just after construction
	
    //bool hasField() { return openWithField; }
    
	void addCollision() { numCollisions++; }
	void addReflexCollision() { numReflexCollisions++; }
	
    void resetExtTorque();
	//RobotObservation observe();
	void evaluateConstraints();
	
	virtual void publishState();										// emit signals for observations and reflexes
	
	// generic 'get' functions that may be useful
	Model*				getModel() const { return model; }
	DT_RespTableHandle	getResponseTable() { return responseTable; }
	DT_RespTableHandle	getFieldResponseTable() { return fieldResponseTable; }
	DT_ResponseClass	getWorldRobotClass() { return worldRobotClass; }
	DT_ResponseClass	getWorldBaseClass() { return worldBaseClass; }
    DT_ResponseClass	getWorldFieldClass() { return worldFieldClass; }
    DT_ResponseClass	getWorldBaseFieldClass() { return worldBaseFieldClass; }
    

	const QString&	getName() const { return robotName; }				//!< Get the name of the Robot
	const QString*	getPartName( int partNum ) const;					//!< Get the name of a BodyPart, given its index (usually for printing messages) 
	const QString*	getMotorName( int partNum, int motorNum ) const;		//!< Get the name of a Motor, given its index and the index of its body part
	
	BodyPart*		getPart( int partNum ) const { return partList.at(partNum); }
	BodyPart*		getPartByName( const QString& partName );					//!< Get the BodyPart itself, given its name
	Motor*			getMotorByName( const QString& motorName );					//!< Get the moter itself, given its name
	
	int				getNumMotors( int partNum ) const;								//!< Get the number of motors in a BodyPart, given its index

	int	numBodyParts() const { return partList.size();}		//!< Returns the number of BodyParts currently in the list, which is also the index of the next one to be added
	int numMotors() const { return motorList.size(); }	//!< Returns the number of Motors currently in the list, which is also the index of the next one to be added
	

signals:

	void collisions(int);
	void reflexCollisions(int);
	//void observation( RobotObservation obs );							//!< make new marker positions and orientations known

public slots:
	
	void setNormalPosition( qreal pos = 0.0 );							//!< Set the position of every joint on the robot.
																		/**< Position must be on the interval [0,1], if it is out of range, 0 or 1 will be used */
	bool setNormalPosition( int partNum, int motorNum, qreal pos );		//!< Set the position of a single Motor.
																		/**< If the value is out of range, the maximum or minimum value will be used accordingly.  */
	void setEncoderPosition( qreal pos = 0.0 );							//!< Set the position of every joint on the robot.
																		/**< If the value is out of range, the maximum or minimum value will be used accordingly. */
	bool setEncoderPosition( int partNum, int motorNum, qreal pos );	//!< Set the position of a single Motor.
																		/**< If the value is out of range, the maximum or minimum value will be used accordingly.  */
	bool setEncoderPosition( int partNum, const QVector<qreal>& pos );	//!< Set the positions of each Motor in a particular BodyPart.
																		/*! Returns 0 if branchNum is out of range. 
																			\param pos A QVector of encoder positions. If one or more of these values is out of range,
																					   the maximum or minimum value will be used accordingly.
																		 Motors are numbered as they are encountered by the parser (see configure()).
																		 To see how Motors and BodyParts are numbered, try looking at the output of printJoints() and printBodyParts(). */
	
	qreal EncoderToNormalPosition( int partNum, int motorNum, qreal pos );	//!< Convert between normal [0-1] to real encoder positions for motors
	qreal NormalToEncoderPosition( int partNum, int motorNum, qreal pos );



	void updatePose();					//!< Do forward kinematics, pushing results down the link/joint trees
	
	void home(bool verbose = false);		//!< Set the position of the robot to the home position (also calls updatePose())
	void ignoreAdjacentPairs();							//!< Turn off collision response (via SOLID) between 'adjacent pairs of objects'. See KinTreeNode.ignoreAdjacentPairs().
	//void appendTreeToModel( KinTreeNode* node = NULL );
	
private:
	Model*					model;				//!< The Model that is doing collision detection on this Robot
	DT_RespTableHandle		responseTable,		//!< For managing self-collisions
                            fieldResponseTable; //!< For managing self-repulsion
	DT_ResponseClass		worldRobotClass,	//!< For checking the robot w.r.t the world and other robots
                            worldBaseClass,		//!< So as not to check the robot's base against the world    (so a stand can intersect the table for example)
                            worldFieldClass,
                            worldBaseFieldClass;
	
	QString					robotName;		//!< Human readable identifier for the robot
	QVector<BodyPart*>		partList;		//!< "Body Parts" correspond to Yarp motor control groups such as 'torso' and 'leftArm'
	QVector<Motor*>			motorList;		//!< "Motors" serve as an interface to set the position of one or more joints
	QVector<KinTreeNode*>	tree;			//!< root nodes of the link/joint trees
	QVector<Marker*>		markers;		//!< list of markers
	int						numCompositObjects;		//!< Number of KinTreeNodes
	bool					isConfigured;	//!<
    //bool                    openWithField;
	
	int						numCollisions;
	int						numReflexCollisions;
    QVector< QVector3D >    force;
	
	QMutex mutex;
	
    // just for debugging
    void countCompositeObject() { ++numCompositObjects; }	
    
	bool partIdxInRange( int idx ) const;					//!< Check validity of a BodyPart index
	bool motorIdxInRange( int idx, int partNum ) const;		//!< Check validity of a Motor index
	
	
	//void removeCollisionResponse( DT_ResponseClass c, DT_RespTableHandle t ); //!< Turn off collision response to class c in table t (for the whole robot)
	
	void setName( const QString& name )		{ robotName = name; }			//!< Sets a human readable name of the robot
	void appendBodyPart( BodyPart* part )	{ part->setIndex(partList.size()); partList.append(part); }		//!< Appends a BodyPart to the list
	void appendMotor( Motor* motor )		{ motorList.append(motor); }	//!< Appends a Motor to the list
	void resizeMotorList( int size )		{ motorList.resize(size); }		//!< Resizes the list of Motors
	void appendNode( KinTreeNode* node );									//!< Append a root node of a kinematic tree to the list																		/**< In case you want to populate the list in reverse order */
	
	int getNumPrimitives();
	void kill();
	
	/*** FRIENDS  ***/
	friend class BodyPart;
	friend class Motor;
	friend class KinTreeNode;	// root nodes of the link/joint trees must be able to request other root nodes to ignoreAdjacentPairs().
	friend class ZPHandler;		
};

#endif

/** @} */
