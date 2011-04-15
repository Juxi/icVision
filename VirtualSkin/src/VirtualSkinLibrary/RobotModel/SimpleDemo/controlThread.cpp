
#include "math.h"
#include "controlThread.h"

ControlThread::ControlThread() : model(true)
{
}

void ControlThread::run()
{
	qreal	minPos=-100,
			maxPos=100;
	
	while (true)
	{
		for ( double i=0; i<6.282; i+=.01 )
		{
			model.robot->setEncoderPosition( minPos + (sin(i)+1)/2 * (maxPos-minPos) );  // sets all motor positions on the robot
			model.computePose();
			msleep(20);
		}
	}
}
