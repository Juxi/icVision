#include <iostream>
#include "interval.h"

using namespace std;

Interval::Interval() : min(0), max(0) {}
Interval::Interval( const qreal min, const qreal max ) : min(min), max(max) {}
Interval::~Interval() {}
void Interval::setMin( const qreal minimum )
{
    if ( minimum <= 0 ) { min = minimum; }
    else {
        min = 0;
        cout << "WARNING: Minimum joint value must be <= 0. Using minimum = 0." << endl;
    }
}
void Interval::setMax( const qreal maximum )
{
    if ( maximum >= 0 ) { max = maximum; }
    else {
        max = 0;
        cout << "WARNING: Maximum joint value must be <= " << max << ". Using minimum = " << max << "." << endl;
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
