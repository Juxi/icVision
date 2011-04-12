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

ReflexFilter::ReflexFilter( bool visualize ) : VirtualSkin::RobotFilter(visualize) 
{
}

ReflexFilter::~ReflexFilter()
{
}

void ReflexFilter::extraOpenStuff()
{
	originalPose.resize(model.robot.numBodyParts());
	targetPose.resize(model.robot.numBodyParts());
}

void ReflexFilter::collisionResponse()
{
	yarp::os::Bottle rewind;
	QVector<qreal>::const_iterator joint;
	
	// stop the whole robot
	for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
	{
		cbFilters.at(bodyPart)->injectCall(stop_command);
	}
	
	// move the robot back to a safe configuration
	for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
	{
		// get the current (colliding) pose from the pose buffer
		originalPose.replace(bodyPart, stateObservers.at(bodyPart)->currentPose());
		
		// get the desired pose from the pose buffer
		targetPose.replace(bodyPart, stateObservers.at(bodyPart)->nonCollidingPose());
		
		//TODO: if you want to set ref speeds, do it here...
		
		// build a position move bottle
		rewind.clear();
		rewind.addVocab(VOCAB_SET);
		rewind.addVocab(VOCAB_POSITION_MOVES);
		yarp::os::Bottle& newPose = rewind.addList();
		
		// put the desired pose into the position move bottle
		for ( joint = targetPose.at(bodyPart).begin(); joint != targetPose.at(bodyPart).end(); ++joint ) {
			newPose.addDouble(*joint);
		}
		
		if ( newPose.size() > 0 )
		{
			printf("%s\n",rewind.toString().c_str());	// print the position move command
			cbFilters.at(bodyPart)->injectCall(rewind);	// send the position move command
		}
	}
}

void ReflexFilter::responseComplete()
{
	while ( isColliding )
	{
		printf("Waiting for safe position...\n");
		msleep(YARP_PERIOD_ms);
	}
	
	printf("Found a safe position");
			
	time_t	startTime = time(NULL);
	while ( time(NULL) - startTime < 1 )
	{
		printf(".");
		msleep(YARP_PERIOD_ms);
	}
	printf("\n");
	
	// TODO: Use Qtime to implement the timeout?
	/*time_t							startTime = time(NULL);
	bool							poseReached = false;
	QVector<qreal>::const_iterator	original,
									current,
									target;
	qreal							dstToTarget;
	
	while ( !poseReached )
	{
		
		if ( !targetPose.isEmpty() && ! originalPose.isEmpty() )
		{
			poseReached = true;
			printf("Distance to Target: ");
			
			// compute the current distance to target pose
			for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
			{
				const QVector<qreal>& currentPose = stateObservers.at(bodyPart)->currentPose();
				
				for ( original  = originalPose.at(bodyPart).begin(), current  = currentPose.begin(), target  = targetPose.at(bodyPart).begin();
					  original != originalPose.at(bodyPart).end() && current != currentPose.end() && target != targetPose.at(bodyPart).end();
					  ++original, ++current, ++target )
				{
					dstToTarget = qAbs((*target-*current)/(*target-*original));
					if ( dstToTarget > NEGLIGIBLE_FRACTION && !( *original<*target<*current || *original>*target>*current) )
					{
						poseReached = false;
					}
					printf("%f ", dstToTarget );
				}
			}
			printf("\n");
		}
		else
		{
			printf("Target pose or original pose unknown... corrupted pose buffer!?!\n");
		}
		
		if ( time(NULL) - startTime >= POSITION_MOVE_TIMEOUT )
		{
			printf("Position move for collision recovery timed out!\n");
			break;
		}
	}*/
}
