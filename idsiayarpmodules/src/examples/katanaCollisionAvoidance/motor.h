#ifndef MOTOR_H
#define MOTOR_H

#include "joint.h"
#include "interval.h"

/*! \brief Affords control of one or more joints (see Branch and Joint)
 *
 * This class can control an arbitrary number of joints via a linear mapping between the motor position and each joint
 * position. The mapping is implicitly defined by the intervals motor.limits and joint.limits, and it works as follows:
 * The motor position is set and stored in motor.encoderPosition. Using the interval motor.limits, the encoder position
 * is normalized to a value on the interval [0,1] and this value is stored in motor.normalPosition. Finally, the normalized
 * motor position is used to interpolate the limits of each joint to which the motor is bound. The motor object inherits
 * QVector<Joint*>, and in this way it is its own list of joints. When a motor position is set, it immediately sets all
 * relevent joint positions.
 *
 * NOTE: Since it inherits both TreeNode<Motor> and QVector<Joint*>, this class has two different iterators. To iterate
 * through the joint pointers in the QVector<Joint*>, use 'QVector<Joint*>::iterator' with the member functions begin()
 * and end(). To iterate through the child motors in the tree, use 'TreeNode<Motor>::iterator' with the member functions
 * childrenBegin() and childrenEnd();
 */

class Motor : public QVector<Joint*>
{
public:
    Motor( Robot* robot, Motor* motor = 0 );
    ~Motor();

    // get stuff
    int idx() const { return index; }
    const QString& name() const { return motorName; }
    Motor* parent() const { return parentMotor; }
    qreal minPos() const { return motorLimits.getMin(); }
    qreal maxPos() const { return motorLimits.getMax(); }
    qreal encPos() const { return encoderPosition; }
    qreal normPos() const { return normalPosition; }
    void print();

    // set stuff
    void setName( const QString& name ) { motorName = name; }
    void setMin( qreal min ) { motorLimits.setMin(min); }
    void setMax( qreal max ) { motorLimits.setMax(max); }
    void setEncPos( qreal );
    //void setNormPos( qreal );


private:
    int        index;
    Motor*     parentMotor;
    QString    motorName;
    Interval   motorLimits;          // encoder position limits
    qreal      encoderPosition, // encoder position is on the interval 'limits'
               normalPosition;  // normalized position is >=0 and <=1

    void setJointPositions();
};

#endif // MOTOR_H
