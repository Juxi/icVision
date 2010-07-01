#include "joint_revolute.h"
#include "displmatrix.h"

RevoluteJoint::RevoluteJoint( Robot* robot, Motor* motor ) : Joint( robot, motor, RJOINT )
{
    limits = Interval(-M_PI,M_PI);
}
RevoluteJoint::RevoluteJoint( KinTreeNode* parent, Motor* motor ) : Joint( parent, motor, RJOINT )
{
    limits = Interval(-M_PI,M_PI);
}
void RevoluteJoint::setM()
{
    M = DisplMatrix( nodeAxis, QVector3D(), position, 0 );
}
