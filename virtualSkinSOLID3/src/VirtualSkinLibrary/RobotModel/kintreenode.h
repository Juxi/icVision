/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef KINTREENODE_H
#define KINTREENODE_H

#include <QtGui>
#include "compositeObject.h"

namespace RobotModel
{
	class PrimitiveObject;
	class Robot;
	class KinTreeNode;
}

/** \brief The building block of the kinematic tree structure.
 *
 * This class is abstract and has three implementations: Link, RevoluteJoint and PrismaticJoint. It inherits CompositeObject so that the kinematic tree 
 * can represent geometries and their kinematic constraints naturally.
 */
class RobotModel::KinTreeNode : public CompositeObject
{
	
public:
	
	enum Type { 
				LINK,	//!< See Link
				RJOINT,	//!< See RevoluteJoint
				PJOINT	//!< See PrismaticJoint
			   };		//!< Indicates the sub-type of this object

	KinTreeNode( Robot* parentRobot, KinTreeNode* parentNode, Type type = LINK );	//!< Initializes member variables and appends the KinTreeNode to the supplied Robot and KinTreeNode
																					/**< \param parentRobot The robot to which the KinTreeNode belongs
																						 \param parentNode The parent of the KinTreeNode in the kinematic tree structure */
	virtual ~KinTreeNode();															//!< First delete the child KinTreeNodes and PrimitiveObjects associated with this KinTreeNode
																					/**< Also emit a signal to the graphics thread that it should delete all relevant display lists */

	void setNodeAxis( const QVector3D& vector );									//!< Sets the value of a vector that represents either a body vector (like a bone in a skeleton) or a joint axis. See nodeAxis.

	KinTreeNode* parent() const { return parentNode; }								//!< Returns the parent KinTreeNode of this object in the kinematic tree structure
	Robot* robot() const { return parentRobot; }									//!< Returns the Robot to which this KinTreeNode belongs
	const int idx() const { return index; }											//!< Returns the index of this KinTreeNode object. The index is unique among KinTreeNodes belonging to the same Robot
	
	const Type& getNodeType() const { return nodeType; }							//!< Returns the sub-type of the object (Link, RevoluteJoint or PrisMaticJoint)
	const QVector3D& getNodeAxis() const { return nodeAxis; }						//!< Returns nodeAxis
	const QMatrix4x4& getM() const { return M; }									//!< Returns the transformation matrix between the next coordinate system in the tree and this one

	void append( PrimitiveObject* primitive );										//!< Appends a PrimitiveObject to the KinTreeNode, which is also a CompositeObject
	bool remove( PrimitiveObject* primitive );										//!< Removes a PrimitiveObject from the KinTreeNode, which is also a CompositeObject
	
	void render();																	//!< Calls CompositeObject::render() to render the primitives in this KinTreeNode, then calls KintreeNode::render() recursively on children
	
	void print() const;																//!< Prints the member variables to the terminal
	void printAll();																//!< Recursively calls print() on all nodes

protected:
	Robot*                parentRobot;
	KinTreeNode*          parentNode;
	int                   index;
	Type                  nodeType;
	QVector<KinTreeNode*> children;
	QVector3D             nodeAxis;     /**< The meaning of nodeAxis depends on the sub-type of the object. If the object is a Link, nodeAxis represents
											 a body vector, a displacement between two joint axes (like a bone in a skeleton), and both magnitude and direction
											 are significant. Otherwise if the object is a PrismaticJoint or a RevoluteJoint, nodeAxis represents the joint axis,
											 and only direction is significant. */
	DisplMatrix           M;            // transformation (displacement and rotation) matrix from the next CS to this CS

	virtual void setM() = 0;														//!< Sets the local transformation from the next coordinate system to this coordinate system as a function of nodeAxis
																					/**< Implemented by Link, RevoluteJoint and PrismaticJoint */
	void serialFilter( KinTreeNode* node, bool link = false, bool joint = false );	//!< Recursively decends serial chains eliminating SOLID collision response to collisions between 'adjacent' geometries
																					/**< \param node The KinTreeNode with respect to which we are filtering as we decend the serial chain.
																						 The function is called on a node (that node's member function is called) with respect to itself (the same node 
																						 is passed as a parameter) to decend the serial chain associated with that node. Alternatively, it can be called on a 
																						 node with respect to one of its siblings (pass a pointer to the node to the member function of the sibling) to decend 
																						 the serial chain of the siblings and avoid erroneous collisions at branching nodes. These calls are made by filterCollisionPairs()
																						 and Robot.filterCollisionPairs(). */
	void filterCollisionPairs();													//!< Recursively parses the tree, calling serialFilter( KinTreeNode* ) on each KinTreeNode with respect to itself and with respect to its siblings
	void update( const QMatrix4x4& txfr );											//!< Propogates forward kinematics calculations down the tree
																					/**< Called by Robot.updatePose() */

	friend class Robot;		//!< Robot calls filterCollisionPairs() and update()
};

#endif

/** @} */
