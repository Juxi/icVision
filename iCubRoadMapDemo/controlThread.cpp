#include "controlThread.h"
#include <QTime>
//#include <yarp/os/all.h>

ControlThread::ControlThread( iCubController* _robot, Roadmap* _map ) : robot(_robot),
																		roadmap(_map),
																		refVelocity(5),
																		refAcceleration(5),
																		keepRunning(false),
																		currentBehavior(MultiEdgeExplore),
																		salientObject("")
{
	MoBeEStatus.open("/statusIn");
	if ( !yarp.connect("/filterStatus","/statusIn") )
	{ 
		printf("failed to connect to robot filter status port\n");
		return;
	}
	printf("opening world port\n");
	worldPort.open("/worldClient");
	//printf("done");
}

ControlThread::~ControlThread()
{
	//vSkinStatus.close();
	//	int one = 1;
}

void ControlThread::restart()
{
	if (isRunning())
		stop();
	
	printf("starting iCub Roadmap Controller\n");
		
	keepRunning = true;
	QThread::start();
}

void ControlThread::setVelocity( int i )
{ 
	if ( i < 0 ) i = 0;
	else if ( i > 100 ) i = 100;
	refVelocity = (double)i;
}

bool ControlThread::gotoNearest()
{
	std::vector<double> p = robot->getCurrentPose();
	if ( p.size() == 0 ) return false;
	
	Roadmap::vertex_t v = roadmap->nearestVertex(p);
	
	robot->setRefVelocity( refVelocity );
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
		//printf(".");
		msleep(500);
		if ( !robot->checkMotionDone(&flag) || !keepRunning )
			return false;
		else if ( timer.elapsed() > 30000)
		{
			printf("waitForMotion() timed out\n");
			return false;
		}
		else { printf("%.2fs  ",(double)timer.elapsed()/1000.0); }
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
	
				//std::vector<double>::iterator k;
				//printf("current: ");
				//for ( k = a.begin(); k!=a.end(); ++k )
				//	printf("%f ",*k);
				//printf("\n");

				//printf("nearest: ");
				//for ( k = b.begin(); k!=b.end(); ++k )
				//	printf("%f ",*k);
				//printf("\n");

	double err = robot->maxDiff(a,b);
	
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

/*double ControlThread::maxDiff(std::vector<double> a,std::vector<double> b)
{
	double result = 0;
	std::vector<double>::iterator i,j;
	int count = 0;
	for ( i=a.begin(), j=b.begin();
		  i!=a.end() && j!=b.end();
		  ++i, ++j )
	{
		//if ( count < 10 || (count > 18 && count < 26) )
		if ( MainWindow::jMask.at(count) )
		{
			if ( qAbs(*i-*j) > result )
				result = qAbs(*i-*j);
		}
	count++;
	}
	return result;
}*/

void ControlThread::run()
{
	//yarp::os::Port vSkinStatus;
	//yarp::os::Bottle b;
	
	//printf("Moving iCub to the nearest state on the roadmap\n");
	//if ( !gotoNearest() )
	//{ 
	//	return;
	//}

	while ( keepRunning )
	{
		//if ( !isOnMap() )	
		//{
		//	gotoNearest();
		//}
		
		/*** EXPLORE THE GRAPH ONE EDGE AT A TIME ***/
		if ( currentBehavior == SingleEdgeExplore )
		{
			if ( !singleEdgePositionMove() )
			{
				printf("There are no out edges from the current vertex. Please resolve the situation manually.\n");
				break;
			}
		}
		
		/*** EXPLORE THE GRAPH VIA RANDOM SHORTEST-PATHS ***/
		//else 
		if ( currentBehavior == MultiEdgeExplore )
		{
			std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path;
			path = roadmap->aToB( roadmap->currentVertex, Roadmap::vertex_t(rand()%num_vertices(roadmap->map)) );
			
			//if ( !positionMoveImpl( path ) )
			if ( !velocityMoveImpl( path ) )
			//if ( !simpleVelocityMoveImpl( path ) )
			{
				printf("velocityMoveImpl() failed.\n");
				//break;
			}
		}
		
		/*** REACH FOR A PARTICULAR OBJECT ***/
		else if ( currentBehavior == GoToObject || currentBehavior == velocityMove )
		{
			if (worldPort.getOutputCount()==0)
			{
				printf("Trying to connect to %s\n", "/world");
				yarp.connect("/worldClient","/world");
			}
			else
			{
				// get the position of cup1
				yarp::os::Bottle cmd;
				cmd.addString("get");
				cmd.addString(salientObject.toStdString().c_str());
				
				printf("Sending message... %s\n", cmd.toString().c_str());
				yarp::os::Bottle response;
				worldPort.write(cmd,response);
				printf("Got response: %s\n", response.toString().c_str());
				printf("size: %d",response.size());
				
				if ( response.size() == 17 )
				{
					std::vector<double> objectPosition;
					objectPosition.push_back(response.get(13).asDouble());
					objectPosition.push_back(response.get(14).asDouble());
					objectPosition.push_back(response.get(15).asDouble());
					
					printf("Object Location: ");
					for (int i=0; i<3; i++)
					{
						printf("%f ", objectPosition[i] );
					}
					printf("\n");
					
					// find the node in the map that brings a hand closest to cup1
					Roadmap::vertex_t graspingVertex = roadmap->nearestWorkspaceVertex( objectPosition );
					if ( currentBehavior == GoToObject )
						positionMoveImpl( roadmap->aToB( roadmap->currentVertex, graspingVertex ) );
					else if ( currentBehavior == velocityMove )
						velocityMoveImpl( roadmap->aToB( roadmap->currentVertex, graspingVertex ) );
				}
				
			}
		}
		yarp::os::Time::delay(1);

        // Move to a random other vertex in the map
        // positionMoveImpl( roadmap->aToB( roadmap->currentVertex, Roadmap::vertex_t(rand()%num_vertices(roadmap->map) ) ) );
        
		// Move between vertices 0 and 1
		//else if ( roadmap->currentVertex == 0 )	
		//	positionMoveImpl( roadmap->aToB( roadmap->currentVertex, Roadmap::vertex_t(1) ) );
		//else
		//	positionMoveImpl( roadmap->aToB( roadmap->currentVertex, Roadmap::vertex_t(0) ) );
	}
	
	//Roadmap::out_edge_i e, e_end;
	//tie(e, e_end) = out_edges( roadmap->currentVertex, roadmap->map );
	//if ( e == e_end ) {
	//printf("There are no out edges from the current vertex. Please resolve the situation manually.\n");
	//keepRunning = false;
	
	//vSkinStatus.interrupt();
	//yarp.disconnect("/filterStatus","/statusOut",false);
	printf("*** RUN METHOD RETURNED ***\n");
}

bool ControlThread::singleEdgePositionMove()
{
	// select a random action given our current location in the map
	std::pair< Roadmap::edge_t, std::vector<double> > thisMove = roadmap->randomMove();
	if ( thisMove.second.size() == 0 )
		return false;
	
	// if we got a valid action, try it out
	if ( thisMove.second.size() > 0 )
	{
		printf("*** TRAVERSING A RANDOM EDGE ***\n");
		roadmap->setEdgeColor( thisMove.first, Qt::red );	// mark the currently selected action on the
		robot->setRefVelocity( refVelocity );
		robot->positionMove( thisMove.second );					// move the robot
		
		if ( waitForMotion() ) 
		{
			std::cout << "POSITION MOVE COMPLETE\n" << std::endl;
			roadmap->setEdgeColor( thisMove.first, Qt::black ); 
			roadmap->setCurrentVertex(target(thisMove.first,roadmap->map));
		}
		else 
		{
			QColor color = Qt::lightGray;
			std::cout << "POSITION MOVE INTERRUPTED\n" << std::endl;
			roadmap->setEdgeColor( thisMove.first, color.lighter() ); 
			roadmap->removeEdge( thisMove.first );
		}
	}
	return true;
}



bool ControlThread::positionMoveImpl( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path )
{
	std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > >::iterator i;
	
	for ( i = path.begin(); i != path.end(); ++i )
		roadmap->setEdgeColor( i->first, Qt::red );
	
	bool motionInterrupted = false;
	for ( i = path.begin(); i != path.end(); ++i )
	{
		if ( !motionInterrupted )
		{
			Roadmap::out_edge_i e, e_end;
			tie(e, e_end) = out_edges( i->second, roadmap->map );
			if ( e == e_end )
				return false;

			robot->setRefVelocity( refVelocity );
			robot->positionMove(roadmap->map[i->second].q);
			motionInterrupted = !waitForMotion();

			if ( !motionInterrupted ) {
				std::cout << "POSITION MOVE COMPLETE\n" << std::endl;
				roadmap->setEdgeColor( i->first, Qt::black ); 
				roadmap->setCurrentVertex( i->second );
				robot->setWaypoint();
			} else {
				QColor color = Qt::lightGray;
				std::cout << "POSITION MOVE INTERRUPTED\n" << std::endl;
				//roadmap->setEdgeColor( i->first, color.lighter() );
                roadmap->setEdgeColor( i->first, Qt::darkGray );
				roadmap->removeEdge( i->first );
			}
		} else {
			roadmap->setEdgeColor( i->first, Qt::darkGray );
		}
	}	
	return true;
}

bool ControlThread::simpleVelocityMoveImpl( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path )
{	
	int period = 20; //ms
	
	yarp::os::Bottle statusBottle;
	bool interrupted = false;
	
	// set the whole path to be red
	std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > >::iterator i;
	for ( i = path.begin(); i != path.end(); ++i )
		roadmap->setEdgeColor( i->first, Qt::red );
	
	Roadmap::CGAL_Point		s,		// (last) config space pose at time t-1
							p,		// (current) config space pose at time t
							q;		// curently active control point
	
	Roadmap::CGAL_Vector	v,//w,	// velocity (q-p)
							err,	// error
							//erl,
							cmd;	// control command
	
	std::cout << std::setprecision(3);
	std::cout << std::setw(4);
	
	p = currentPose();
	//roadmap->setCurrentVertex( nearestVertex );
	if ( !robot->setWaypoint() )
		printf("failed to set waypoint in the MoBeE history\n");
	
	for ( i = path.begin(); i != path.end(); ++i )
	{
		printf("\nNEW CONTROL POINT\n");
		q = roadmap->getCgalPose(target(i->first,roadmap->map));	// active control point
		
		do {
			
			// if MoBeE cut us off, give up
			MoBeEStatus.read(statusBottle);
			if ( statusBottle.get(0).asInt() == 0 )
			{
				interrupted = true;
				break;
			}
			
			msleep(period);
			
			s =		p;													// config at time t-1
			p =		currentPose();										// configuration at time t
			v =		1000*(p-s)/period;									// velocity at time t (degrees/second)
			err =	q-p;												// the error vector
		
			std::cout << "-------------------" << std::endl;
			std::cout << "|v|:     " << sqrt(v.squared_length()) << std::endl;
			std::cout << "|err|:   " << sqrt(err.squared_length()) << std::endl;
			std::cout << "maxDiff: " << robot->maxDiff( std::vector<double>( q.cartesian_begin(), q.cartesian_end() ),
													    std::vector<double>( p.cartesian_begin(), p.cartesian_end() )
													  ) << std::endl;
			
			
			cmd =	20.0 * ( 1 - pow( 2.71828183, (-0.001*err.squared_length()) ) ) * err/sqrt(err.squared_length());
			//std::cout << "cmd: " << cmd << "(" << sqrt(cmd.squared_length()) << ")" << std::endl;
			
			robot->setRefAcceleration( refAcceleration );
			robot->velocityMove( std::vector<double>( cmd.cartesian_begin(), cmd.cartesian_end() ) );
		} 
		while ( robot->maxDiff( std::vector<double>( q.cartesian_begin(), q.cartesian_end() ),
								std::vector<double>( p.cartesian_begin(), p.cartesian_end() )
							  ) > 5
				&& keepRunning
			  );
		
		// if MoBeE cut us off, give up
		if ( interrupted )
			break;
	}
	
	if ( interrupted ) {
		printf("motion interrupted.\n");
		while (true) {
			MoBeEStatus.read(statusBottle);
			if ( statusBottle.get(0).asInt() == 1 )
				break;
		}
		return false;
	}
	else {
		printf("motion complete. all stopped.\n");
		cmd = p-p;
		robot->setRefAcceleration( refAcceleration );
		robot->velocityMove( std::vector<double>( cmd.cartesian_begin(), cmd.cartesian_end() ) );
		for ( i = path.begin(); i != path.end(); ++i )
			roadmap->setEdgeColor( i->first, Qt::black );
		return true;
	}
}

bool ControlThread::velocityMoveImpl( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path )
{
	int period = 20; //ms
	
	yarp::os::Bottle statusBottle;
	bool interrupted = false;
	
	// set the whole path to be red
	std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > >::iterator i;
	for ( i = path.begin(); i != path.end(); ++i )
		roadmap->setEdgeColor( i->first, Qt::red );
	
	std::cout << std::setprecision(3);
	std::cout << std::setw(4);
	
	if ( !robot->setWaypoint() )
		printf("failed to set waypoint in the MoBeE history\n");
	
	Roadmap::CGAL_Vector v; // the velocity command
	for ( i = path.begin(); i != path.end(); ++i )
	{
		printf("\nNEW CONTROL POINT\n");
		do {
			// if MoBeE cut us off, give up
			MoBeEStatus.read(statusBottle);
			if ( statusBottle.get(0).asInt() == 0 )
			{
				interrupted = true;
				break;
			}
			
			msleep(period);
			
			std::cout << "-------------------" << std::endl;
			v = funnelAccel(20, i->first);
			std::cout << "|v|:     " << sqrt(v.squared_length()) << std::endl;
			//std::cout << "|err|:   " << sqrt(err.squared_length()) << std::endl;
			//std::cout << "maxDiff: " << robot->maxDiff( std::vector<double>( q.cartesian_begin(), q.cartesian_end() ),
			//										   std::vector<double>( p.cartesian_begin(), p.cartesian_end() )
			//										   ) << std::endl;
			//cmd =	20.0 * ( 1 - pow( 2.71828183, (-0.001*err.squared_length()) ) ) * err/sqrt(err.squared_length());
			//std::cout << "cmd: " << cmd << "(" << sqrt(cmd.squared_length()) << ")" << std::endl;
			
			robot->setRefAcceleration( refAcceleration );
			robot->velocityMove( std::vector<double>( v.cartesian_begin(), v.cartesian_end() ) );
		} while ( sqrt(v.squared_length()) > 5 && keepRunning );
		
		// if MoBeE cut us off, give up
		if ( interrupted )
			break;
	}
	
	if ( interrupted ) {
		printf("motion interrupted.\n");
		while (true) {
			MoBeEStatus.read(statusBottle);
			if ( statusBottle.get(0).asInt() == 1 )
				break;
		}
		return false;
	}
	else {
		printf("motion complete. all stopped.\n");
		v -= v;
		robot->setRefAcceleration( refAcceleration );
		robot->velocityMove( std::vector<double>( v.cartesian_begin(), v.cartesian_end() ) );
		for ( i = path.begin(); i != path.end(); ++i )
			roadmap->setEdgeColor( i->first, Qt::black );
		return true;
	}
}

Roadmap::CGAL_Vector ControlThread::funnelAccel( double c, Roadmap::edge_t e )
{
	Roadmap::CGAL_Point	p = roadmap->getCgalPose(source(e,roadmap->map)),	// last control point
						q = roadmap->getCgalPose(target(e,roadmap->map));	// active control point
	Roadmap::CGAL_Vector	err = currentPose()-q,							// configuration w.r.t. active control point (error vector)
							axial = p-q,									// active edge vector (axial direction)
							radial;											// shortest segment from t to the line q + a*t, where t is the parameter (radial direction)
	qreal errMag = sqrt(err.squared_length()),		// magnitude of error verctor
		  x,										// axial error
		  y;										// radial error
	
	axial /= sqrt(axial.squared_length());	// normalize the axial direction vector
	x = err * axial;						// compute the magnitude of the projection of qt onto qp
	radial = x*axial - err;					// find the radial direction by vector difference
	y = sqrt(radial.squared_length());		// compute the magnitude of the radial acceleration
	radial /= y;							// normalize the radial direction vector
	
	Roadmap::CGAL_Vector Aa,Ar;
	Aa = -c*x/qAbs(x) * ( 1 - pow( 2.71828183, -0.01*qAbs(x) ) ) * axial;
	Ar = c*( 1 - pow( 2.71828183, -0.1*y ) ) * radial; // y is positive by definition
	
	std::cout << "err = " << errMag << std::endl;
	std::cout << "x = " << x << std::endl;
	std::cout << "y = " << y << std::endl;
	std::cout << "|x+y| = " << sqrt(pow(x,2)+pow(y,2)) << std::endl;
	std::cout << "|Aa| = " << sqrt(Aa.squared_length()) << std::endl;
	std::cout << "|Ar| = " << sqrt(Ar.squared_length()) << std::endl;
	
	return Aa + Ar;
}

Roadmap::CGAL_Point ControlThread::currentPose()
{
	std::vector<double> p = robot->getCurrentPose();
	return Roadmap::CGAL_Point( p.size(), p.begin(), p.end() );
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
