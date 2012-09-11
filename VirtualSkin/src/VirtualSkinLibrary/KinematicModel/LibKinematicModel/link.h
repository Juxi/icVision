/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef LINK_H
#define LINK_H

#include "kintreenode.h"

namespace KinematicModel
{
	class Robot;
	class Link;
}

/*! \brief A translation in the kinematic tree that models the robot.
 *
 * Intuitively, a link acts like a bone in the robot's skeleton.
 */

class KinematicModel::Link : public KinTreeNode
{
	
public:
	
	Link( /*DT_ResponseClass nodeResponseClass,*/ Robot* robot, int part, KinTreeNode* parent );	//!< Calls the KinTreeNode constructor
	~Link();									//!< Nothing special to do here

private:
	
	virtual void setM();						//!< Sets the local transformation to be translation (only) in the direction and magnitude of KinTreeNode.nodeAxis
	
};

#endif

/** @} */
