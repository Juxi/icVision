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
	//Roadmap::edge_t randomMove();
	bool waitForMotion();	// wait until the motion is done or we are cut off from the robot
	bool isOnMap();
	//double maxDiff(std::vector<double>,std::vector<double>);
	
	//void start();
	void restart();
	void stop();
	
	enum BehaviorType
	{
		SingleEdgeExplore,
		MultiEdgeExplore,
		GoToObject,
		velocityMove
	};
	
	void setBehavior( BehaviorType b ) { currentBehavior = b; }
	void setSalientObject( QString s ) { salientObject = s; }

protected:
	
	yarp::os::Network yarp;
	//yarp::os::Port vSkinStatus;
	yarp::os::RpcClient worldPort;
	
	iCubController* robot;
	Roadmap* roadmap;
	double	velocity;
	
	volatile bool keepRunning;
	
	BehaviorType currentBehavior;
	QString salientObject;
	
	void run();
	bool singleEdgeMove();
	bool positionMoveImpl( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path );
	bool velocityMoveImpl( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path, double c ); 
	
	std::pair<Roadmap::CGAL_Vector,Roadmap::CGAL_Vector> funnelAccel( double axialCoeff, Roadmap::edge_t edge ); 
	Roadmap::CGAL_Point currentPose();
};
#endif
/** @} */
