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
	originalPose.resize(model.robot->numBodyParts());
	targetPose.resize(model.robot->numBodyParts());
}

void ReflexFilter::collisionResponse()
{
	QVector< QVector<qreal> >	currentPose;
	yarp::os::Bottle rewind;
	QVector<qreal>::const_iterator joint;
	
	// move the robot back to a safe configuration
	for ( int bodyPart = 0; bodyPart < model.robot->numBodyParts(); bodyPart++)
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
	time_t	startTime = time(NULL);
	while ( isColliding )	// wait until the model is no longer colliding with itself
	{
		if ( time(NULL) - startTime > POSITION_MOVE_TIMEOUT ) { break; }
		//printf("Waiting for safe position...\n");
		msleep(YARP_PERIOD_ms);
	}
	
	//printf("Found a safe position");
	
	startTime = time(NULL);
	while ( time(NULL) - startTime < ALL_CLEAR_WAIT )
	{
		printf(".");
		msleep(YARP_PERIOD_ms);
	}
	//printf("\n");
}
