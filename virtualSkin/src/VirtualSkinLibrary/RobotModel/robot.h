/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef KINTREE_H
#define KINTREE_H

#include "renderList.h"
#include "bodyPart.h"
#include "motor.h"
#include "link.h"

namespace RobotModel { class Robot; }
	
/*! \brief A kinematic model of a robot.
 *
 * The robot model consists of a tree of KinTreeNode objects of type RevoluteJoint, PrismaticJoint and Link, as well as a list of
 * Motor objects and a list of BodyPart objects. A Motor is bound to one or more joints and control the joints' position
 * according to a linear mapping. A BodyPart is a groups of motors, the positions of which can be set all at once. 
 */
class RobotModel::Robot : public RenderList
{
	Q_OBJECT

public:
	
	Robot();		//!< Nothing to do here
	~Robot();		//!< Nothing to do here
	
	bool open( const QString& fileName );			//!< Parse the XML config file and construct BodyParts, Motors, Links and RevoluteJoints to build up the robot model
	bool isOpen() const	{ return isConfigured; }	//!< Returns whether or not open( const QString& ) has been called (and has succeeded)
	void close();									//!< Delete the BodyParts, Motors, Links and RevoluteJoints, returning the Robot to the state it was in just after construction
	
	// These are generic 'get' functions that may be useful
	const QString& getName() const { return robotName; }				//!< Get the name of the Robot
	const QString* getPartName( int partNum ) const;					//!< Get the name of a BodyPart, given its index (usually for printing messages) 
	int getNumMotors( int partNum ) const;								//!< Get the number of motors in a BodyPart, given its index
	BodyPart* getPartByName( const QString& partName );					//!< Get the BodyPart itself, given its name
	const QString* getMotorName( int partNum, int motorNum ) const;		//!< Get the name of a Motor, given its index and the index of its body part
	Motor* getMotorByName( const QString& motorName );					//!< Get the moter itself, given its name
	
	void printLinks();			//!< Prints the kinematic tree depth first
	void printBodyParts();		//!< Print a list of the Motor objects in each BodyPart
	
	void updatePose();		//!< Do forward kinematics, pushing results down the link/joint trees
	void render();			//!< Recursively calls render() on the link/joint trees, updating the OpenGL display lists
	void home();			//!< Set the position of the robot to the home position (also calls updatePose())

public slots:
	void setEncoderPosition( qreal pos = 0 );							//!< Set the position of every joint on the robot.
																		/**< If the value is out of range, the maximum or minimum value will be used accordingly. */
	bool setEncoderPosition( int partNum, int motorNum, qreal pos );	//!< Set the position of a single Motor.
																		/**< If the value is out of range, the maximum or minimum value will be used accordingly.  */
	bool setEncoderPosition( int partNum, const QVector<qreal>& pos );	//!< Set the positions of each Motor in a particular BodyPart.
																		/*! Returns 0 if branchNum is out of range. 
																			\param pos A QVector of encoder positions. If one or more of these values is out of range,
																					   the maximum or minimum value will be used accordingly.
																		 Motors are numbered as they are encountered by the parser (see configure()).
																		 To see how Motors and BodyParts are numbered, try looking at the output of printJoints() and printBodyParts(). */
	
	int numBodyParts() const	{ return partList.size(); }		//!< Returns the number of BodyParts currently in the list, which is also the index of the next one to be added
	int numMotors() const		{ return motorList.size(); }	//!< Returns the number of Motors currently in the list, which is also the index of the next one to be added
	int numNodes()			{ return numLinks++; }			//!< Returns the number of KinTreeNodes currently in the list, which is also the index of the next one to be added
	
private:
	QString					robotName;	//!< Human readable identifier for the robot
	QVector<BodyPart*>		partList;	//!< "Body Parts" correspond to Yarp motor control groups such as 'torso' and 'leftArm'
	QVector<Motor*>			motorList;	//!< "Motors" serve as an interface to set the position of one or more joints
	QVector<KinTreeNode*>	tree;		//!< root nodes of the link/joint trees
	
	int						numLinks;		//!< Number of KinTreeNodes
	bool					isConfigured;	//!< Indicates whether 
	
	bool partIdxInRange( int idx ) const;					//!< Check validity of a BodyPart index
	bool motorIdxInRange( int idx, int partNum ) const;		//!< Check validity of a Motor index
	void filterCollisionPairs();							//!< Turn off collision response (via FreeSOLID) between 'adjacent pairs of objects'. See KinTreeNode.filterCollisionPairs().
	
	/** These functions are used by ZPRobotHandler to build up the Robot from an XML description.
	 * \addtogroup ToBuildTheRobot
	 *	@{
	 */
	void setName( const QString& name )		{ robotName = name; }			//!< Sets a human readable name of the robot
	void appendBodyPart( BodyPart* part )	{ partList.append(part); }		//!< Appends a BodyPart to the list
	void appendMotor( Motor* motor )		{ motorList.append(motor); }	//!< Appends a Motor to the list
	void resizeMotorList( int size )		{ motorList.resize(size); }		//!< Resizes the list of Motors
																			/**< In case you want to populate the list in reverse order */
	void appendNode( KinTreeNode* node )	{ tree.append(node); }			//!< Append a root node of a kinematic tree to the list
	/** @} */
	
	/*** FRIENDS  ***/
	friend class BodyPart;
	friend class Motor;
	friend class KinTreeNode;	// root nodes of the link/joint trees must be able to request other root nodes to filterCollisionPairs().
	friend class ZPHandler;		
};

#endif

/** @} */