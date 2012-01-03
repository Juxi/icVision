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

namespace KinematicModel { class PrismaticJoint; }

/** \brief Implements Joint::setM() for a prismatic (telescoping) joint
 *
 * The prismatic joint is not yet supported by the rest of the RobotModel library...  How to define geometries such that collision detection
 * can be done in a useful way is not straightforward. Since the iCub has no prismatic joints, this is on hold
 */
class KinematicModel::PrismaticJoint : public Joint
{
	
public:
	
	PrismaticJoint( Robot* robot, KinTreeNode* parent, Motor* motor );
	
private:
	
	virtual void setM();
	
};

#endif

/** @} */
