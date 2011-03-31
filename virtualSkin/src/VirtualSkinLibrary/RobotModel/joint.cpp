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
	if (motor) { position = limits.getMin() + motor->normPos()*( limits.getMax() - limits.getMin() ); }
	else if ( limits.isTooSmall(0) ) { position = limits.getMin(); }
	else if ( limits.isTooBig(0) ) { position = limits.getMax(); }
	else { position = 0; }
	setM();
}

void Joint::setMin( qreal pos )
{
	limits.setMin(pos);
	if ( position < limits.getMin() )
	{
		position = limits.getMin();
	}
}

void Joint::setMax( qreal pos )
{
	limits.setMax(pos);
	if ( position > limits.getMax() )
	{
		position = limits.getMax();
	}
}