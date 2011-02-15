#include "joint_revolute.h"
#include "displmatrix.h"

using namespace RobotModel;

RevoluteJoint::RevoluteJoint( Robot* robot, KinTreeNode* parent, Motor* motor ) : Joint( robot, parent, motor, RJOINT )
{	
    limits = Interval();
	limits.setMin(-M_PI);
	limits.setMax(M_PI);
}
void RevoluteJoint::setM()
{
	M.setAxisAngleRotation(nodeAxis, position);
}
