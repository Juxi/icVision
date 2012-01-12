#include "controlThread.h"

ControlThread::ControlThread( iCubController* _robot, Roadmap* _map ) : robot(_robot), roadmap(_map), keepRunning(false)
{
	vSkinStatus.open("/statusOut");
	if ( !yarp.connect("/filterStatus","/statusOut") )
	{ 
		printf("failed to connect to robot filter status port\n");
		return;
	}
}

ControlThread::~ControlThread()
{
	vSkinStatus.close();
		int one = 1;
}

void ControlThread::restart()
{
	if (isRunning())
		stop();
	
	printf("starting iCub Roadmap Controller\n");
		
	keepRunning = true;
	QThread::start();
}

bool ControlThread::gotoNearest()
{
	std::vector<double> p = robot->getCurrentPose();
	if ( p.size() == 0 ) return false;
	
	Roadmap::vertex_t v = roadmap->nearestVertex(p);
	
	if ( !robot->positionMove(roadmap->getStdPose(v)) )
		return 0;
	
	// this is a hack...  need a better waitForMotion that does not rely on iPositionControl::checkMotionDone()
	msleep(200);
	
	if ( !waitForMotion() )
		return 0;
	
	if ( !isOnMap() )
	{
		printf("Failed to move iCub onto the roadmap\n");
		return false;
	}
	
	return true;
}

bool ControlThread::waitForMotion()
{
	printf("waiting for motion to finish\n");
	bool flag = false;
	while ( !flag ) { 
		if ( !robot->checkMotionDone(&flag) )
			return false;
		if ( !keepRunning )
		{
			printf("broke waitForMotion()\n");
			return false;
		}
		printf(".");
		msleep(20);
	}
	printf("\n");
	return true;
}

bool ControlThread::isOnMap()
{
	//printf("called isOnMap()\n");
	
	std::vector<double> p = robot->getCurrentPose();
	Roadmap::vertex_t	v = roadmap->nearestVertex(p);
	Roadmap::CGAL_Point a( p.size(), p.begin(), p.end() );
	Roadmap::CGAL_Point b = roadmap->getCgalPose( v );
	
	//std::cout << "a: " << a << std::endl;
	//std::cout << "b: " << b << std::endl;
	
	if ( (b-a).squared_length() < 5.0 )
	{
		printf("robot is on the roadmap\n");
		roadmap->setCurrentVertex( v );
		return robot->setWaypoint();
	}
	return 0;
}

void ControlThread::run()
{
	//yarp::os::Port vSkinStatus;
	yarp::os::Bottle b;
	
	printf("Moving iCub to the nearest state on the roadmap\n");
	if ( !gotoNearest() )
	{ 
		return;
	}
	
	while ( keepRunning )
	{
		vSkinStatus.read(b);
		if (  b.get(0).asInt() != 1 ) {	// try to control the robot only when the Virtual Skin Proxy is open
			printf("Reflex behavior is active. Waiting for control of the robot.\n");
		}
		else {
			if ( !isOnMap() ) { // if the filter is open, the robot should be on its way back onto the roadmap
				printf("The iCub is not on the roadmap. Waiting for reflex move to finish.\n");
			}
			else
			{
				//singleEdgeMove();
				multipleEdgeMove();
			}
		}
		msleep(100);
	}
	
	//vSkinStatus.interrupt();
	//yarp.disconnect("/filterStatus","/statusOut",false);
	printf("*** RUN METHOD RETURNED ***\n");
}

void ControlThread::singleEdgeMove()
{
	// select a random action given our current location in the map
	std::pair< Roadmap::edge_t, std::vector<double> > thisMove = roadmap->randomMove();
	
	// if we got a valid action, try it out
	if ( thisMove.second.size() > 0 )
	{
		printf("*** TAKING A RANDOM ACTION ***\n");
		roadmap->setEdgeColor( thisMove.first, Qt::yellow );	// mark the currently selected action on the
		robot->positionMove( thisMove.second );					// move the robot
		
		if ( waitForMotion() ) 
		{
			std::cout << "POSITION MOVE COMPLETE\n" << std::endl;
			roadmap->setEdgeColor( thisMove.first, Qt::green ); 
		}
		else 
		{
			std::cout << "POSITION MOVE INTERRUPTED\n" << std::endl;
			roadmap->setEdgeColor( thisMove.first, Qt::red ); 
			roadmap->removeEdge( thisMove.first );
		}
	}
}

void ControlThread::multipleEdgeMove()
{
	std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > >::iterator i;
	std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path = roadmap->randomMoves();
	
	for ( i = path.begin(); i != path.end(); ++i )
		roadmap->setEdgeColor( i->first, Qt::yellow );
	
	bool motionInterrupted = false;
	for ( i = path.begin(); i != path.end(); ++i )
	{
		if ( !motionInterrupted )
		{
			robot->positionMove(roadmap->map[i->second].q);
			motionInterrupted = !waitForMotion();
			//!move(i->second);
			
			if ( !motionInterrupted ) {
				std::cout << "POSITION MOVE COMPLETE\n" << std::endl;
				roadmap->setEdgeColor( i->first, Qt::green ); 
				roadmap->setCurrentVertex( i->second );
				robot->setWaypoint();
			} else {
				std::cout << "POSITION MOVE INTERRUPTED\n" << std::endl;
				roadmap->setEdgeColor( i->first, Qt::red );
				roadmap->removeEdge( i->first );
			}
		} else {
			roadmap->setEdgeColor( i->first, Qt::black );
		}
	}	
}

void ControlThread::stop()
{
	//robot->stop();
	keepRunning = false;
	printf("shutting down control thread\n");
	wait();
	/*while ( isRunning() )
	{ 
		printf("*");
		msleep(100);
	}*/
	printf("\n");
	printf("stopped the controller\n");
}

/*Roadmap::edge_t ControlThread::randomMove()
{	
	printf("Trying a random position move on the roadmap\n");
	
	//if ( !isOnMap() )
	//	return 0;
	
	std::pair< Roadmap::edge_t, std::vector<double> > thisMove;
	thisMove = map->randomMove();
	
	if ( thisMove.second.size() > 0 )
	{
		map->setColor( thisMove.first, Qt::red ); 

		robot->positionMove( thisMove.second );
		waitForMotion();

		map->setColor( thisMove.first, Qt::black ); 
	}
	
	return thisMove.first;
}*/