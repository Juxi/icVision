#include <iostream>
#include "interval.h"

using namespace std;

Interval::Interval() : min(0), max(0) {}
Interval::Interval( const qreal min, const qreal max ) : min(min), max(max) {}
Interval::~Interval() {}
void Interval::setMin( const qreal minimum )
{
	min = minimum;
    if ( min > max ) { 
		//max = min;
		cout << "WARNING: Set Minimum (" << minimum << ") > Maximum" << endl; //, using Min=Max" << endl;
	}
}
void Interval::setMax( const qreal maximum )
{
	max = maximum;
    if ( max < min ) { 
		//min = max;
		cout << "WARNING: Set Maximum (" << maximum << ") < Minimum" << endl; //, using Min=Max" << endl;
	}
}
qreal Interval::getMin() const { return min; }
qreal Interval::getMax() const { return max; }
bool Interval::isTooSmall(qreal num )
{
    if ( num < min && num < max ) { return true; }
    else { return false; }
}
bool Interval::isTooBig(qreal num )
{
    if ( num > max && num > min ) { return true; }
    else { return false; }
}
bool Interval::isInRange(qreal num )
{
    if ( !isTooSmall(num) && !isTooBig(num) ) { return true; }
    else { return false; }
}
