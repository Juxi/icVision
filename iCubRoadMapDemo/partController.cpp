#include "partController.h"
#include <time.h>

PartController::PartController() : numJoints(0), jointMask(NULL), dd(NULL)
{
}

PartController::~PartController()
{
	close();
}

bool PartController::open( const char* robotName, const char* partName )
{
	printf( "Opening Remote Control Board: %s %s\n", robotName, partName );
	
	if ( !network.checkNetwork() )
	{
		printf("Cannot find YARP network.\n");
		return 0;
	}
	
	yarp::os::Property options;
	options.put( "robot", robotName ); // typically from the command line.
	options.put( "device", "remote_controlboard" );
	
	std::string remotePort = "/";
	remotePort.append( robotName );
	remotePort.append( "/" );
	remotePort.append( partName );
	
	std::string localPort = remotePort;
	localPort.append( "/control" );
	
	options.put( "local", localPort.c_str() );
	options.put( "remote", remotePort.c_str() );
	
	dd = new yarp::dev::PolyDriver(options);
	if (!dd->isValid())
	{
		printf("Device not available. Failed to create PolyDriver.");
		return 0;
	}

	dd->view(pos);	if (pos==0) { printf("IPositionControl Error!"); return 0; }
	dd->view(vel);	if (vel==0) { printf("IVelocityControl Error!"); return 0; }
	dd->view(enc);	if (enc==0) { printf("IEncoders Error!"); return 0; }
	dd->view(pid);	if (pid==0) { printf("IPidControl Error!"); return 0; }
	dd->view(amp);	if (amp==0) { printf("IAmplifierControl Error!"); return 0; }
	dd->view(lim);	if (lim==0) { printf("IControlLimits Error!"); return 0; }
	
	pos->getAxes(&numJoints);
	jointMask = new bool[numJoints];
	
	for ( int i = 0; i < numJoints; i++ )
	{
		pos->setRefAcceleration(i, 20);
		amp->enableAmp(i);
		pid->enablePid(i);
		jointMask[i] = true;
	}
	
	/* initialize random seed: */
	srand ( time(NULL) );

	return 1;
}

bool PartController::isValid()
{
	if ( !dd ) { return 0; }
	else if ( !dd->isValid() || !pos || !vel || !enc || !pid || !amp || !lim ) { return 0; }
	return 1;
}

void PartController::close()
{
	/* this just closes the driver: OK */
	if ( dd )
	{ 
		dd->close();
		delete dd;
		dd = NULL;
	}
	
	numJoints = 0;
	if ( jointMask )
	{
		delete[] jointMask;
		jointMask = NULL;
	}
	
}

bool PartController::stop()
{ 
	if ( !vel ) { return 0; }
	return vel->stop();
}

bool PartController::positionMove( double* q, double* v )
{ 
	if ( !pos ) { return 0; }
	if ( v )
	{ 
		if ( !pos->setRefSpeeds( v ) ) { return 0; }
	}
	return pos->positionMove( q );
}

bool PartController::setJointMask( int jointNum, bool val )
{
	if ( !jointMask || jointNum >= numJoints ) { return 0; }
	jointMask[jointNum] = val;
	return 1;
}

bool PartController::checkMotionDone( bool* flag )
{
	if ( !pos ) { return 0; }
	return pos->checkMotionDone( flag );
}

bool PartController::getRandomPose( double* q )
{
	if ( !enc ) { return 0; }
	
	double min, max;
	
	enc->getEncoders( q );
	
	for ( int i = 0; i < numJoints; i++ )
	{
		if ( jointMask[i] )
		{
			lim->getLimits( i, &min, &max );
			q[i] = min + (max-min) * (double)rand()/RAND_MAX;
		}
	}
	
	return 1;
}

std::vector<double> PartController::getRandomPose()
{
	double min, max, val;
	std::vector<double> q;
	if ( enc )
	{
		for ( int i = 0; i < numJoints; i++ )
		{
			enc->getEncoder( i, &val );
			q.push_back( val );
			if ( jointMask[i] )
			{
				q.pop_back();
				lim->getLimits( i, &min, &max );
				q.push_back( min + (max-min) * (double)rand()/RAND_MAX );
			}
		}
	}
	return q;
}
