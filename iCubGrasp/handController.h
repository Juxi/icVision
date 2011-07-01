/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup iCubBabble
 *	@{
 */

#ifndef HANDCONTROLLER_H
#define HANDCONTROLLER_H

#include <QtCore>
#include <QThread>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

class HandController : private QThread
{
	
public:
	
	HandController( qreal _errorTolerance = 0.3, qreal _attenuationFactor = 3.0 );
    ~HandController();
	
	void start( const char* _robotName, const char* _partName, int refAcceleration, int refSpeed );	//!< Connects to the remote device
	void checkValidity();																			//!< Checks if the remote device is ready and the interface is working
	void stop();																					//!< Closes the connection to the remote device
	
	void preGrasp();
	void grasp( int speed );
	void unGrasp( int speed );
	/* need a function to make the trajectory from a config file */
	
private:
	
	qreal errorTolerance;
	qreal attenuationFactor;

	QString robotName;
	QString partName;
	
	int i;
	double q[16];		//encoder positions
	
	qreal err[9];	// position error
	qreal ERR[9];	// absolute value of error
	qreal max;		// maximum error
	qreal cmd;		// velocity control signal
	qreal n;		// normalized error
	qreal t;		// the independant variable of the attenuation function
	qreal att;		// evaluated attenuator
	//qreal mean;		// mean attenuation
	
	yarp::os::Network network;
	yarp::dev::PolyDriver *dd;
	yarp::dev::IPositionControl *pos;
	yarp::dev::IVelocityControl *vel;
	yarp::dev::IEncoders *enc;
	yarp::dev::IPidControl *pid;
	yarp::dev::IAmplifierControl *amp;
	yarp::dev::IControlLimits *lim;
	
	struct ControlPoint
	{
		qreal t;
		qreal p[9];
	};
	QVector<ControlPoint> graspTrajectory;
	QVector<ControlPoint>::iterator tgt;
	
	struct Interval
	{
		qreal min;
		qreal max;
	};
	Interval limits[9];
	
	bool palmTouch;
	bool thumbTouch;
	bool indexTouch;
	bool middleTouch;
	bool littleTouch;
	
	void blockingPositionMove( ControlPoint* );
	qreal doVelocityControl( int speed );
	
};
#endif
/** @} */