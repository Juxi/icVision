#include "partController.h"
#include <time.h>


PartController::PartController() : numJoints(0)
{
}
PartController::~PartController()
{
	printf("Deleting %s %s\n", robotName.toStdString().c_str(), partName.toStdString().c_str());
	close();
}

bool PartController::isValid()
{
	if ( !dd ) { return 0; }
	else if ( !dd->isValid() || !pos || !vel || !enc || !pid || !amp || !lim ) { return 0; }
	else { return 1; }
}

bool PartController::open( const char* _robotName, const char* _partName )
{
	robotName = _robotName;
	partName = _partName;
	return open();
}

bool PartController::open()
{
	printf("CONFIGURING: %s %s\n", robotName.toStdString().c_str(), partName.toStdString().c_str());
	
	if ( !network.checkNetwork() )
	{
		printf("Cannot find YARP network.\n");
		return 0;
	}
	
	yarp::os::Property options;
	options.put( "robot", robotName.toStdString().c_str() ); // typically from the command line.
	options.put( "device", "remote_controlboard" );
	
	QString localPort = "/" + robotName + "/" + partName + "/control";
	options.put("local", localPort.toStdString().c_str());
	
	QString remotePort = "/" + robotName + "/" + partName;
	options.put("remote", remotePort.toStdString().c_str());
	
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
	
	int i;
	for (i = 0; i < numJoints; i++)
	{
		pos->setRefAcceleration(i, 20);
		amp->enableAmp(i);
		pid->enablePid(i);
		
	}
	
	/* initialize random seed: */
	srand ( time(NULL) );

	return 1;
}

bool PartController::checkMotionDone( bool* flag )
{
	if ( !pos ) { return false; }
	else { return pos->checkMotionDone(flag); }
}

bool PartController::setRefSpeeds( const qreal speed )
{
	qreal *speeds;
	speeds = new qreal[numJoints];
	
	for ( int i=0; i<numJoints; i++ ) { speeds[i] = speed; }
	bool result = pos->setRefSpeeds(speeds);
	delete[] speeds;
	
	return result;
}

bool PartController::positionMove( const QVector<qreal>& positions )
{
	if ( positions.size() == numJoints ) { return pos->positionMove(positions.constData()); }
	else { return 0; }
}

bool PartController::randomPosMove( qreal maxSpeed, bool hands )
{
	qreal min,max,aSpeed,aPos;
	QVector<qreal> pose,speed;
	for (int i = 0; i < numJoints; i++)
	{
		if ( i < 7 || hands )
		{
			lim->getLimits(i, &min, &max);
			aPos = min + (max-min) * (qreal)rand()/RAND_MAX;
		} else { aPos = 0; }

		aSpeed = maxSpeed*(qreal)rand()/RAND_MAX;
		speed.append( aSpeed );
		pose.append( aPos );
		printf("%f ",aPos);
			
	}
	//printf("\n");
	
	if (setRefSpeeds(speed)) 
	{
		return positionMove(pose);
	}
	else
	{
		return 0;
	}
}

bool PartController::velocityMove( int i, qreal v )
{
	if ( i<numJoints ) { return vel->velocityMove(i,v); }
	else { return 0; }
}


//void PartController::getPositions( QVector<qreal>&pos ) {
////	QVector<qreal> pos;
//	pos.resize(numJoints);
//	
//	double val;
//	for (int i = 0; i < numJoints; i++)
//	{
//		enc->getEncoder(i, &val);
//		pos.append( val );
//	}	
//
//	//	return pos;
//}

void PartController::close()
{
	/* warning: this for() loop kills the motors: */
	for (int i = 0; i < numJoints; i++) {
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