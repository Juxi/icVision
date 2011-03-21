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

namespace RobotModel
{
	class Robot;
	class Link;
}

/*! \brief A translation in the kinematic tree that models the robot. Intuitively, a link acts like a bone in the robot's skeleton.
 *
 */

class RobotModel::Link : public KinTreeNode
{
	
public:
	
	Link( Robot* robot, KinTreeNode* parent );
	~Link();

private:
	virtual void setM();
};

#endif

/** @} */
