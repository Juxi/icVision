/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef JOINT_REVOLUTE_H
#define JOINT_REVOLUTE_H

#include "joint.h"

namespace RobotModel { class RevoluteJoint; }

/** \brief Implements Joint::setM() for a revolute (rotational) joint
 *
 */
class RobotModel::RevoluteJoint : public Joint
{
	
public:
	
	RevoluteJoint( Robot* robot, KinTreeNode* parent, Motor* motor );
	
private:
	
	virtual void setM();
	
};

#endif

/** @} */
