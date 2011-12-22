#include "controlThread.h"

ControlThread::ControlThread( iCubController* _robot, Roadmap* _map ) : robot(_robot), map(_map), keepRunning(true)
{
}

ControlThread::~ControlThread()
{
}

void ControlThread::start()
{
	vSkinStatus.open("/statusOut");
	if ( !yarp.connect("/filterStatus","/statusOut") )
	{ 
		printf("failed to connect to robot filter status port\n");
		return;
	}
	
	if ( !gotoNearest() )
	{ 
		printf("Failed to move iCub onto the roadmap\n");
		return;
	}
		
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
	
	printf("iCub is on the roadmap\n");
	return true;
}

bool ControlThread::waitForMotion()
{
	printf("waiting for motion to finish\n");
	bool flag = false;
	while ( !flag ) { 
		if ( !robot->checkMotionDone(&flag) )
			return false;
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
	Roadmap::vertex_t	v = map->nearestVertex(p);
	Roadmap::CGAL_Point a( p.size(), p.begin(), p.end() );
	Roadmap::CGAL_Point b = map->getCgalPose( v );
	
	//std::cout << "a: " << a << std::endl;
	//std::cout << "b: " << b << std::endl;
	
	if ( (b-a).squared_length() < 5.0 )
	{
		map->setCurrentVertex( v );
		return robot->setWaypoint();
	}
	return 0;
}

void ControlThread::run()
{
	yarp::os::Bottle b;
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
		//else { printf("."); }
		msleep(100);
	}
}

void ControlThread::stop()
{
	keepRunning = false;
	while ( isRunning() ) { msleep(20); }
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