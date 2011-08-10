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

void ReflexFilter::collisionResponse()
{
	
	yarp::os::Bottle stop;
	
	QVector< QVector<qreal> >::iterator	pose;
	QVector<qreal>::const_iterator joint;
	QVector< QVector<qreal> > bpHistory;
	
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
	{
		// stop the body part
		stop.clear();
		stop.addVocab(VOCAB_SET);
		stop.addVocab(VOCAB_STOPS);
		cbFilters.at(bodyPart)->injectCall(stop);
		
		// display histories
		/*if ( bodyPart == 0 )
		{
			printf("  Body Part - %d:", bodyPart);
			bpHistory = stateObservers.at(bodyPart)->getHistory();
			printf("Body Part: %d\n", bodyPart);
			for ( pose = bpHistory.begin(); pose != bpHistory.end(); ++pose )
			{
				for ( joint = pose->begin(); joint != pose->end(); ++joint )
				{
						printf("%f ",*joint);
				}
				printf("\n");
			}
			printf("\n");
		}*/
		
		// this is part of the smart response below that doesn't work on the hardware :-(
		history.replace(bodyPart, stateObservers.at(bodyPart)->getHistory() );
	}
	
	// stupid response
	/*yarp::os::Bottle rewind;
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
	{
		rewind.clear();
		rewind.addVocab(VOCAB_SET);
		rewind.addVocab(VOCAB_POSITION_MOVES);
		yarp::os::Bottle& bodyPartPose = rewind.addList();
		const QVector<qreal>& pose = stateObservers.at(bodyPart)->earliestPose();
		for ( joint = pose.begin(); joint != pose.end(); ++joint )
		{
			bodyPartPose.addDouble(*joint);
		}
		// INJECT COMMAND!
		cbFilters.at(bodyPart)->injectCall(rewind);
		printf("%s\n", rewind.toString().c_str());
	}
	printf("\n");*/
}

void ReflexFilter::responseComplete()
{
	yarp::os::Bottle rewind;
	QVector<qreal>::const_iterator joint;
	QTime safeTime;
	qreal period;

	// this works great on the simulator, but not on the robot !?!
	for ( int n = 0; n < POSE_BUFFER_SIZE; n++ )
	{
		if ( isColliding || safeTime.elapsed() < ALL_CLEAR_WAIT)
		{
			//printf("Pose: %d\n",n);
			period = 0.0;
			for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
			{
				//if ( bodyPart == 0) printf("  Body Part - %d:", bodyPart);
				rewind.clear();
				rewind.addVocab(VOCAB_SET);
				rewind.addVocab(VOCAB_POSITION_MOVES);
				yarp::os::Bottle& bodyPartPose = rewind.addList();
				for ( joint = history.at(bodyPart).at(n).begin(); joint != history.at(bodyPart).at(n).end(); ++joint )
				{
					bodyPartPose.addDouble(*joint);
					//if ( bodyPart == 0) printf(" %f", *joint);
				}
				cbFilters.at(bodyPart)->injectCall(rewind);
				period += stateObservers.at(bodyPart)->getPeriod();
				//if ( bodyPart == 0) printf("\n");
			}
			//printf("  waiting %f/%d msec\n", period, model.robot->numBodyParts());
			period /= static_cast<qreal>(robot->numBodyParts());
			msleep(period);
			if ( isColliding ) { safeTime.restart(); }
		}
		else { break; }
	}
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
