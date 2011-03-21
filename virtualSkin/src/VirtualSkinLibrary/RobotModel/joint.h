/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef JOINT_H
#define JOINT_H

#include "interval.h"
#include "kintreenode.h"

namespace RobotModel
{
	class Joint;
	class Motor;
}

class RobotModel::Joint : public KinTreeNode
{
public:
    Joint( Robot* robot, KinTreeNode* parent, Motor* motor, Type type );	//!< \param robot The robot to which this joint belongs
																			//!< \param parent The parent KinTreeNode in the kinematic tree structure
																			//!< \param motor The motor that controls this joint
																			//!< \param type Indicates whether the Joint is RevoluteJoint or PrismaticJoint
    virtual ~Joint();

    void setMin( qreal pos ) { limits.setMin(pos); }	//!< Sets the minimum physical joint position (radians)
    void setMax( qreal pos ) { limits.setMax(pos); }	//!< Sets the maximum physical joint position (radians)
    void setPos();

protected:
    Motor*   motor;				//!< The motor that controls this joint
    qreal    position;			//!< The current joint position
    Interval limits;			//!< The physical limits of the joint position (radians)

    virtual void setM() = 0;	//!< Set the transformation matrix from the next CS in the kinematic tree (that of the child KinTreeNode) to this CS
};

#endif

/** @} */
