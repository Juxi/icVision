#include "controlThread.h"

ControlThread::ControlThread( iCubController* _robot, Roadmap* _map ) : robot(_robot), map(_map), keepRunning(false)
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
	Roadmap::vertex_t v = map->nearestVertex( robot->getCurrentPose() );
	
	if ( !robot->positionMove(map->getStdPose(v)) )
		return 0;
	
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
	printf("------------------------------------------------------------------------\n");
	
	std::vector<double> p = robot->getCurrentPose();
	Roadmap::vertex_t	v = map->nearestVertex(p);
	//Roadmap::CGAL_Point a( p.size(), p.begin(), p.end() );
	Roadmap::CGAL_Point q = map->getCgalPose( v );
	
	//std::cout << "a: " << a << std::endl;
	//std::cout << "b: " << b << std::endl;
	
	std::vector<double>::iterator i;
	Roadmap::CGAL_Point::Cartesian_const_iterator j;
	for ( i = p.begin(), j = q.cartesian_begin();
		  i != p.end() && j != q.cartesian_end();
		  ++i, ++j
		 )
	{
		printf("abs(%f - %f) = %f ",*i,*j,qAbs(*i-*j));
		if ( qAbs(*i-*j) > 5.0 )
			return 0;
	}
	
	map->setCurrentVertex( v );
	printf(" robot is on the map\n");
	return robot->setWaypoint();
}

void ControlThread::run()
{
	//yarp::os::Port vSkinStatus;
	yarp::os::Bottle b;
	
	printf("Moving iCub to the nearest state on the map\n");
	if ( !gotoNearest() )
	{ 
		return;
	}
	
	while ( keepRunning )
	{
		// try to control the robot only when the Virtual Skin Proxy is open
		vSkinStatus.read(b);
		if (  b.get(0).asInt() != 1 )
		{
			printf("Reflex behavior is active. Waiting for control of the robot.\n");
		}
		else
		{
			//printf("\n");
			
			// if the filter is open, the robot should be on the roadmap
			if ( !isOnMap() )
			{
				printf("The iCub is not on the roadmap. Waiting for reflex move to finish.\n");
				//break;
			}
			else
			{
				// select a random action given our current location in the map
				std::pair< Roadmap::edge_t, std::vector<double> > thisMove = map->randomMove();
				
				// if we got a valid action, try it out
				if ( thisMove.second.size() > 0 )
				{
					printf("*** TAKING A RANDOM ACTION ***\n");
					map->setEdgeColor( thisMove.first, Qt::yellow );	// mark the currently selected action on the
					robot->positionMove( thisMove.second );		// move the robot
					
					if ( waitForMotion() ) 
					{
						std::cout << "POSITION MOVE COMPLETE\n" << std::endl;
						map->setEdgeColor( thisMove.first, Qt::black ); 
					}
					else 
					{
						std::cout << "POSITION MOVE INTERRUPTED\n" << std::endl;
						map->setEdgeColor( thisMove.first, Qt::red ); 
						map->removeEdge( thisMove.first );
					}
				}
			}
		}
		//printf("@");
		msleep(100);
	}
	//vSkinStatus.interrupt();
	
	//yarp.disconnect("/filterStatus","/statusOut",false);
	printf("*** RUN METHOD RETURNED ***\n");
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