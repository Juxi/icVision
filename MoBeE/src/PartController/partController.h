/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup PartController
 *	@{
 */

#ifndef PARTCONTROLLER_H
#define PARTCONTROLLER_H

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

/*! \brief Wraps YARP's remote device driver interface neatly
 *  This could be a motor control interface for one part of the iCub for example. see: http://eris.liralab.it/wiki/Motor_control
 */

class PartController : public yarp::os::RateThread
{
	
public:
	
    PartController( const char* _robotName, const char* _partName, int r );
    virtual ~PartController();

	bool            isValid();								//!< Checks if the remote device is ready
	int             getNumJoints() { return numJoints; }	//!< Returns the number of controllable axes
    virtual bool    getEncoders( double* q ) { return enc->getEncoders(q); }
    
	//bool stop(); //!< Stops all joints immediately
	//bool isWithinLimits( const std::vector<double>& pose );
	//std::vector<double> withinLimits( const std::vector<double>& pose );
	//std::vector<double> getRandomPose();
	//std::vector<double> getCurrentPose();
	//void setRefVelocity( int );
	//void setRefAcceleration( int );
	
protected:
    
    bool set( double **var, yarp::os::Bottle* list );
	
	yarp::os::Network							network;
	yarp::os::BufferedPort<yarp::os::Bottle>	port;
	yarp::dev::PolyDriver						*dd;
	yarp::dev::IVelocityControl					*vel;
	yarp::dev::IEncoders						*enc;
    
    virtual bool threadInit();
	virtual void afterStart(bool s);
    virtual void run();
    virtual void threadRelease();
    
private:
	
	
	
	

	int					numJoints;	// number of controllable DOFs
	double				*x,			// the attractor pose
						*w,			// weight vector indicates importance of DOFs
						*k,			// spring constants for each DOF
						*c,			// damping constants for each DOF
						*min,		// min joint positions
						*max,		// max joint positions
						*q1,		// current pose
						*q0,		// last pose
						*v,			// velocity q1-q0
						*e,			// the error vector x-q1
						*a,			// the acceleration currently felt by the robot
						*cmd;		// the next velocity control command	
	
	
};
#endif
/** @} */
