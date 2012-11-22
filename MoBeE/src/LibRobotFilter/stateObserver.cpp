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

#include "stateObserver.h"
#include "robotFilter.h"
//#include "collisionDetector.h"
#include "constants.h"
#include <QtGlobal>
#include <QVector>

using namespace MoBeE;

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
	CircularBuffer::Item item;
	QVector<qreal> poss;
	for (int i = 0; i < b.size(); i++)
	{
		poss.append((qreal) (((b.get(i).asDouble()))));
	}
	item.value = poss;
	
	if ( !robotFilter->cbFilters.at(bodyPart)->isCut() ) { item.label = SAFE; }
	else { item.label = COLLIDING; }
	
	poseBuffer.put(item);
	
	int i = bodyPart;
	const QVector<qreal>& v = poss;
	emit setPosition(i,v);
	
	//printf("Emitted set position\n");
}
