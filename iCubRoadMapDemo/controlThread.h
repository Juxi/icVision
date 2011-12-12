/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RoadmapDemo
 *	@{
 */

#ifndef CTRLTHREAD_H_
#define CTRLTHREAD_H_

#include "iCubController.h"
#include "roadmap.h"

#include <QThread>

class ControlThread : public QThread
{
	Q_OBJECT
	
public:
	
	ControlThread( iCubController* _robot, Roadmap* _map );
	~ControlThread();
	
	bool gotoNearest();
	bool isOnMap();
	bool motionCompleted();	// wait until the motion is done or we are cut off from the robot
	bool randomMove();

protected:
	
	iCubController* robot;
	Roadmap* map;
	
	void run();

};
#endif
/** @} */
