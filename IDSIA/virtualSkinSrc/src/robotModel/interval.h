#ifndef INTERVAL_H
#define INTERVAL_H

#include <QtGui>

/*! \brief A simple representation of a 1D interval (see Joint and Motor)
 *
 * The interval is inclusive of 'min' and 'max'.
 */

class Interval
{
private:
    qreal   min,
            max;
public:
    // constructor and destructor
    Interval();
    ~Interval();

    // set stuff
    void setMin( const qreal minimum );
    void setMax( const qreal maximum );

    // get stuff
    qreal getMin() const;
    qreal getMax() const;

    // other
    bool isInRange( const qreal num );
    bool isTooSmall( const qreal num );
    bool isTooBig( const qreal num );
};

#endif // INTERVAL_H
