#ifndef JOINT_PRISMATIC_H
#define JOINT_PRISMATIC_H

#include "joint.h"

class PrismaticJoint : public Joint
{
public:
    PrismaticJoint( Robot* robot, Motor* motor );
    PrismaticJoint( KinTreeNode* parent, Motor* motor);
private:
    virtual void setM();
};

#endif // JOINT_PRISMATIC_H