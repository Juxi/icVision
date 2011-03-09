/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StateObserver.cpp Implementation file for the StateObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "StateObserver.h"
#include "RobotFilter.h"
#include "collisionDetector.h"
#include "constants.h"
#include <QtGlobal>
#include <QVector>

using namespace VirtualSkin;

StateObserver::StateObserver(RobotFilter *f, const int b) : IObserver(), robotFilter(f), bodyPart(b)
{
	poseBuffer.setBufferSize(POSE_BUFFER_SIZE);
}

StateObserver::~StateObserver()
{
	// Nothing to destruct here
}

void StateObserver::onDataObserved(yarp::os::Bottle &b)
{
	QVector<qreal> poss;
	for (int i = 0; i < b.size(); i++) {
		poss.append((qreal) (((b.get(i).asDouble()))));
	}
	
	// set the current position
	//robotFilter->mutex.lock();
		 poseBuffer.put(poss);
	
	
		 //robotFilter->getRobot()->setEncoderPosition(bodyPart, poss);
		 //bool feasible = robotFilter->computePose();
		bool feasible = robotFilter->setPosition(bodyPart, poss);
	//robotFilter->mutex.unlock();
		 
	if ( !feasible )
	{
		robotFilter->takeControl();
	}
	
	
}
