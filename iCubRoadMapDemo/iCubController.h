/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup iCubRoadMap
 *	@{
 */

#ifndef ICUBCONTROLLER_H
#define ICUBCONTROLLER_H

#include "partController.h"
#include <QObject>


/*! \brief Wraps YARP's remote device driver interface neatly
 *  This could be a motor control interface for one part of the iCub for example. see: http://eris.liralab.it/wiki/Motor_control
 */

class iCubController: public QObject
{
	Q_OBJECT
	
public:
    iCubController();	//!< Nothing special
    ~iCubController();	//!< Nothing special
	
	bool open( const char* _robotName );	//!< Connects to the remote device
	void close();							//!< Closes the connection to the remote device
	
	bool isValid();							//!< Checks if the remote device is ready and the interface is working
	int	getNumJoints();						//!< Returns the number of controllable axes
	bool checkMotionDone( bool* );			//!< Checks if a position move has finished
	bool setWaypoint();
	
	bool stop();							//!< Stops all joints immediately
	
	std::vector<double> withinLimits( const std::vector<double>& poss );
	bool positionMove( std::vector<double> );		//!< Moves the device to a specified position
	bool setJointMask( std::vector<bool> );
	void setVelocity( int );
	
public slots:
	std::vector<double> getRandomPose();
	std::vector<double> getCurrentPose();
	
signals:
	void newSample( std::vector<double> );
	
private:

	int numJoints;
	PartController torso, left_arm, right_arm;
	
	yarp::os::Network yarp;
	yarp::os::Port filterRpc;
	
	template <class T>
	bool chop( const std::vector<T>& data,
					 std::vector<T>& torsoData, 
					 std::vector<T>& left_armData, 
					 std::vector<T>& right_armData  )
	{ 
		if ( data.size() != (unsigned int)numJoints ) { return 0; }
		std::vector<T> a, b, c;
		for ( int i=0; (unsigned int)i<data.size(); i++ )
		{
			if ( i < torso.getNumJoints() ) { a.push_back( data.at(i) ); }
			else if ( i < torso.getNumJoints() + left_arm.getNumJoints() ) { b.push_back( data.at(i) ); }
			else if ( i < torso.getNumJoints() + left_arm.getNumJoints() + right_arm.getNumJoints() ) { c.push_back( data.at(i) ); }
		}
		torsoData = a;
		left_armData = b;
		right_armData = c;
		return 1;
	}
};
#endif
/** @} */
