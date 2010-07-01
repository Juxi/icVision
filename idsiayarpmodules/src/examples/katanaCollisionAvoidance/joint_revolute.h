#ifndef JOINT_REVOLUTE_H
#define JOINT_REVOLUTE_H

#include "joint.h";

class RevoluteJoint : public Joint
{
public:
    RevoluteJoint( Robot* robot, Motor* motor );
    RevoluteJoint( KinTreeNode* parent, Motor* motor);
private:
    virtual void setM();
};

#endif // JOINT_REVOLUTE_H
