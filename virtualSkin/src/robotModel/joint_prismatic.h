#ifndef JOINT_PRISMATIC_H
#define JOINT_PRISMATIC_H

#include "joint.h"

namespace RobotModel {

class PrismaticJoint : public Joint
{
public:
    PrismaticJoint( Robot* robot, KinTreeNode* parent, Motor* motor );
    //PrismaticJoint( KinTreeNode* parent, Motor* motor);
private:
    virtual void setM();
};
	
}

#endif
