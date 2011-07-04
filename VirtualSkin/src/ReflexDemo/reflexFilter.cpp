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
	history.resize(model.robot->numBodyParts());
}

void ReflexFilter::collisionResponse()
{
	//QVector< QVector<qreal> >	currentPose;
	yarp::os::Bottle stop;
	//QVector<qreal>::const_iterator joint;
	for ( int bodyPart = 0; bodyPart < model.robot->numBodyParts(); bodyPart++)
	{
		// stop the body part
		stop.clear();
		stop.addVocab(VOCAB_SET);
		stop.addVocab(VOCAB_STOPS);
		cbFilters.at(bodyPart)->injectCall(stop);
		
		// get the body part's history
		history.replace(bodyPart, stateObservers.at(bodyPart)->getHistory());
	}
}

void ReflexFilter::responseComplete()
{
	yarp::os::Bottle rewind;
	QVector<qreal>::const_iterator joint;
	qreal period;
	QTime safeTime;

	for ( int n = 0; n < POSE_BUFFER_SIZE; n++ )
	{
		if ( isColliding || safeTime.elapsed() < ALL_CLEAR_WAIT)
		{
			printf("Pose: %d\n",n);
			period = 0.0;
			for ( int bodyPart = 0; bodyPart < model.robot->numBodyParts(); bodyPart++ )
			{
				printf("  Body Part - %d:", bodyPart);
				rewind.clear();
				rewind.addVocab(VOCAB_SET);
				rewind.addVocab(VOCAB_POSITION_MOVES);
				yarp::os::Bottle& bodyPartPose = rewind.addList();
				for ( joint = history.at(bodyPart).at(n).begin(); joint != history.at(bodyPart).at(n).end(); ++joint )
				{
					bodyPartPose.addDouble(*joint);
					printf(" %f", *joint);
				}
				cbFilters.at(bodyPart)->injectCall(rewind);
				period += stateObservers.at(bodyPart)->getPeriod();
				printf("\n");
			}
			//printf("  waiting %f/%d msec\n", period, model.robot->numBodyParts());
			period /= static_cast<qreal>(model.robot->numBodyParts());
			msleep(period);
			if ( isColliding ) { safeTime.restart(); }
		}
		else { break; }
	}
}
