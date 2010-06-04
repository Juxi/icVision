#include "joint_revolute.h"
#include "displmatrix.h"

RevoluteJoint::RevoluteJoint( Motor* motor ) : Joint( motor )
{
    limits = Interval(-M_PI,M_PI);
}
void RevoluteJoint::setM( )
{
    M = DisplMatrix( axis, QVector3D(0,0,0), position, 0 );
}
