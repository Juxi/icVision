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

class RobotModel::KinTreeNode : public CompositeObject
{
	
public:
	
	enum Type { LINK,
				RJOINT,
				PJOINT };

	KinTreeNode( Robot* parentRobot, KinTreeNode* parentNode, Type type = LINK );
	virtual ~KinTreeNode();

	void setNodeAxis( const QVector3D& vector );

	KinTreeNode* parent() const { return parentNode; }
	Robot* robot() const { return parentRobot; }
	const int idx() const { return index; }
	
	const Type& getNodeType() const { return nodeType; }
	const QVector3D& getNodeAxis() const { return nodeAxis; }
	const QMatrix4x4& getM() const { return M; }

	void append( PrimitiveObject* primitive );
	bool remove( PrimitiveObject* primitive );
	
	void render();

	void filterCollisionPairs();
	void serialFilter( KinTreeNode* node, bool link = false, bool joint = false );
	
	void update( const QMatrix4x4& txfr );
	
	void print() const;
	void printAll();

protected:
	Robot*                parentRobot;
	KinTreeNode*          parentNode;
	int                   index;
	Type                  nodeType;
	QVector<KinTreeNode*> children;
	QVector3D             nodeAxis;     // joint axis or link body vector
	DisplMatrix           M;            // transformation (displacement and rotation) matrix from the next CS to this CS

	virtual void setM() = 0;

};

#endif

/** @} */
