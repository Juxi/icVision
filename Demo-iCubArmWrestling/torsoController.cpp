#include "TorsoController.h"
#include <QTime>

TorsoController::TorsoController( qreal _attenuationThreshold, qreal _attenuationFactor ) : 
	number_of_joints(NUMBEROFTORSOJOINTS),
	dd(NULL), pos(NULL), vel(NULL), enc(NULL), pid(NULL), amp(NULL), lim(NULL), tgt(NULL)
{
	attenuationThreshold = _attenuationThreshold;	// 0-1, where smaller numbers favor precision
	attenuationFactor = _attenuationFactor;			// 0-inf larger numbers cause the controller to approach target points more slowly
}

TorsoController::~TorsoController()
{
	//printf("Deleting %s %s\n", robotName.toStdString().c_str(), partName.toStdString().c_str());
	stop();
}

void TorsoController::addControlPoint(float a[NUMBEROFTORSOJOINTS]) {
	ControlPoint next;
	
	next.t = 0;
	
	// TODO add timing
	//  A.t = 0whi;
	//	B.t = 0.25;
	//	C.t = 0.5;
	//	D.t = 0.75;
	//	E.t = 1;
	
	//	// joint num	7	8	9	10	11	12	13	14	15
	//	// First position (used for pre and final for open)
	//	qreal a[9] = {	0,	60,	0,	0,	10,	10,	10,	10,	15 };
	//	qreal b[9] = {	0,	60,	30,	0,	25,	10,	20,	10,	15 };
	//	qreal c[9] = {	0,	60,	30,	0,	25,	10,	20,	10,	15 };
	//	qreal d[9] = {	0,	60,	30,	30,	25,	55,	20,	100,	130 };
	//	// Final position (used as last position for close)
	//	qreal e[9] = {	0,	60,	30,	30,	25,	55,	20,	100,	130 };
	//	
	
	for (int i = 0; i < number_of_joints; i++) {
		// TODO check limits?
		limits[i].min = 0;
		limits[i].max = 0;
		next.p[i] = a[i];
	}

	trajectory.append(next);
	if( tgt == NULL ) tgt = trajectory.begin();

}

void TorsoController::connect( const char* _robotName, const char* _partName, int refAccel, int refSpeed )
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
		QString err = QString("Wrong number of DOFs: TorsoController must connect to an arm of the iCub robot, which has %1 joints. The requested device ").arg(number_of_joints);
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

	printf("TorsoController started successfully.\n");
}

void TorsoController::checkValidity()
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

void TorsoController::stop()
{
	// checkValidity();
	
	/* this just closes the driver: OK */
//?!?!	if (dd)
//	{ 
//		dd->close();
//		delete dd;
//	}
}

void TorsoController::initialPose()
{
	printf("[TORSO] Initializing pose...\n");
	checkValidity();
	tgt = trajectory.begin(); 
	blockingPositionMove(tgt++);
	printf("[TORSO] Reached initial pose...\n");
}

void TorsoController::blockingPositionMove( ControlPoint* target )
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

qreal TorsoController::doVelocityControl( int speed, bool stopOnTouch )
{
	// get encoder positions and tabulate error
	max = 0.0;
	//qreal meanErr = 0.0;
	enc->getEncoders(q);
	printf("[TORSO] Position Error:\t");
	for ( i = 0; i < number_of_joints; i++ )
	{
		err[i] = tgt->p[i] - q[i];
		ERR[i] = qAbs( err[i] );
		//meanErr += ERR[i];
		if ( ERR[i] > max ) { max = ERR[i]; }
		printf( "%f\t", err[i] );
	}
	if ( qFuzzyIsNull(max) ) { return 0; }
	//meanErr /= 9.0;
		
	// do feedback velocity control
	qreal meanAtt = 0.0;
	//printf("Velocity Move:\t");
	for ( i = 0; i < number_of_joints; i++ )
	{
		t = ERR[i]/attenuationFactor;
		att = -exp(-t)*(1+t)+1;
		cmd = att * speed * err[i]/max;
		vel->velocityMove(i, cmd);
		//printf( "%f\t", cmd );
		meanAtt += att;
	}
	meanAtt /= 9.0;
	
	//printf(" meanCmd = %f", mean );
	printf("\n");
	return meanAtt;
}


void TorsoController::run() {
	float speed = 10.0;
	
	printf("[TORSO] Thread running!\n");	
	
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
				printf("[TORSO] goto next target\n");
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
	
	printf("[TORSO] FINISHED!\n");
	
}