#include "joint.h"
#include "motor.h"

Joint::Joint( Motor* motor ) : motor(motor), position(0)
{
    limits = Interval();
    M.setToIdentity();
    motor->append(this);
}
void Joint::setPos()
{
    position = limits.getMin() + motor->normPos()*( limits.getMax() - limits.getMin() );
    setM();
}
Joint::~Joint(){}
