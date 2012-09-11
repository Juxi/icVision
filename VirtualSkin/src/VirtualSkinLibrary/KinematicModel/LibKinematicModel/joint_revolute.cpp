#include "joint_revolute.h"
//#include "displmatrix.h"

using namespace KinematicModel;

RevoluteJoint::RevoluteJoint( /*DT_ResponseClass nodeResponseClass,*/ Robot* robot, int part, KinTreeNode* parent, Motor* motor ) : Joint( /*nodeResponseClass,*/ robot, part, parent, motor, RJOINT )
{	
    limits = Interval();
	limits.setMin(-M_PI);
	limits.setMax(M_PI);
}
void RevoluteJoint::setM()
{
	M.setAxisAngleOrientation(nodeAxis, position);
}
