
#include "math.h"
#include <QTime>
#include "controlThread.h"
#include "compositeObject.h"

ControlThread::ControlThread( bool visualize ) : model(visualize)
{
}

void ControlThread::run()
{
	qreal	epochDuration = 4; //seconds
	qreal	msEpochDuration = epochDuration*1000;
	RobotModel::CompositeObject* box = NULL;
	QTime	qTime;
	srand( time(NULL) );
	
	while (true)
	{
		// initialize
		qTime.restart();
		model.world->clear();
		
		//make random boxes
		for ( int k = 0; k < 5; k++ )
		{
			box = model.world->newBox(   QVector3D(.1,.1,.1),
										 QVector3D(	-.3 + (double)rand()/RAND_MAX * .6,
													-.3 + (double)rand()/RAND_MAX * .6,
												    -.3 + (double)rand()/RAND_MAX * .6
												   )
									  );
			box->setCartesianOrientation(QVector3D(	(double)rand()/RAND_MAX * 6.282,
												    (double)rand()/RAND_MAX * 6.282,
												    (double)rand()/RAND_MAX * 6.282
												   )
										 );
		}/**/
		
		// set the robot to a random position
		for ( int i = 0; i < model.robot->numBodyParts(); i++ )
		{
			for ( int j = 0; j < model.robot->getNumMotors(i) && j < 7 ; j++ )
			{
				model.robot->setNormalPosition( i, j, (double)rand()/RAND_MAX );
			}
		}/**/
		
		//model.robot->setEncoderPosition(0);
		
		// repeatedly test for collision 
		while ( qTime.elapsed() < msEpochDuration )
		{
			model.computePose();
			msleep(20);
			printf("elapsed: %d, duration: %f \n", qTime.elapsed(), msEpochDuration);
		}
		
		//while ( true )
		//{
		//	for ( double i=0; i<6.282; i+=.01 )
		//	{
		//		model.robot->setNormalPosition( 0, 5, (sin(i)+1)/2 );  // sets all motor positions on the robot
		//		model.computePose();
		//		msleep(20);
		//	}
		//}
		
	}
}
