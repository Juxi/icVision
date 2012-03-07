/*******************************************************************
 ***               Copyright (C) 2012 Juxi Leitner               ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup katanaCtrl
 *	@{
 */

#ifndef JOINTCONTROLLER_H
#define JOINTCONTROLLER_H

#define KATANA_JOINT_NR	6

#include <vector>
#include <string>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

class CtrlPoint {
public:
	double pos[KATANA_JOINT_NR];
};

class JointController {
	
public:
	
	JointController(  );
    ~JointController();

	void loadPositions( const char* fileName );			//!< Read CSV file
	void start( const char* _robotName, const char* _partName );	//!< Connects to the remote device
	void checkValidity();																			//!< Checks if the remote device is ready and the interface is working
	void stop();																					//!< Closes the connection to the remote device
	
	void goToNext( );
	/* need a function to make the trajectory from a config file */
	unsigned int nextTarget;

	yarp::os::Bottle* getEncodersAsBottle();
	
private:
	std::string robotName;
	std::string partName;
	
	std::vector<CtrlPoint*> storedTargets;
	
	
	int i;
	double q[16];		//encoder positions
	
	yarp::os::Network network;


	yarp::os::RpcClient katanaPort;

	yarp::dev::PolyDriver *dd;
//	yarp::dev::IPositionControlRaw *posRaw;
	yarp::dev::IPositionControl *pos;	
	yarp::dev::IVelocityControl *vel;
	yarp::dev::IEncoders *enc;
	yarp::dev::IPidControl *pid;
//	yarp::dev::IAmplifierControl *amp;
//	yarp::dev::IControlLimits *lim;	

	void blockingPositionMove( double * );
	
};
#endif
/** @} */
