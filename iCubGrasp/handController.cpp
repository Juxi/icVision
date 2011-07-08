#include "handController.h"
#include <QTime>
//#include <QMATH_H>

//HandController::HandController()
//{
//	HandController(  );
//}
HandController::HandController( qreal _attenuationThreshold, qreal _attenuationFactor ) : 
																	dd(NULL), pos(NULL), vel(NULL), enc(NULL), pid(NULL), amp(NULL), lim(NULL), tgt(NULL),
																	palmTouch(false), thumbTouch(false), indexTouch(false), middleTouch(false), ringTouch(false), littleTouch(false)
{
	attenuationThreshold = _attenuationThreshold;	// 0-1, where smaller numbers favor precision
	attenuationFactor = _attenuationFactor;			// 0-inf larger numbers cause the controller to approach target points more slowly
	
	ControlPoint A,B,C,D,E;
	
	A.t = 0;
	B.t = 0.25;
	C.t = 0.5;
	D.t = 0.75;
	E.t = 1;
	
	// joint num	7	8	9	10	11	12	13	14	15
//	qreal a[9] = {	15,	0,	0,	0,	0,	10,	10,	10,	10 };
//	qreal b[9] = {	15,	30,	0,	0,	30,	10,	40,	10,	20 };
//	qreal c[9] = {	15,	50,	0,	0,	50,	10,	60,	10,	30 };
//	qreal d[9] = {	15,	70,	0,	0,	70,	10,	80,	10,	50 };
//	qreal e[9] = {	15,	90,	0,	0,	90,	90,	90,	90,	70 };


	qreal a[9] = {	50,	5,	5,	5,	5,	10,	10,	10,	10 };
	qreal b[9] = {	50,	60,	30,	30,	30,	10,	40,	10,	20 };
	qreal c[9] = {	50,	60,	30,	30,	30,	50,	40,	50,	20 };
	qreal d[9] = {	15,	70,	5,	5,	70,	10,	80,	10,	50 };
	qreal e[9] = {	15,	90,	5,	5,	90,	90,	90,	90,	70 };


	
	for (int i = 0; i < 9; i++)
	{
		limits[i].min = 0;
		limits[i].max = 0;
		A.p[i] = a[i];
		B.p[i] = b[i];
		C.p[i] = c[i];
		D.p[i] = d[i];
		E.p[i] = e[i];
	}
	
	graspTrajectory.append(A);
	graspTrajectory.append(B);
	graspTrajectory.append(C);
//	graspTrajectory.append(D);
//	graspTrajectory.append(E);
	tgt = graspTrajectory.begin();
}
HandController::~HandController()
{
	//printf("Deleting %s %s\n", robotName.toStdString().c_str(), partName.toStdString().c_str());
	stop();
	delete[] q;
}

void HandController::start( const char* _robotName, const char* _partName, int refAccel, int refSpeed )
{
	if ( !network.checkNetwork() ) { throw QString("Cannot find YARP network."); }
	
	robotName = _robotName;
	partName = _partName;
	
	yarp::os::Property options;
	options.put( "robot", robotName.toStdString().c_str() ); // typically from the command line.
	options.put( "device", "remote_controlboard" );
	
	QString localPort = "/" + robotName + "/" + partName + "/hand_control";
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
	if ( numJoints != 16 )
	{
		QString err = "Wrong number of DOFs: HandController must connect to an arm of the iCub robot, which has 16 joints. The requested device ";
		err.append(remotePort); err.append(" has "); err.append(numJoints);	err.append(" joints.");
		throw err;
	}
	
	printf("Fluuuuf A\n");

	for (int i = 7; i < 16; i++)
	{
		pos->setRefAcceleration(i, refAccel);
		pos->setRefSpeed(i, refSpeed );
		lim->getLimits(i, &(limits[i-7].min), &(limits[i-7].max));
		amp->enableAmp(i);
		pid->enablePid(i);
	}

	printf("Fluuuuf B\n");

/*	// check that trajectories are within limits
	QVector<ControlPoint>::iterator i;
	for ( i = graspTrajectory.begin(); i != graspTrajectory.end(); ++i )
	{
		printf("Fluuuuf \n");

		for ( int j = 0; j < 9; j++ )
		{
			printf("Fluuuuf j:%d \tp: %f\t", j, i->p[j]);
			printf(
			if ( i->p[j] < limits[j].min))
			{
				printf("WARNING: position not within limits, min:%f\tmax:%f\n", limits[j].min, limits[j].max );
				//QString err = "Joint "; err.append(j); err.append(" goes out of range.");
				//throw err;
				i->p[j] = limits[j].min + 0.1;
				printf("resetting position to %d\n", i->p[j] );

			}
			if ( i->p[j] > limits[j].max ) {
				printf("WARNING: position not within limits, min:%f\tmax:%f\n", limits[j].min, limits[j].max );
				i->p[j] = limits[j].max - 0.1;
				printf("resetting position to %d\n", i->p[j] );
			}
		}
	}
*/
	
	printf("Fluuuuf C\n");


/*
	// init alex's touch stuff
	QString remoteTouchPort;
	QString touchPartName;
	std::string strippedRobotName;
	if ( robotName == "icubSim" || robotName == "icubSimF" )
	{
		if ( partName == "left_arm" ) touchPartName = "left_hand";
		else if ( partName == "right_arm" ) touchPartName = "right_hand";
		else throw "Not connected to an arm";

		strippedRobotName = "icubSim";
	}
	else if ( robotName == "icub" || robotName == "icubF" )
	{
		if ( partName == "left_arm" ) touchPartName = "lefthand";
		else if ( partName == "right_arm" ) touchPartName = "righthand";
		else throw "Not connected to an arm";

		strippedRobotName = "icub";
	}
	else throw "Not connected to an iCub";
	
	remoteTouchPort = "/";
	remoteTouchPort.append(strippedRobotName.c_str());
	remoteTouchPort += "/skin/" + touchPartName;

	ts = new TouchSenseThread( remoteTouchPort.toStdString(), "/touchsensor" );
	
	if (!ts->start())
	{
		QString err = "Cannot start touch sensor thread";
		throw err;
	};

*/

	printf("HandController started successfully.\n");
}

void HandController::checkValidity()
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

void HandController::stop()
{
	ts->stop();

	checkValidity();
	
	/* warning: this for() loop kills the motors: */
	for (int i = 7; i < 16; i++) {
	    if (amp) { amp->disableAmp(i); }
	    if (pid) { pid->disablePid(i); }
	}
	/* this just closes the driver: OK */
	if (dd)
	{ 
		dd->close();
		delete dd;
	}
}

void HandController::preGrasp()
{
	checkValidity();
	blockingPositionMove(graspTrajectory.begin());
	tgt++;
	printf("Reached pregrasp pose...\n");
}

void HandController::blockingPositionMove( ControlPoint* target )
{
	bool ok;
	for (int i = 0; i < 9; i++ )
	{
		ok = pos->positionMove( i+7, target->p[i] );
		if ( !ok ) { throw QString("Position move failed"); }
	}
	
	bool finished = false;
	while ( !finished )
	{
		pos->checkMotionDone(&finished);
	}
}

void HandController::grasp( int speed )
{
	qreal currentAttenuation;
	checkValidity();
	while ( true )
	{
		// update booleans for touch sensors
		palmTouch = ts->getPalmTouch();
		thumbTouch = ts->getThumbTouch(); 
		indexTouch = ts->getIndexTouch();
		middleTouch = ts->getMiddleTouch();
		ringTouch = ts->getRingTouch();
		littleTouch = ts->getLittleTouch();
		
		currentAttenuation = doVelocityControl( speed );
		msleep(20);
		
		// move through the vector of control points
		if ( currentAttenuation <= attenuationThreshold )
		{
			if ( tgt != graspTrajectory.end() - 1 ) 
			{
				printf("GOTO NEXT TARGET\n");
				tgt++;
			}
			else if ( currentAttenuation <= attenuationThreshold/10 )
			{
				// stop everything
				printf("STOP\n");
				for ( i = 0; i < 9; i++ )
				{
					vel->velocityMove( i+7, 0 );
				}
				break;
			}
		}
	}
}

void HandController::unGrasp( int speed )
{
	qreal currentAttenuation;
	checkValidity();
	while ( true )
	{
		currentAttenuation = doVelocityControl( speed, false );
		msleep(20);
		
		// move through the vector of control points
		if ( currentAttenuation <= attenuationThreshold )
		{
			if ( tgt != graspTrajectory.begin() ) 
			{
				printf("GOTO NEXT TARGET\n");
				tgt--;
			}
			else if ( currentAttenuation <= attenuationThreshold/10 )
			{
				// stop everything
				printf("STOP\n");
				for ( i = 0; i < 9; i++ )
				{
					vel->velocityMove( i+7, 0 );
				}
				break;
			}
		}
	}
}

qreal HandController::doVelocityControl( int speed, bool stopOnTouch )
{
	// get encoder positions and tabulate error
	max = 0.0;
	//qreal meanErr = 0.0;
	enc->getEncoders(q);
	printf("Position Error:\t");
	for ( i = 0; i < 9; i++ )
	{
		err[i] = tgt->p[i] - q[i+7];
		ERR[i] = qAbs( err[i] );
		//meanErr += ERR[i];
		if ( ERR[i] > max ) { max = ERR[i]; }
		printf( "%f\t", err[i] );
	}
	if ( qFuzzyIsNull(max) ) { return 0; }
	//meanErr /= 9.0;
	
	// stop digits that have touched something
	if ( stopOnTouch )
	{
		if ( thumbTouch || indexTouch || middleTouch || ringTouch || littleTouch ) { ERR[7] = 0; }
		if ( thumbTouch ) { ERR[8] = 0; ERR[9] = 0; ERR[10] = 0; }
		if ( indexTouch ) { ERR[11] = 0; ERR[12] = 0; }
		if ( middleTouch ) { ERR[13] = 0; ERR[14] = 0; }
		if ( littleTouch || ringTouch) { ERR[15] = 0; }
	}
	
	// do feedback velocity control
	qreal meanAtt = 0.0;
	//printf("Velocity Move:\t");
	for ( i = 0; i < 9; i++ )
	{
		t = ERR[i]/attenuationFactor;
		att = -exp(-t)*(1+t)+1;
		cmd = att * speed * err[i]/max;
		vel->velocityMove(i+7,cmd);
		//printf( "%f\t", cmd );
		meanAtt += att;
	}
	meanAtt /= 9.0;
	
	//printf(" meanCmd = %f", mean );
	printf("\n");
	return meanAtt;
}
