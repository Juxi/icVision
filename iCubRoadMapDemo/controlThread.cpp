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

void ControlThread::run()
{
	yarp::os::Bottle b;
	while ( keepRunning )
	{
		vSkinStatus.read(b);
		printf("Filter status: %s\n",b.toString().c_str());
		if (  b.get(0).asInt() == 1 )
		{
			//if ( !isOnMap() ) 
			//{
			//	if ( !gotoNearest() )  { printf("Failed to move iCub onto the roadmap\n"); } 
			//}
			if ( randomMove() ) std::cout << "OK" << std::endl;			
			else std::cout << "OH NO!!!" << std::endl;
			
			// MODIFY GRAPH EDGES HERE
			
		}
		else { printf("waiting for filter to open\n"); }
		usleep(100000);
	}
}

void ControlThread::stop()
{
	keepRunning = false;
	while ( isRunning() ) { msleep(20); }
}

bool ControlThread::gotoNearest()
{
	printf("Trying a move to the nearest roadmap vertex\n");
	
	Roadmap::vertex_t v = map->nearestVertex( robot->getCurrentPose() );
	
	if ( !robot->positionMove( map->getStdPose(v) ) ) { return 0; }
	
	return isOnMap();
}

bool ControlThread::isOnMap()
{
	printf("called isOnMap()\n");
	
	if ( !motionCompleted() ) { return 0; }
	
	std::vector<double> p = robot->getCurrentPose();
	Roadmap::vertex_t v = map->nearestVertex(p);
	
	Roadmap::CGAL_Point a(  p.size(), p.begin(), p.end() );
	Roadmap::CGAL_Point b = map->getCgalPose( v );
	
	if ( (b-a).squared_length() < 5.0 )
	{
		map->setCurrentVertex(v);
		return robot->setWaypoint();
	}
	
	return 0;
}

bool ControlThread::motionCompleted()
{
	printf("called motionCompleted()\n");
	
	bool flag = false;
	do { 
		if ( !robot->checkMotionDone(&flag) ) { break; }
		msleep(20);
	}
	while ( !flag );
	
	return flag;
}



bool ControlThread::randomMove()
{	
	printf("Trying a random position move on the roadmap\n");
	
	if ( !isOnMap() ) { return 0; }
	
	std::vector<double> p = map->randomMove();
	
	printf("Position move:\n");
	for ( std::vector<double>::iterator i = p.begin(); i != p.end(); ++i )
	{
		printf("%f\n", *i);
	}

	if ( !robot->positionMove( p ) )
	{ 
		//widget.removeEdge( map[ moves.at(idx) ].qtGraphEdge );
		printf("failed\n"); 
		return 0;
	}
	else { printf("succeded"); }
	

	return isOnMap();
}