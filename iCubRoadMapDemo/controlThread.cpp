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

void ControlThread::run()
{
	while ( keepRunning )
	{
		/*** EXPLORE THE GRAPH VIA RANDOM SHORTEST-PATHS ***/
		//else 
		if ( currentBehavior == MultiEdgeExplore )
		{
			std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path;
			path = roadmap->aToB( roadmap->currentVertex, Roadmap::vertex_t(rand()%num_vertices(roadmap->map)) );
		
			if ( !velocityMoveImpl( path ) )
			//if ( !simpleVelocityMoveImpl( path ) )
			{
				printf("velocityMoveImpl() failed.\n");
				//break;
			}
		}
		
		/*** REACH FOR A PARTICULAR OBJECT ***/
		else if ( currentBehavior == GoToObject )
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
					velocityMoveImpl( roadmap->aToB( roadmap->currentVertex, graspingVertex ) );
				}
				
			}
		}
		yarp::os::Time::delay(1);
	}
	printf("*** RUN METHOD RETURNED ***\n");
}

bool ControlThread::velocityMoveImpl( std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > > path )
{
	int period = 20; //ms
	double vMax = 30; // [0,100]
	
	yarp::os::Bottle statusBottle;
	bool interrupted = false;
	
	// set the whole path to be red
	std::list< std::pair< Roadmap::edge_t, Roadmap::vertex_t > >::iterator i,j;
	for ( i = path.begin(); i != path.end(); ++i )
	{
		roadmap->setEdgeColor( i->first, Qt::red );
		roadmap->setEdgeWeight( i->first, 3 );
	}
	
	std::cout << std::setprecision(3);
	std::cout << std::setw(4);
	
	if ( !robot->setWaypoint() )
		printf("failed to set waypoint in the MoBeE history\n");
	
	std::pair<Roadmap::CGAL_Vector,Roadmap::CGAL_Vector> vIn,vOut; // the velocities from the in edge
	Roadmap::CGAL_Vector v;
	
	//double att;
	for ( i = path.begin(); i != path.end(); ++i )
	{
		printf("\nNEW CONTROL POINT\n");
		j = i;
		j++;

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
			
			// compute velocity control signal
			vIn = funnelAccel(	vMax,	i->first );
			if ( j != path.end() ) {
				vOut = funnelAccel(	vMax,	j->first );
				v = (vIn.first+vIn.second+vOut.first)/3;
			} else {
				vOut.first = 0;
				vOut.second = 0;
				v = (vIn.first+vIn.second)/2;
			}
			
			//std::cout << "|vIn.first|:     " << sqrt(vIn.first.squared_length()) << std::endl;
			//std::cout << "|vIn.second|:    " << sqrt(vIn.second.squared_length()) << std::endl;
			//std::cout << "|vOut.first|:    " << sqrt(vOut.first.squared_length()) << std::endl;
			std::cout << "|v|:             " << sqrt(v.squared_length()) << std::endl;
			
			robot->setRefAcceleration( refAcceleration );
			robot->velocityMove( std::vector<double>( v.cartesian_begin(), v.cartesian_end() ) );
		} while ( vIn.first.squared_length() > vOut.first.squared_length() && v.squared_length() > 5 && keepRunning );
		
		if ( !keepRunning ) { // we don't know if the edge is good or not, mark it untested
			roadmap->setEdgeColor( i->first, Qt::black );
			roadmap->setEdgeWeight( i->first, 1 );
		}
		else if ( !interrupted ) { // the edge is good, mark it bold black
			roadmap->setEdgeColor( i->first, Qt::black );
		}
		else { // the edge is bad, get rid of it and mark the rest untested
			roadmap->removeEdge( i->first );
			while ( ++i != path.end() ) {
				roadmap->setEdgeColor( i->first, Qt::black );
				roadmap->setEdgeWeight( i->first, 1 );
			}
			break;
		}
	}
	
	if ( interrupted ) {
		printf("motion interrupted.\n");
		// wait for the MoBeE filter to reopen
		// TODO: implement timeout
		while (true) {
			MoBeEStatus.read(statusBottle);
			if ( statusBottle.get(0).asInt() == 1 )
				break;
		}
		return false;
	}
	else {
		printf("motion complete. all stopped.\n");
		robot->setRefAcceleration( refAcceleration );
		Roadmap::CGAL_Vector v = zeroVector();
		robot->velocityMove( std::vector<double>( v.cartesian_begin(), v.cartesian_end() ) );
		//for ( i = path.begin(); i != path.end(); ++i )
		//	roadmap->setEdgeColor( i->first, Qt::black );
		return true;
	}
}

std::pair<Roadmap::CGAL_Vector,Roadmap::CGAL_Vector> ControlThread::funnelAccel( double c, Roadmap::edge_t e )
{
	Roadmap::CGAL_Vector err = positionErr(e);	// configuration w.r.t. active control point (error vector)
	qreal errMag = sqrt(err.squared_length()),	// magnitude of error verctor
		  x,									// axial error
		  y;									// radial error
	
	Roadmap::CGAL_Point	p = roadmap->getCgalPose(source(e,roadmap->map)),	// last control point
						q = roadmap->getCgalPose(target(e,roadmap->map));	// active control point
	Roadmap::CGAL_Vector	axial = p-q,									// active edge vector (axial direction)
							radial;											// shortest segment from t to the line q + a*t, where t is the parameter (radial direction)
	
	axial /= sqrt(axial.squared_length());	// normalize the axial direction vector
	x = err * axial;						// compute the magnitude of the projection of qt onto qp
	radial = x*axial - err;					// find the radial direction by vector difference
	y = sqrt(radial.squared_length());		// compute the magnitude of the radial acceleration
	radial /= y;							// normalize the radial direction vector
	
	std::pair<Roadmap::CGAL_Vector,Roadmap::CGAL_Vector> a;
	a.first = -c*x/qAbs(x) * ( 1 - pow( 2.71828183, -0.01*qAbs(x) ) ) * axial;
	a.second = c*( 1 - pow( 2.71828183, -0.1*y ) ) * radial; // y is positive by definition
	
	/*std::cout << "err = " << errMag << std::endl;
	std::cout << "x = " << x << std::endl;
	std::cout << "y = " << y << std::endl;
	std::cout << "|x+y| = " << sqrt(pow(x,2)+pow(y,2)) << std::endl;
	std::cout << "|Aa| = " << sqrt(a.first.squared_length()) << std::endl;
	std::cout << "|Ar| = " << sqrt(a.second.squared_length()) << std::endl;*/
	
	return a;
}


Roadmap::CGAL_Vector ControlThread::positionErr( Roadmap::edge_t edge )
{
	std::vector<double> err = robot->diff( roadmap->getStdPose(target(edge,roadmap->map)) );
	Roadmap::CGAL_Vector CGAL_err( err.size(), err.begin(), err.end() );
	return -CGAL_err;
}
	
Roadmap::CGAL_Vector ControlThread::zeroVector()
{
	std::vector<double> zero;
	for ( int i = 0; i < robot->getNumJoints(); i++ )
		zero.push_back(0.0);
	return Roadmap::CGAL_Vector(zero.size(),zero.begin(),zero.end());
}

double ControlThread::dot(Roadmap::edge_t i,Roadmap::edge_t o)
{
	Roadmap::CGAL_Point	p = roadmap->getCgalPose(source(i,roadmap->map)),
						q = roadmap->getCgalPose(target(i,roadmap->map)),
						r = roadmap->getCgalPose(target(o,roadmap->map));
	Roadmap::CGAL_Vector	in  = q-p,
							out = r-q;
	
	in /= sqrt(in.squared_length());
	out /= sqrt(out.squared_length());
	
	std::cout << "i:   " << i << std::endl;
	std::cout << "o:   " << o << std::endl;
	
	std::cout << "p:   " << p << std::endl;
	std::cout << "r:   " << r << std::endl;
	std::cout << "q:   " << q << std::endl;
	std::cout << "out: " << out << std::endl;
	
	return in*out;
}

/*Roadmap::CGAL_Point ControlThread::currentPose()
{
	std::vector<double> p = robot->getCurrentPose();
	return Roadmap::CGAL_Point( p.size(), p.begin(), p.end() );
}*/

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
