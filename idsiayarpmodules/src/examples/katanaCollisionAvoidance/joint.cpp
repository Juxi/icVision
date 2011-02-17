#include "kintreenode.h"
#include "joint.h"
#include "motor.h"

Joint::Joint( Robot* robot, Motor* motor, KinTreeNode::Type type ) : KinTreeNode(robot,type), motor(motor), position(0)
{
    limits = Interval();
    motor->append(this);
}
Joint::Joint( KinTreeNode* parent, Motor* motor, KinTreeNode::Type type ) : KinTreeNode(parent,type), motor(motor), position(0)
{
    limits = Interval();
    motor->append(this);
}
Joint::~Joint(){}
void Joint::setPos()
{
    position = limits.getMin() + motor->normPos()*( limits.getMax() - limits.getMin() );
    setM();
}
