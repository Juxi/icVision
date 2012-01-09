/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef INTERVAL_H
#define INTERVAL_H

#include <QtGui>

namespace KinematicModel { class Interval; }
	
/** \brief A simple representation of a 1D interval
 *
 * The interval is inclusive of its limits, 'min' and 'max'.
 */

class KinematicModel::Interval
{

public:
    Interval();								//!< Initializes 'min' and 'max' to 0
    ~Interval();							//!< Nothing to do here

    void setMin( const qreal minimum );		//!< Sets the value of 'min'. If min > max, min is set equal to max and a warning is printed.
    void setMax( const qreal maximum );		//!< Sets the value of 'max'. If max < min, max is set equal to max and a warning is printed.

    qreal getMin() const;	//!< Returns the value of 'min'.
    qreal getMax() const;	//!< Returns the value of 'max'.

    bool isInRange( const qreal num );		//!< Returns true iff min <= num <= max
    bool isTooSmall( const qreal num );		//!< Returns true iff num < min
    bool isTooBig( const qreal num );		//!< Returns true iff num > max
	
private:
    qreal   min,	//!< The lower limit of the Interval
			max;	//!< The upper limit of the Interval
};

#endif

/** @} */
