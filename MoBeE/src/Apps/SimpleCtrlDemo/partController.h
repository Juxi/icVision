/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef PARTCONTROLLER_H
#define PARTCONTROLLER_H

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>

#define VOCAB_QATTR VOCAB4('q','a','t','t')
#define VOCAB_QFORCE VOCAB4('q','f','o','r')

/*! \brief Wraps YARP's remote device driver interface neatly
 *  This could be a motor control interface for one part of the iCub for example. see: http://eris.liralab.it/wiki/Motor_control
 */

class PartController : public yarp::os::RateThread
{
	
public:
	
    PartController( const char* _robotName, const char* _partName, int r );
    virtual ~PartController();

	bool    isValid();								//!< Checks if the remote device is ready
	int     getNumJoints() { return numJoints; }	//!< Returns the number of controllable axes
    bool    getEncoders( double* q ) { return enc->getEncoders(q); }
    
    void setAttractorPosition(yarp::os::Bottle*);
    void setConstForce(yarp::os::Bottle*);
    
private:
	
	yarp::os::Network							network;
	yarp::os::BufferedPort<yarp::os::Bottle>	port;
	yarp::dev::PolyDriver						*dd;
	yarp::dev::IVelocityControl					*vel;
	yarp::dev::IEncoders						*enc;
    
	int					numJoints;	// number of controllable DOFs
	double				*q1,		// current pose
                        *q0,		// last pose
                        *v,			// velocity q1-q0
                        *e,			// the error vector x-q1
    
                        *x,			// the attractor pose
                        *min,		// min joint positions
                        *max,		// max joint positions
                        *nogo,      // length (in degrees) of limit avoidance spring
                        
						*w,			// weight vector indicates importance of DOFs
						*k,			// spring constants for each DOF
						*c,			// damping constants for each DOF
                        *a,			// the acceleration currently felt by the robot
                        *ctrl,		// the next velocity control command
                        
                        *fX,         // attractor spring force
                        *fXMax,      // maximum attractor spring force
                        
                        *fLim,         // joint limit repulsion
                        *fLimMax,      // maximum repulsive force
                        
                        *fRPC,         // user defined force (from RPC)
                        *kfRPC,
                        *fRPCMax;
    
protected:
    
    virtual bool threadInit();
	virtual void afterStart(bool s);
    virtual void run();
    virtual void threadRelease();
    
    double magnitude(yarp::os::Bottle*);
	
};
#endif
/** @} */
