#include "joint_prismatic.h"
#include "displmatrix.h"

using namespace RobotModel;

PrismaticJoint::PrismaticJoint( Robot* robot, KinTreeNode* parent, Motor* motor ) : Joint( robot, parent, motor, PJOINT )
{
    limits = Interval();
	limits.setMin(-M_PI);
	limits.setMax(M_PI);
}
void PrismaticJoint::setM( )
{
    //M = DisplMatrix( QVector3D(), nodeAxis, 0, position );
	M.setTranslation( nodeAxis + position*nodeAxis.normalized() );
}
