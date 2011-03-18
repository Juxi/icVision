#include "joint.h"
#include "motor.h"
#include "robotmodelexception.h"

using namespace RobotModel;

Joint::Joint( Robot* robot, KinTreeNode* parent, Motor* motor, KinTreeNode::Type type ) : KinTreeNode(robot,parent,type), motor(motor), position(0)
{
	if ( !motor ) { throw RobotModelException("The Joint constructor requires a pointer to a valid motor."); }
	
    limits = Interval();
    motor->append(this);
}
Joint::~Joint()
{
}
void Joint::setPos()
{
	//mutex.lock();
		position = limits.getMin() + motor->normPos()*( limits.getMax() - limits.getMin() );
		setM();
	//mutex.unlock();
}
