#include "robot.h"
#include "modelexception.h"
#include <iostream>

using namespace std;
using namespace KinematicModel;

Motor::Motor( Robot* robot, Motor* motor ) : bodyPartRoot(false),
                                             torque(0.0),
                                             spring(0.0),
                                             parentMotor(motor),
											 homePosition(0.0)
{
	if ( !robot ) { throw KinematicModelException("The Motor constructor requires a pointer to a valid Robot."); }
	//motorLimits = Interval();
	motorLimits.setMin(-1.0);
	motorLimits.setMax(1.0);
	setEncPos(0.0);
	robot->appendMotor(this);
}
Motor::~Motor()
{
}
void Motor::setMin( qreal min )
{
	if ( min > encoderPosition ) { setEncPos(min); }
	if ( min > homePosition ) { setHomePos(min); }
	motorLimits.setMin(min);
}
void Motor::setMax( qreal max )
{ 
	if ( max < encoderPosition ) { setEncPos(max); }
	if ( max < homePosition ) { setHomePos(max); }
	motorLimits.setMax(max);
}
void Motor::setEncPos( qreal pos )
{
	if ( motorLimits.isTooSmall( pos ) )	{ pos = motorLimits.getMin(); }
	else if ( motorLimits.isTooBig( pos ) )	{ pos = motorLimits.getMax(); }

	encoderPosition = pos;
	normalPosition = (encoderPosition - minPos()) / (maxPos() - minPos());
	setJointPositions();
}
void Motor::setHomePos( qreal pos )
{
	if ( motorLimits.isTooSmall( pos ) )	{ homePosition = motorLimits.getMin(); }
	else if ( motorLimits.isTooBig( pos ) )	{ homePosition = motorLimits.getMax(); }
	else { homePosition = pos; }
}
void Motor::setNormPos( qreal pos )
{
    if ( pos < 0 )		{ pos = 0; }
	else if ( pos > 1 ) { pos = 1; }
		
	normalPosition = pos;
	encoderPosition = minPos() + normalPosition * (maxPos() - minPos()); /***********/
	setJointPositions();
}

qreal Motor::normToEnc(qreal pos) {
	if ( pos < 0 )		{ pos = 0; }
	else if ( pos > 1 ) { pos = 1; }
	qreal new_pos = minPos() + pos * (maxPos() - minPos()); /***********/
	return new_pos;
}

qreal Motor::encToNorm(qreal pos) {
	qreal new_pos = (pos - minPos()) / (maxPos() - minPos());
	if ( new_pos < 0 ) new_pos = 0;
	else if ( new_pos > 1 ) new_pos = 1;

	return new_pos;
}

void Motor::home(bool verbose)
{
	setEncPos(homePosition);
	//if (verbose) printf("Set Motor %s to position %f.\n",name().toStdString().c_str(),encoderPosition);
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
	printf("      limits: [%f,%f]\n", motorLimits.getMin(), motorLimits.getMax() );
	printf("      encoderPosition: %f\n", encoderPosition);
	printf("      normalPosition: %f\n", normalPosition);
	printf("      jointList: ");
         QVector<Joint*>::iterator i;
         for ( i=begin(); i!=end(); ++i )
		 {
			 //printf("%i ", (*i)->idx());
         }
	printf("\n");
    printf("    ======================================\n");
}
