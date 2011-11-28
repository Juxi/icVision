/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup iCubBabble
 *	@{
 */

#ifndef PARTCONTROLLER_H
#define PARTCONTROLLER_H

#include <QtCore>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>


/*! \brief Wraps YARP's remote device driver interface neatly
 *  This could be a motor control interface for one part of the iCub for example. see: http://eris.liralab.it/wiki/Motor_control
 */

class PartController
{
public:
    PartController();	//!< Nothing special
    ~PartController();	//!< Nothing special
	
	bool open( const char* _robotName, const char* _partName );	//!< Connects to the remote device
	void close();												//!< Closes the connection to the remote device
	
	bool isValid();												//!< Checks if the remote device is ready and the interface is working
	int	getNumJoints() { return numJoints; }					//!< Returns the number of controllable axes
	bool checkMotionDone( bool* );								//!< Checks if a position move has finished
	
	bool stop() { return vel->stop(); }							//!< Stops all joints immediately
	bool setRefSpeeds( const qreal );							//!< Sets the reference velocity for position move commands
	bool setRefSpeeds( const QVector<qreal>& speeds) { return pos->setRefSpeeds(speeds.constData()); }	//!< Sets the reference velocity for position move commands
	bool positionMove( const QVector<qreal>& );					//!< Moves the device to a specified position
	bool velocityMove( int i, qreal v);							//!< Move the robot (indefinitely) with a specified velocity (you should stop it eventually)
	bool randomPosMove( qreal maxSpeed, bool hands );			//!< do a random position move with all controllable axes

private:
	
	bool open();
	
	QString robotName;
	QString partName;
	int numJoints;
	
	yarp::os::Network network;
	yarp::dev::PolyDriver *dd;
	yarp::dev::IPositionControl *pos;
	yarp::dev::IVelocityControl *vel;
	yarp::dev::IEncoders *enc;
	yarp::dev::IPidControl *pid;
	yarp::dev::IAmplifierControl *amp;
	yarp::dev::IControlLimits *lim;
};
#endif
/** @} */