/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RobotFilter.h Header file for the RobotFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef REFLEXFILTER_H_
#define REFLEXFILTER_H_

#include <time.h>

#include "robotFilter.h"

class ReflexFilter : public VirtualSkin::RobotFilter
{

public:
	
	ReflexFilter( bool visualize = true );
	//ReflexFilter( RobotModel::Robot* robot, RobotModel::World* world, bool visualize = true );
	virtual ~ReflexFilter();

	virtual void extraOpenStuff();
	virtual void collisionHandler();

private:
	
	QVector< QVector<qreal> > originalPose,targetPose;

};

#endif
