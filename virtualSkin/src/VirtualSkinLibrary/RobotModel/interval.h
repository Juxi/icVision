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

namespace RobotModel { class Interval; }
	
/** \brief A simple representation of a 1D interval
 *
 * The interval is inclusive of its limits, 'min' and 'max'.
 */

class RobotModel::Interval
{

public:
    Interval();
    ~Interval();

    void setMin( const qreal minimum );
    void setMax( const qreal maximum );

    qreal getMin() const;
    qreal getMax() const;

    bool isInRange( const qreal num );		//!< Returns true iff min <= num <= max
    bool isTooSmall( const qreal num );		//!< Returns true iff num < min
    bool isTooBig( const qreal num );		//!< Returns true iff num > max
	
private:
    qreal   min,	//!< The lower limit of the Interval
			max;	//!< The upper limit of the Interval
};

#endif

/** @} */
