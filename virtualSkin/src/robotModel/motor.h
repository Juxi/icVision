#ifndef MOTOR_H
#define MOTOR_H

#include "joint.h"
#include "interval.h"

namespace RobotModel {

/*! \brief Affords control of one or more joints (see Branch and Joint)
 *
 * This class can control an arbitrary number of joints via a linear mapping between the motor position and each joint
 * position. The mapping is implicitly defined by the intervals motor.limits and joint.limits, and it works as follows:
 * The motor position is set and stored in motor.encoderPosition. Using the interval motor.limits, the encoder position
 * is normalized to a value on the interval [0,1] and this value is stored in motor.normalPosition. Finally, the normalized
 * motor position is used to interpolate the limits of each joint to which the motor is bound. The motor object inherits
 * QVector<Joint*>, and in this way it is its own list of joints. When a motor position is set, it immediately sets all
 * relevent joint positions.
 */

class Motor : public QVector<Joint*>
{
public:
	Motor( Robot* robot, Motor* motor = 0 );
    ~Motor();

    // get stuff
    //int idx() const { return index; }
    const QString& name() const { return motorName; }
    Motor* parent() const { return parentMotor; }
    qreal minPos() const { return motorLimits.getMin(); }
    qreal maxPos() const { return motorLimits.getMax(); }
    qreal encPos() const { return encoderPosition; }
    qreal normPos() const { return normalPosition; }
    void print();

    // set stuff
    void setName( const QString& name ) { motorName = name; }
    void setMin( qreal );
    void setMax( qreal );
    void setEncPos( qreal );
	void setHomePos( qreal );
    void setNormPos( qreal );
	
	void home();

private:
    //int        index;
    Motor*		parentMotor;
    QString		motorName;
    Interval	motorLimits;          // encoder position limits
    qreal		encoderPosition, // encoder position is on the interval 'limits'
				homePosition,
				normalPosition;  // normalized position is >=0 and <=1
	
    //Oscillator oscillator;
    void setJointPositions();
};
	
}

#endif 
