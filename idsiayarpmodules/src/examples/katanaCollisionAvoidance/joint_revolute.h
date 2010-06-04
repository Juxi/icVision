#ifndef JOINT_REVOLUTE_H
#define JOINT_REVOLUTE_H

#include "joint.h";

class RevoluteJoint : public Joint
{
public:
    RevoluteJoint( Motor* motor);
private:
    void setM();
};

#endif // JOINT_REVOLUTE_H
