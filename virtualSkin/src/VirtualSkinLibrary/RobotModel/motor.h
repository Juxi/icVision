/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "joint.h"
#include "interval.h"

namespace RobotModel { class Motor; }

/*! \brief Affords control of one or more joints (see Joint)
 *
 * This class can control an arbitrary number of joints via a linear mapping between the motor position and each joint
 * position. The mapping is implicitly defined by the intervals motor.limits and joint.limits, and it works as follows:
 * The motor position is set and stored in motor.encoderPosition. Using the interval motor.limits, the encoder position
 * is normalized to a value on the interval [0,1] and this value is stored in motor.normalPosition. Finally, the normalized
 * motor position is used to interpolate the limits of each joint to which the motor is bound. The motor object inherits
 * QVector<Joint*>, and in this way it is its own list of joints. When a motor position is set, it immediately sets all
 * relevent joint positions.
 */

class RobotModel::Motor : public QVector<Joint*>
{
	
public:
	
	Motor( Robot* robot, Motor* motor = 0 );
	~Motor();

	const QString& name() const { return motorName; }
	Motor* parent() const { return parentMotor; }
	qreal minPos() const { return motorLimits.getMin(); }
	qreal maxPos() const { return motorLimits.getMax(); }
	qreal encPos() const { return encoderPosition; }
	qreal normPos() const { return normalPosition; }
	void print();

	void setName( const QString& name ) { motorName = name; }
	void setMin( qreal );
	void setMax( qreal );
	void setEncPos( qreal );
	void setHomePos( qreal );
	void setNormPos( qreal );
	
	void home();

private:
	
	Motor*		parentMotor;
	QString		motorName;
	Interval	motorLimits;          // encoder position limits
	qreal		encoderPosition, // encoder position is on the interval 'limits'
				homePosition,
				normalPosition;  // normalized position is >=0 and <=1
	
	void setJointPositions();
};

#endif 

/** @} */
