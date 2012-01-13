#include "controlThread.h"
#include <QTime>

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
	msleep(500);
	
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
	QTime timer;
	timer.start();
	
	bool flag = false;
	while ( !flag ) { 
		if ( !robot->checkMotionDone(&flag) ||!keepRunning )
			return false;
		else if ( timer.elapsed() > 10000)
		{
			printf("waitForMotion() timed out\n");
			return false;
		}
		printf(".");
		msleep(100);
	}
	printf("\n");
	return true;
}

bool ControlThread::isOnMap()
{
	//printf("called isOnMap()\n");
	
	std::vector<double> a = robot->getCurrentPose();
	Roadmap::vertex_t	v = roadmap->nearestVertex(a);
	std::vector<double> b = roadmap->map[v].q;
	
	double err = maxDiff(a,b);
	
	if ( err > 5.0 )
	{
		printf("robot is not on the roadmap. maxErr: %f\n", err);
		return false;
	}
	
	//Roadmap::CGAL_Point a( p.size(), p.begin(), p.end() );
	//Roadmap::CGAL_Point b = roadmap->getCgalPose( v );

	//Roadmap::CGAL_Point::Cartesian_const_iterator i,j;
	//for ( i=a.cartesian_begin(), j=b.cartesian_begin();
	//	  i!=a.cartesian_end() && j!=b.cartesian_end();
	//	  ++i, ++j )
	//{
	//	if ( *i-*j > 5 )
	//		return false;
	//}
	
	printf("robot is on the roadmap. maxErr: %f\n", err);
	roadmap->setCurrentVertex( v );
	if ( !robot->setWaypoint() )
		printf("failed to set waypoint in the VSkin history\n");
	
	return true;
}

double ControlThread::maxDiff(std::vector<double> a,std::vector<double> b)
{
	double result = 0;
	std::vector<double>::iterator i,j;
	for ( i=a.begin(), j=b.begin();
		  i!=a.end() && j!=b.end();
		  ++i, ++j )
	{
		if ( *i-*j > result )
			result = *i-*j;
	}
	return result;
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
				Roadmap::out_edge_i e, e_end;
				tie(e, e_end) = out_edges( roadmap->currentVertex, roadmap->map );
				if ( e == e_end ) {
					printf("There are no out edges from the current vertex. Please resolve the situation manually.\n");
					keepRunning = false;
				} else {
					//singleEdgeMove();
					multipleEdgeMove();
				}
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
			Roadmap::out_edge_i e, e_end;
			tie(e, e_end) = out_edges( i->second, roadmap->map );
			
			//if ( /*e == e_end || */
			//	 !robot->isWithinLimits( roadmap->map[i->second].q ) ) {
			//	 motionInterrupted = true;
			//} else {
				robot->positionMove(roadmap->map[i->second].q);
				motionInterrupted = !waitForMotion();
			//}

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