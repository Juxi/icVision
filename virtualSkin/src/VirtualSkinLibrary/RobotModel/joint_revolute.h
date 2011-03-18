#ifndef JOINT_REVOLUTE_H
#define JOINT_REVOLUTE_H

#include "joint.h"

namespace RobotModel {

class RevoluteJoint : public Joint
{
public:
    RevoluteJoint( Robot* robot, KinTreeNode* parent, Motor* motor );
    //RevoluteJoint( KinTreeNode* parent, Motor* motor);
private:
    virtual void setM();
};
	
}

#endif
