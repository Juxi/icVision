#include <iostream>
#include "robot.h"

using namespace std;

Motor::Motor( Robot* robot, Motor* motor ) : index(robot->motorList.size()),
                                             parentMotor(motor),
                                             encoderPosition(0.0),
                                             normalPosition(0.0)
{
    robot->motorList.append(this);
    motorLimits = Interval(-1.0,1.0);
}
Motor::~Motor() { cout << "delete( motor" << this->idx() << " )..." << endl; }

int Motor::idx() const { return index; }
const QString& Motor::name() const { return motorName; }
Motor* Motor::parent() const { return parentMotor; }
qreal Motor::minPos() const { return motorLimits.getMin(); }
qreal Motor::maxPos() const { return motorLimits.getMax(); }
qreal Motor::encPos() const { return encoderPosition; }
qreal Motor::normPos() const { return normalPosition; }
void Motor::setEncPos( qreal thisEncoderPosition )
{
    //cout << "Called motor->setPos()" << endl;
    // if the motor position has changed, or if it is zero, do the position update calculations
    if ( !qFuzzyCompare(encoderPosition,thisEncoderPosition) || thisEncoderPosition == 0 ) {
        if ( motorLimits.isTooSmall( thisEncoderPosition ) ) {
            thisEncoderPosition = motorLimits.getMin();
            cout << "WARNING Motor " << idx() << ": " << motorName.toStdString()
                 << ". Encoder position must be >= " << motorLimits.getMin()
                 << ". Using encoderPosition = " << motorLimits.getMin() << "." << endl;
        } else if ( motorLimits.isTooBig( thisEncoderPosition ) ) {
            thisEncoderPosition = motorLimits.getMax();
            cout << "WARNING Motor " << idx() << ": " << motorName.toStdString()
                 << ". Encoder position must be <= " << motorLimits.getMax()
                 << ". Using encoderPosition = " << motorLimits.getMax() << "." << endl;
        }
        encoderPosition = thisEncoderPosition;
        normalPosition = (encoderPosition - minPos()) / (maxPos() - minPos());
        setJointPositions();
    }
}
void Motor::setJointPositions()
{
    QVector<Joint*>::iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->setPos();
        //(*i)->print();
    }
}
void Motor::print()
{
    cout << "      limits: [" << motorLimits.getMin() << ", " << motorLimits.getMax() << "]" << endl
         << "      encoderPosition: " << encoderPosition << endl
         << "      normalPosition:  " << normalPosition << endl
         << "      Joint List: ";
         QVector<Joint*>::iterator i;
         for ( i=begin(); i!=end(); ++i ) {
             //cout << (*i)->idx() << " ";
         }
    cout << endl;
    cout << "    ======================================" << endl;
         for ( i=begin(); i!=end(); ++i ) {
             //(*i)->print();
         }

}
