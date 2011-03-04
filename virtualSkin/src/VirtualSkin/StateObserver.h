/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StateObserver.h Header file for the StateObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef STATEOBSERVER_H_
#define STATEOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "circularBuffer.h"
#include "robot.h"

namespace VirtualSkin {
	
class RobotFilter;

class StateObserver: public yarp::os::IObserver
{

public:
	
	StateObserver(RobotFilter *f, const int b);
	virtual ~StateObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);
	
	QVector<qreal> nonCollidingPose() { return poseBuffer.getOldest(); }
	QVector<qreal> currentPose() { return poseBuffer.getCurrent(); }
	void initPoseBuffer( const QVector<qreal>& v ) { poseBuffer.init(v); }

private:
	
	RobotFilter *robotFilter;
	const int bodyPart;
	CircularBuffer poseBuffer;
};
}
#endif
