/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RobotFilter.cpp Implementation file for the RobotFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "constants.h"
#include "reflexFilter.h"
#include <yarp/dev/all.h>
#include <QTime>

ReflexFilter::ReflexFilter( KinematicModel::Robot* r, bool visualize ) : VirtualSkin::RobotFilter( r, visualize) 
{
}

ReflexFilter::~ReflexFilter()
{
}

void ReflexFilter::extraOpenStuff()
{
	originalPose.resize(robot->numBodyParts());
	targetPose.resize(robot->numBodyParts());
	history.resize(robot->numBodyParts());
}

/*void ReflexFilter::stopRobot()
{
	yarp::os::Bottle stop;
	stop.addVocab(VOCAB_SET);
	stop.addVocab(VOCAB_STOPS);
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
	{
		cbFilters.at(bodyPart)->injectCall(stop);
	}
	printf("sent stop commands\n");
}*/

void ReflexFilter::collisionResponse()
{
	yarp::os::Bottle rewind;
	//QVector<qreal>::const_iterator jointA,jointB;
	//int jointNum;
	QVector<qreal>::const_iterator joint;
	QTime safeTime;
	qreal period;
	
	// this works great on the simulator, but not on the robot !?!
	/**/
	
	// get the pose history
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
	{
		history.replace(bodyPart, stateObservers.at(bodyPart)->getHistory() );
	}
	
	yarp::os::Bottle& prefix = rewind.addList();
	//prefix.addVocab(VOCAB_VELOCITY_MOVES);
	prefix.addVocab(VOCAB_POSITION_MOVES);
	yarp::os::Bottle& bodyPartPose = rewind.addList();
	
	for ( int n = 0; n < POSE_BUFFER_SIZE ; n++ )
	{
		if ( isColliding || safeTime.elapsed() < 500 )
		{
			// we want to control at 10 Hz, but the hardware streams poses at 100Hz
			// the simulator does it at 50Hz, so reflexes will be rather fast
			if ( n % 10 == 0 )
			{
				printf("------------------------------------------ %d\n",n%10);
				for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
				{
					bodyPartPose.clear();
					period = stateObservers.at(bodyPart)->getPeriod();
					printf("Body Part - %d, %f period: ", bodyPart, period );
					/*for ( jointA  = history.at(bodyPart).at(n).begin(), jointB  = history.at(bodyPart).at(n+1).begin();
						  jointA != history.at(bodyPart).at(n).end() && jointB != history.at(bodyPart).at(n+1).end() ;
						 ++jointA, ++jointB )
					{
						bodyPartPose.addDouble( 100*(*jointB-*jointA)/period );
					}*/
					for ( joint = history.at(bodyPart).at(n).begin(); joint != history.at(bodyPart).at(n).end(); ++joint )
					{
						bodyPartPose.addDouble( *joint );
					}
					
					printf("%s\n", rewind.toString().c_str());
					cbFilters.at(bodyPart)->injectCommand(rewind);
				}
				
				//printf("  waiting %f/%d msec\n", period, model.robot->numBodyParts());
				//period /= static_cast<qreal>(robot->numBodyParts());
			
				//printf("sleeping for: %f\n", 20);
				
				msleep( 100 );
				if ( isColliding )
				{
					safeTime.restart();
					printf("colliding\n");
				}
				else
				{
					printf("not colliding - %d\n", safeTime.elapsed());
				}
			}
		}
		else { break; }
	}
	
	//stopRobot();
	
	/*
	// wait for stupid response to complete or for 10 seconds, whichever comes first
	QTime timeout;
	timeout.start();
	while ( isColliding || safeTime.elapsed() < ALL_CLEAR_WAIT )
	{
		if ( isColliding ) { safeTime.restart(); }
		if ( timeout.elapsed() > 10000 ) { break; }  // 10 seconds
		msleep(20);
	}*/
}
