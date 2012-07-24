/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup iCubRoadMap
 *	@{
 */

#ifndef PARTCONTROLLER_H
#define PARTCONTROLLER_H

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <string>
#include <vector>


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
	
	bool stop();												//!< Stops all joints immediately
	
	bool isWithinLimits( const std::vector<double>& poss );
	std::vector<double>  withinLimits( const std::vector<double>& poss );
	bool positionMove( const std::vector<double>& poss );
	bool velocityMove( const std::vector<double>& v );
	//bool positionMove( const std::vector<double>& poss, const std::vector<double>& vels );		//!< Moves the device to a specified position

	bool setJointMask( const std::vector<bool>& vals );
	std::vector<double> getRandomPose();
	std::vector<double> getCurrentPose();
	
	void setRefVelocity( int );
	void setRefAcceleration( int );
	
	
private:

	int numJoints;
	std::vector<double> min,max;
	std::vector<bool> jointMask;
	
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
