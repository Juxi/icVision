#include "partController.h"

PartController::PartController( const char* _robotName, const char* _partName, int r ) : yarp::os::RateThread(r), 
																							vel(NULL),
																							enc(NULL), 
																							q1(NULL),
																							q0(NULL)
{
	printf( "Opening Remote Control Board: %s %s\n", _robotName, _partName );
	
	// make sure we can find a yarp server
	if ( !network.checkNetwork() ) {
		printf("Cannot find YARP network.\n");
	}
	
	// Make local and remote port names
	yarp::os::Property options;
	options.put( "robot", _robotName );
	options.put( "device", "remote_controlboard" );
	yarp::os::ConstString remotePort("/");
	remotePort += _robotName;
	remotePort += "/";
	remotePort += _partName;
	yarp::os::ConstString localPort = remotePort;
	localPort += "/control";
	options.put( "local", localPort.c_str() );
	options.put( "remote", remotePort.c_str() );
	
	//sleep(1);
	
	// connect to the iCub control board interface
	dd = new yarp::dev::PolyDriver(options);
	if (!dd->isValid()) {
		printf("Device not available. Failed to create PolyDriver.\n");
	}
	
	// interface to get motor encoder positions
	dd->view(enc);
	if ( !enc ) printf("IEncoder Error!\n");
	
	// interface to do velocity control
	dd->view(vel);
	if ( !vel ) printf("IVelocityControl Error!\n");
	
	// get number of DOFs and joint position limits, and initialize control parameters
	yarp::dev::IPositionControl *pos;
	yarp::dev::IControlLimits   *lim;
	dd->view(lim);
	dd->view(pos);
	if ( !lim ) printf("Joint Limits Error!\n");
	if ( !pos ) printf("IPositionControl Error!\n");
	if ( pos && lim ) {
		double _min,_max;
		pos->getAxes(&numJoints);
		min = new double[numJoints];
		max = new double[numJoints];
		q1  = new double[numJoints];
		q0	= new double[numJoints];
		w	= new double[numJoints];
		x	= new double[numJoints];
		k	= new double[numJoints];
		c	= new double[numJoints];
		e	= new double[numJoints];
		v	= new double[numJoints];
		a	= new double[numJoints];
		cmd	= new double[numJoints];
		for ( int i = 0; i < numJoints; i++ ) {
			lim->getLimits( i, &_min, &_max );
			min[i] = _min;
			max[i] = _max;
			w[i] = 1.0;
			k[i] = 1000.0;
			c[i] = 20.0;
			//printf("joint %d: min = %f max = %f\n",i,_min,_max);
		}
		port.open("/partController");
		srand ( yarp::os::Time::now() );
	} //else { throw("could not connect to robot!"); }
}

bool PartController::isValid()
{
	if ( !dd || !dd->isValid() || !vel || !enc )
		return false;
	return true;
}

bool PartController::threadInit()
{
	if ( !isValid() ) {
		printf("Device driver not valid.\n");
		return false;
	}
	printf("Starting PartController\n");
	return true;
}

//called by start after threadInit, s is true iff the thread started
//successfully
void PartController::afterStart(bool s)
{
	if (s) {
		printf("PartController started successfully\n");
		// set the attractor to the current pose
		enc->getEncoders(q1);
		for ( int j=0; j<numJoints; j++ ) {
			q0[j] = q1[j];
			x[j] = q1[j];
		}
	}
	else
		printf("PartController did not start\n");
}

bool PartController::set( double **var, yarp::os::Bottle* list )
{
	if ( list->size() != numJoints ) return false;
	for ( int i = 0; i < numJoints; i++ )
		(*var)[i] = list->get(i).asDouble();
	return true;
}

void PartController::run() 
{
	if ( !isValid() ) {
		printf("Device driver not valid.\n");
		return;
	}
	
	// move the attractor and/or change control parameters
	yarp::os::Bottle* b = NULL;
	b = port.read(false);
	if ( b ) {
		//printf("got a bottle: %s\n", b->toString().c_str());
		yarp::os::Bottle* list = b->get(1).asList();
		if ( b->get(0).asVocab() == yarp::os::Vocab::encode("x") )	{
			set( &x, list );
			printf("Set Attractor Position!!! (%s)\n", list->toString().c_str());
		}
		/*if ( b->get(0).asVocab() == yarp::os::Vocab::encode("w") )	set( &w, list );
		if ( b->get(0).asVocab() == yarp::os::Vocab::encode("k") )	set( &k, list );
		if ( b->get(0).asVocab() == yarp::os::Vocab::encode("c") )	set( &c, list );*/
	}
	
	
	for ( int j=0; j<numJoints; j++ )
		q0[j] = q1[j];
	
	// get new encoder positions
	enc->getEncoders(q1);
	
	for ( int i=0; i<numJoints; i++ ) {
		e[i] = w[i]*(x[i] - q1[i]);
		v[i] = 1000.0 * (q1[i] - q0[i]) / getRate();
		a[i] = -c[i]*v[i] + k[i]*e[i];
		cmd[i] = v[i] + a[i] * getRate() / 1000.0;
		
		if ( i == 2 ) {
			//printf("w: %f, k: %f, c: %f, e: %f   ", w[i], k[i], c[i], e[i]);
			//printf("v: %f, e: %f, a: %f, cmd: %f   ", v[i], e[i], a[i], cmd[i]);
			//printf("q1: %f, q0: %f   ", q1[i], q0[i]);
			//printf("Vn: %f,  Vo: %f, q: %f,  e: %f,  ms: %f ", cmd[i], v[i], q1[i], e[i], getRate());
		}
	}
	printf("\n");
	
	printf("---------------------------------\n");
	//printf("q1: (%f %f %f)\n", q1[0], q1[1], q1[2]);
	//printf("q0: (%f %f %f)\n", q0[0], q0[1], q0[2]);
	printf("e: (%f %f %f),  ", e[0], e[1], e[2]);
	printf("v: (%f %f %f),  ", v[0], v[1], v[2]);
	//printf("a: (%f %f %f)\n", a[0], a[1], a[2]);
	printf("cmd: (%f %f %f)\n", cmd[0], cmd[1], cmd[2]);
	
	vel->velocityMove( cmd );
}

void PartController::threadRelease()
{
	vel->stop();
	
	printf("Goodbye from PartController\n");
	if ( dd ) { 
		dd->close();
		delete dd;
		dd = NULL;
		enc = NULL;
		vel = NULL;
	}
	
	numJoints = 0;
	
}

/*bool PartController::stop()
{ 
	if ( !vel ) { return 0; }
	return vel->stop();
}

std::vector<double> PartController::withinLimits( const std::vector<double>& poss )
{
	std::vector<double> p;
	if ( poss.size() != (unsigned int)numJoints )
	{ 
		printf("PartController::withinLimits() received wrong sized position vector.");
		return p;
	}
	
	double offset = 0;
	for ( int i = 0; i < numJoints; i++ ) {
		//offset = (max.at(i) - min.at(i))/20.0;
		if ( poss.at(i) < min.at(i) + offset )
			p.push_back( min.at(i) + offset );
		else if ( poss.at(i) > max.at(i) - offset )
			p.push_back( max.at(i) - offset );
		else
			p.push_back(poss.at(i));
	}
	return p;
}

bool PartController::isWithinLimits( const std::vector<double>& poss )
{
	std::vector<double> p;
	if ( poss.size() != (unsigned int)numJoints )
	{ 
		printf("PartController::isWithinLimits() received wrong sized position vector.");
		return false;
	}
	
	double offset = 0;
	for ( int i = 0; i < numJoints; i++ ) {
		//offset = (max.at(i) - min.at(i))/20.0;
		if ( poss.at(i) < min.at(i) + offset )
			return false;
		else if ( poss.at(i) > max.at(i) - offset )
			return false;
	}
	return true;
}

bool PartController::velocityMove( const std::vector<double>& v )
{
	if ( v.size() != (unsigned int)numJoints || !pos ) { return 0; }
	
	//std::vector<double> q = getCurrentPose();
	
	//printf("velocity move: " );
	double cmd[numJoints];
	for ( int i=0; i<numJoints; i++ )
	{
		if ( jointMask.at(i) )
			cmd[i] = v.at(i);
		else
			cmd[i] = 0.0;
		//printf("%f ", cmd[i]);
	}
	//printf("\n\n");
	
	return vel->velocityMove( cmd );
	//return pos->positionMove( p );
}



std::vector<double> PartController::getRandomPose()
{
	std::vector<double> q = getCurrentPose();
	for ( int i = 0; i < numJoints; i++ )
	{
		if ( weights.at(i) )
			q.at(i) = min.at(i) + (max.at(i)-min.at(i)) * (double)rand()/RAND_MAX;
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
}*/
