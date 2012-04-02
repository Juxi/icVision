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
	
	void setVelocity( int i );
	
	bool gotoNearest();
	bool goTo(Roadmap::vertex_t v);


	//Roadmap::edge_t randomMove();
	bool waitForMotion();	// wait until the motion is done or we are cut off from the robot
	bool isOnMap();
	double maxDiff(std::vector<double>,std::vector<double>);
	
	//void start();
	void restart();
	void stop();

protected:
	
	yarp::os::Network yarp;
	//yarp::os::Port vSkinStatus;
	yarp::os::RpcClient worldPort;
	
	iCubController* robot;
	Roadmap* roadmap;
	double	velocity;
public:
	Roadmap::vertex_t goal_vertex;
	
protected:
	volatile bool keepRunning;
	
	void run();
	void singleEdgeMove();
	void multipleEdgeMove( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path );
};
#endif
/** @} */
