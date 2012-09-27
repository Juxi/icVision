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

namespace KinematicModel { class RevoluteJoint; }

/** \brief Implements Joint::setM() for a revolute (rotational) joint
 *
 */
class KinematicModel::RevoluteJoint : public Joint
{
	
public:
	
	RevoluteJoint( /*DT_ResponseClass nodeResponseClass,*/ Robot* robot, int part, KinTreeNode* parent, Motor* motor );
	
private:
	
	virtual void setM();
	
};

#endif

/** @} */
