#include <iostream>
#include "interval.h"

using namespace std;
using namespace RobotModel;

Interval::Interval() : min(0), max(0) {}
Interval::~Interval() {}
void Interval::setMin( const qreal minimum )
{
	min = minimum;
    if ( minimum > max )
	{
        max = min;
        cout << "WARNING: Minimum joint value was set larger than maximum. Using minJointVal = maxJointVal = " << min << endl;
    }
}
void Interval::setMax( const qreal maximum )
{
	max = maximum;
    if ( maximum < min )
	{
        min = max;
        cout << "WARNING: Maximum joint value was set less than minimum. Using maxJointVal = minJointVal = " << max << endl;
    }
}
qreal Interval::getMin() const { return min; }
qreal Interval::getMax() const { return max; }
bool Interval::isTooSmall(qreal num )
{
    if ( num < min ) { return true; }
    else { return false; }
}
bool Interval::isTooBig(qreal num )
{
    if ( num > max ) { return true; }
    else { return false; }
}
bool Interval::isInRange(qreal num )
{
    if ( !isTooSmall(num) && !isTooBig(num) ) { return true; }
    else { return false; }
}
