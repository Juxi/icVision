#include "ArmController.h"
#include <QTime>

ArmController::ArmController( qreal _attenuationThreshold, qreal _attenuationFactor ) : 
	number_of_joints(NUMBEROFARMJOINTS),
	dd(NULL), pos(NULL), vel(NULL), enc(NULL), pid(NULL), amp(NULL), lim(NULL), tgt(NULL)
{
	attenuationThreshold = _attenuationThreshold;	// 0-1, where smaller numbers favor precision
	attenuationFactor = _attenuationFactor;			// 0-inf larger numbers cause the controller to approach target points more slowly
}

ArmController::~ArmController()
{
	//printf("Deleting %s %s\n", robotName.toStdString().c_str(), partName.toStdString().c_str());
	stop();
}

void ArmController::addControlPoint(float a[NUMBEROFARMJOINTS]) {
	ControlPoint next;
	
	next.t = 0;
	
	for (int i = 0; i < number_of_joints; i++) {
		// TODO check limits?
		limits[i].min = 0;
		limits[i].max = 0;
		next.p[i] = a[i];
	}
	
	trajectory.append(next);
	if( tgt == NULL ) tgt = trajectory.begin();
	
}

void ArmController::connect( const char* _robotName, const char* _partName, int refAccel, int refSpeed )
{
	if ( !network.checkNetwork() ) { throw QString("Cannot find YARP network."); }
	
	robotName = _robotName;
	partName = _partName;
	
	yarp::os::Property options;
	options.put( "robot", robotName.toStdString().c_str() ); // typically from the command line.
	options.put( "device", "remote_controlboard" );
	
	QString localPort = "/armWrestling/" + partName + "/control";
	options.put("local", localPort.toStdString().c_str());
	
	QString remotePort = "/" + robotName + "/" + partName;
	options.put("remote", remotePort.toStdString().c_str());
	
	dd = new yarp::dev::PolyDriver(options);
	
	if ( dd )
	{
		dd->view(pos);
		dd->view(vel);
		dd->view(enc);
		dd->view(pid);
		dd->view(amp);
		dd->view(lim);
	}
	
	checkValidity();
	
	printf("yarp::dev::PolyDriver passed validity check\n");
	
	int numJoints;
	pos->getAxes(&numJoints);
	if ( numJoints != number_of_joints )
	{
		QString err = QString("Wrong number of DOFs: ArmController must connect to an arm of the iCub robot, which has %1 joints. The requested device ").arg(number_of_joints);
		err.append(remotePort); err.append(" has "); err.append(numJoints);	err.append(" joints.");
		throw err;
	}
	
	printf("Fluuuuf A\n");
	
	for (int i = 0; i < number_of_joints; i++)
	{
		pos->setRefAcceleration(i, refAccel);
		pos->setRefSpeed(i, refSpeed );
		lim->getLimits(i, &(limits[i].min), &(limits[i].max));
		amp->enableAmp(i);
		pid->enablePid(i);
	}		
	
	printf("ArmController started successfully.\n");
}

void ArmController::checkValidity()
{
	if ( !dd ) { throw QString("yarp::dev::PolyDriver was not created"); }
	else if ( !dd->isValid() ) { throw QString("yarp::dev::PolyDriver is invalid"); }
	if (!pos) { throw QString("IPositionControl Error!"); }
	if (!vel) { throw QString("IVelocityControl Error!"); }
	if (!enc) { throw QString("IEncoders Error!"); }
	if (!pid) { throw QString("IPidControl Error!"); }
	if (!amp) { throw QString("IAmplifierControl Error!"); }
	if (!lim) { throw QString("IControlLimits Error!"); }
}

void ArmController::stop()
{
	// checkValidity();
	
	/* this just closes the driver: OK */
	if (dd)
	{ 
		dd->close();
//		delete dd;
	}
}

void ArmController::initialPose()
{
	printf("[HEAD] Initializing pose...\n");
	checkValidity();
	tgt = trajectory.begin(); 
	blockingPositionMove(tgt++);
	printf("[HEAD] Reached initial pose...\n");
}

void ArmController::blockingPositionMove( ControlPoint* target )
{
	bool ok;
	for (int i = 0; i < number_of_joints; i++ )
	{
		ok = pos->positionMove( i, target->p[i] );
		printf("%f,		", target->p[i]);
		if ( !ok ) { throw QString("Position move failed"); }
	}
	
	bool finished = false;
	while ( !finished )
	{
		pos->checkMotionDone(&finished);
	}
}

qreal ArmController::doVelocityControl( int speed, bool stopOnTouch )
{
	char buf[1024];
	
	// get encoder positions and tabulate error
	max = 0.0;
	//qreal meanErr = 0.0;
	enc->getEncoders(q);
	sprintf(buf, "[HEAD] Position Error:\t");
	for ( i = 0; i < number_of_joints; i++ )
	{
		if(i != 2) continue;
		err[i] = tgt->p[i] - q[i];
		ERR[i] = qAbs( err[i] );
		//meanErr += ERR[i];
		if ( ERR[i] > max ) { max = ERR[i]; }
		sprintf(buf, "%s%f\t", buf, err[i] );
	}
	if ( qFuzzyIsNull(max) ) { return 0; }
	//meanErr /= 9.0;
	
	// do feedback velocity control
	qreal meanAtt = 0.0;
	//printf("Velocity Move:\t");
	for ( i = 0; i < number_of_joints; i++ )
	{
		if(i != 2 ) continue;
		t = ERR[i]/attenuationFactor;
		att = -exp(-t)*(1+t)+1;
		cmd = att * speed * err[i]/max;
		vel->velocityMove(i, cmd);
		//printf( "%f\t", cmd );
		meanAtt += att;
	}
	meanAtt /= 9.0;
	
	//printf(" meanCmd = %f", mean );
	printf("%s\n", buf);
	return meanAtt;
}


void ArmController::run() {
	
	float speed = 15.0;
	
	printf("[HEAD] Thread running!\n");
	
	tgt = trajectory.begin();
	
	qreal currentAttenuation;
	checkValidity();
	while ( true )
	{
		
		currentAttenuation = doVelocityControl( speed );
		usleep(20);
		
		// move through the vector of control points
		if ( currentAttenuation <= attenuationThreshold )
		{
			if ( tgt != trajectory.end() - 1 ) 
			{
				printf("[ARM] goto next target\n");
				tgt++;
			}
			else if ( currentAttenuation <= attenuationThreshold/10 )
			{
				// stop everything
				for ( i = 0; i < number_of_joints; i++ ) {
					vel->velocityMove( i, 0 );
				}
				break;
			}
		}
	}
	
	
	printf("[HEAD] FINISHED!\n");
	
}