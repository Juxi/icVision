/*******************************************************************
 ***      Copyright (C) 2011 Mikhail Frank, Juxi Leitner         ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup iCubBabble
 *	@{
 */

#ifndef HEADCONTROLLER_H
#define HEADCONTROLLER_H

#include <QtCore>
#include <QThread>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#define NUMBEROFHEADJOINTS 6

class HeadController : public QThread {
	
public:
	
	HeadController( qreal _errorTolerance = 0.3, qreal _attenuationFactor = 3.0 );
    ~HeadController();
	
	void connect( const char* _robotName, const char* _partName, int refAcceleration, int refSpeed );	//!< Connects to the remote device
	void checkValidity();																			//!< Checks if the remote device is ready and the interface is working
	void stop();																					//!< Closes the connection to the remote device
	
	void addControlPoint(float a[NUMBEROFHEADJOINTS]);		
	void initialPose();

	/* need a function to make the trajectory from a config file */
	
	void run();	
	
private:
	qreal attenuationThreshold;
	qreal attenuationFactor;

	QString robotName;
	QString partName;
	
	const int number_of_joints;	
	
	int i;
	double q[NUMBEROFHEADJOINTS];		//encoder positions
	
	qreal err[NUMBEROFHEADJOINTS];	// position error
	qreal ERR[NUMBEROFHEADJOINTS];	// absolute value of error
	qreal max;		// maximum error
	qreal cmd;		// velocity control signal
	qreal t;		// the independant variable of the attenuation function
	qreal att;		// attenuator value
	
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
		qreal p[NUMBEROFHEADJOINTS];
	};
	QVector<ControlPoint> trajectory;
	QVector<ControlPoint>::iterator tgt;
	
	struct Interval
	{
		qreal min;
		qreal max;
	};
	Interval limits[NUMBEROFHEADJOINTS];
	
	void blockingPositionMove( ControlPoint* );
	qreal doVelocityControl( int speed, bool stopOnTouch = true );
	
};
#endif
/** @} */
