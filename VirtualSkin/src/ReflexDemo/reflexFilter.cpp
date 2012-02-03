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
	//originalPose.resize(robot->numBodyParts());
	//targetPose.resize(robot->numBodyParts());
	//history.resize(robot->numBodyParts());
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
	int n = 20;			// we will use every nth pose in the buffer
						// n = 20 -->  5Hz

	// get the pose history and the average period between arriving state bottles
	QVector< QVector< VirtualSkin::CircularBuffer::Item > > history;
	qreal period = 0;
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
	{
		history.push_back( stateObservers.at(bodyPart)->getHistory() );
		period += stateObservers.at(bodyPart)->getPeriod();
	}
	period /= robot->numBodyParts();
	
	// prepare a bottle for the position move commands
	yarp::os::Bottle rewind;
	yarp::os::Bottle& prefix = rewind.addList();
	yarp::os::Bottle& bodyPartPose = rewind.addList();
	prefix.addVocab(VOCAB_POSITION_MOVES);
	
	bool wpReached = false;
	QVector<double>::const_iterator joint;
	for ( int i = 0; i < POSE_BUFFER_SIZE ; i++ )
	{
		if ( wpReached ) { break; }
		else {
			
			//printf("i = %d\n",i);
			if ( i % n == 0 )	// use every nth pose in the buffer
			{
				printf("------------------------------------------\n");
				for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
				{
					printf("Pose: %d, Part: %d, Label: %d\n", i, bodyPart, history.at(bodyPart).at(i).label );
					
					// send the next position move bottle
					bodyPartPose.clear();
					for ( joint = history.at(bodyPart).at(i).value.begin(); joint != history.at(bodyPart).at(i).value.end(); ++joint )
					{
						bodyPartPose.addDouble( *joint );
					}
					cbFilters.at(bodyPart)->injectCommand(rewind);
					//if ( bodyPart ==2 ) { printf("%s\n", rewind.toString().c_str()); }
					
					if ( /*!isColliding &&*/ history.at(bodyPart).at(i).label == VirtualSkin::StateObserver::WAYPOINT )
					{ 
						wpReached = true;
						printf("WAYPOINT REACHED!!!\n\n");
						//setWaypoint();
					}
					else { wpReached = false; }
				}
	
				printf("sleeping %f msec\n\n", n*period );
				msleep( n*period );
			}
		}
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

/*void ReflexFilter::setWaypoint()
{
	printf("setting a waypoint in the pose buffer\n");
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
	{
		cbFilters.at(bodyPart)->setWaypoint();
	}
}*/
