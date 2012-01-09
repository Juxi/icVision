/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef YARPROBOT_H
#define YARPROBOT_H

#include <yarp/os/all.h>
#include "robot.h"


namespace VirtualSkin
{ 
	//class Model;
	class YarpRobot;
}
	
/*! \brief A kinematic model of a robot.
 *
 * The robot model consists of a tree of KinTreeNode objects of type RevoluteJoint, PrismaticJoint and Link, as well as a list of
 * Motor objects and a list of BodyPart objects. A Motor is bound to one or more joints and control the joints' position
 * according to a linear mapping. A BodyPart is a groups of motors, the positions of which can be set all at once. 
 */
class VirtualSkin::YarpRobot : public KinematicModel::Robot
{

public:
	
	YarpRobot( KinematicModel::Model* m, DT_RespTableHandle t );	//!< 
	virtual ~YarpRobot();											//!< Nothing to do here
	
	void openCollisionPort( const QString& name ) { collisionPort.open(name.toStdString().c_str()); }	//!< Starts a YARP Port that streams the results of collision detection (see \ref yarpPorts)
	void closeCollisionPort() { collisionPort.close(); }												//!< Close the port that streams collision information
	
	void openObservationPort( const QString& name ) { observationPort.open(name.toStdString().c_str()); }	//!< 
	void closeObservationPort() { observationPort.close(); }	
	
	void addCollisionData( int, double, double, double, int, double, double, double );
	void addCollisionData( int, double, double, double, char* );
	
	void clearStateBottles();
	void publishState();

private:
	
	yarp::os::Port		collisionPort;		//!< 
	yarp::os::Port		observationPort;
	yarp::os::Bottle	collisionBottle;	//!< The bottle containing the collision information, that is streamed by collisionPort
	yarp::os::Bottle	observationBottle;
	
	//void processState();
	
};

#endif

/** @} */
