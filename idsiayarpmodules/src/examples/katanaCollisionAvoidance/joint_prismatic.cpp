#include "joint_prismatic.h"
#include "displmatrix.h"

PrismaticJoint::PrismaticJoint( Motor* motor ) : Joint( motor )
{
    limits = Interval(-1,1);
}
void PrismaticJoint::setM( )
{
    M = DisplMatrix( QVector3D(0,0,0), axis, 0, position );
}
