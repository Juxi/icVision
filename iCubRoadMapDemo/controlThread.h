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
	//Roadmap::edge_t randomMove();
	bool waitForMotion();	// wait until the motion is done or we are cut off from the robot
	
	void start();
	//void restart();
	void stop();

protected:
	
	yarp::os::Network yarp;
	yarp::os::Port vSkinStatus;
	
	iCubController* robot;
	Roadmap* map;
	bool keepRunning;
	
	void run();

};
#endif
/** @} */
