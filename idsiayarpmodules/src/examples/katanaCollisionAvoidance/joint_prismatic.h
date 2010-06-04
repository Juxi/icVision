#ifndef JOINT_PRISMATIC_H
#define JOINT_PRISMATIC_H

#include "joint.h"

class PrismaticJoint : public Joint
{
public:
    PrismaticJoint( Motor* motor);
private:
    void setM();
};

#endif // JOINT_PRISMATIC_H
