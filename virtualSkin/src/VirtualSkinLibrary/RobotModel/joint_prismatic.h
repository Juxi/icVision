/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef JOINT_PRISMATIC_H
#define JOINT_PRISMATIC_H

#include "joint.h"

namespace RobotModel { class PrismaticJoint; }

class RobotModel::PrismaticJoint : public Joint
{
	
public:
	
	PrismaticJoint( Robot* robot, KinTreeNode* parent, Motor* motor );
	
private:
	
	virtual void setM();
	
};

#endif

/** @} */
