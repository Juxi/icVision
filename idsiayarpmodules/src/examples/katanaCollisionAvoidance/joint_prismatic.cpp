#include "joint_prismatic.h"
#include "displmatrix.h"

PrismaticJoint::PrismaticJoint( Robot* robot, Motor* motor ) : Joint( robot, motor, PJOINT )
{
    limits = Interval(-M_PI,M_PI);
}
PrismaticJoint::PrismaticJoint( KinTreeNode* parent, Motor* motor ) : Joint( parent, motor, PJOINT )
{
    limits = Interval(-1,1);
}
void PrismaticJoint::setM( )
{
    M = DisplMatrix( QVector3D(), nodeAxis, 0, position );
}
