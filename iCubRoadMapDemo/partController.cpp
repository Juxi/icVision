#include "partController.h"
#include <time.h>

PartController::PartController() : numJoints(0), /*jointMask(NULL),*/ dd(NULL)
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
	
	double _min,_max;
	for ( int i = 0; i < numJoints; i++ )
	{
		pos->setRefAcceleration(i, 20);
		amp->enableAmp(i);
		pid->enablePid(i);
		lim->getLimits( i, &_min, &_max );
		
		//jointMask.push_back(true);
		min.push_back(_min);
		max.push_back(_max);
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
	//jointMask.clear();	
}

bool PartController::stop()
{ 
	if ( !vel ) { return 0; }
	return vel->stop();
}

bool PartController::positionMove( const std::vector<double>& poss )
{
	if ( poss.size() != (unsigned int)numJoints || !pos ) { return 0; }
	
	double p[numJoints];
	for ( int i=0; i<numJoints; i++ )
	{
		p[i] = poss.at(i);
	}
	return pos->positionMove( p );
}
								  
/*bool PartController::positionMove( const std::vector<double>& poss, const std::vector<double>& vels )
{ 
	if ( !vels.empty() && vels.size() != (unsigned int)numJoints ) { return 0; }
	
	if ( !vels.empty() )
	{ 
		double v[numJoints];
		for ( int i=0; i<numJoints; i++ )
		{
			v[i] = vels.at(i);
		}
		if ( !pos->setRefSpeeds( v ) ) { return 0; }
	}
	return positionMove(poss);
}*/

bool PartController::setJointMask( const std::vector<bool>& vals )
{
	if ( vals.size() != (unsigned int)numJoints ) { return 0; }
	//jointMask = vals;
	return 1;
}

bool PartController::checkMotionDone( bool* flag )
{
	if ( !pos ) { return 0; }
	return pos->checkMotionDone( flag );
}

std::vector<double> PartController::getRandomPose()
{
	std::vector<double> q;
	for ( int i = 0; i < numJoints; i++ )
	{
		if ( i <= 6 )
		{
			q.push_back( min.at(i) + (max.at(i)-min.at(i)) * (double)rand()/RAND_MAX );
		}
		else { q.push_back(0); }
	}
	return q;
}

std::vector<double> PartController::getCurrentPose()
{
	double val;
	std::vector<double> q;
	if ( enc )
	{
		for ( int i = 0; i < numJoints; i++ )
		{
			enc->getEncoder( i, &val );
			q.push_back(val);
		}
	}
	
	return q;
}
