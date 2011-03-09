#include "robot.h"
#include "robotmodelexception.h"

using namespace std;
using namespace RobotModel;

Motor::Motor( Robot* robot, Motor* motor ) : parentMotor(motor),
											 encoderPosition(0.0),
											 homePosition(0.0),
											 normalPosition(0.0)
{
	if ( !robot ) { throw RobotModelException("The Motor constructor requires a pointer to a valid Robot."); }
	motorLimits = Interval();
	motorLimits.setMin(-1.0);
	motorLimits.setMax(1.0);
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
	setJointPositions();
}
void Motor::home()
{
	setEncPos(homePosition);
	printf("Set Motor %s to position %f.\n",name().toStdString().c_str(),encoderPosition);
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
			 printf("%i ", (*i)->idx());
         }
	printf("\n");
    printf("    ======================================\n");
}
