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
		// build a position move bottle
		rewind.clear();
		rewind.addVocab(VOCAB_SET);
		rewind.addVocab(VOCAB_POSITION_MOVES);
		yarp::os::Bottle& newPose = rewind.addList();
		
		// get the current (colliding) pose from the pose buffer
		originalPose.replace(bodyPart, stateObservers.at(bodyPart)->currentPose());
		
		// get the desired pose from the pose buffer
		targetPose.replace(bodyPart, stateObservers.at(bodyPart)->nonCollidingPose());
		
		//TODO: set ref speeds
		// set velocities and do the position move
		//partControllers.at(bodyPart)->setRefSpeeds(REFLEX_SPEED);
		
		// put the desired pose into the position move bottle
		for ( joint = targetPose.at(bodyPart).begin(); joint != targetPose.at(bodyPart).end(); ++joint ) {
			newPose.addDouble(*joint);
		}
		
		// send the position move command
		cbFilters.at(bodyPart)->injectCall(rewind);
	}
		
	// wait for the robot to reach the target pose
	bool poseReached = false;
	QVector<qreal> dstToTarget;
	QVector<qreal>::const_iterator original,current,target;
	//QVector<qreal>& currentPose;
	
	//TODO: Use Qtime to implement a timeout
	time_t startTime = time(NULL);
	dstToTarget.resize(model.robot.numBodyParts());
	
	while ( !poseReached )
	{
		// compute the current distance to target pose (1-norm of the joint space displacement vector)
		for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
		{
			dstToTarget[bodyPart] = 0;
			const QVector<qreal>& currentPose = stateObservers.at(bodyPart)->currentPose();
			for ( original  = originalPose.at(bodyPart).begin(), current  = currentPose.begin(), target  = targetPose.at(bodyPart).begin();
				 original != originalPose.at(bodyPart).end() && current != currentPose.end() && target != targetPose.at(bodyPart).end();
				 ++original, ++current, ++target )
			{
				//perCentToPose += (*target-*original)/qAbs(*target-*original) * (*target-*current);
				dstToTarget[bodyPart] += qAbs(*target-*current);
			}
		}
		
		poseReached = true;
		for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
		{
			if ( dstToTarget.at(bodyPart) > NEGLIGIBLE_ANGLE ) { poseReached = false; }
		}
		
		if ( time(NULL) - startTime >= POSITION_MOVE_TIMEOUT )
		{
			printf("Position move for collision recovery timed out!\n");
			break;
		}
	}
}
