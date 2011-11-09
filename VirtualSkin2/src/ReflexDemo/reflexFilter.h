/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup ReflexDemo
 *	@{
 */

#ifndef REFLEXFILTER_H_
#define REFLEXFILTER_H_

#include <time.h>

#include "robotFilter.h"
#include "circularBuffer.h"
#include "filterRpcInterface.h"

/** \brief A VirtualSkin::RobotFilter with reflexive collision response
 *
 * This Filter forwards commands from the client (a control program) until the internal robot model detects a dangerous configuration.
 * At that time, the client is cut off from the robot, the Filter moves the robot back to a "safe" configuration, and then control is returned to the client program.
 */
class ReflexFilter : public VirtualSkin::RobotFilter
{

public:
	
	ReflexFilter( KinematicModel::Robot* r, bool visualize = true );		//!< Nothing special to do here
	virtual ~ReflexFilter();					//!< Nothing special to do here

	virtual void extraOpenStuff();				//!< Resizes the QVectors according to the robot model
	//virtual void stopRobot();			//!< Provides reflexive response to collision
												/**< Moves the robot back to a past "safe" state */
	virtual void collisionResponse();			//!< Waits for the commands from stopRobot() to finish
												/**< This example is straightforward, as stopRobot()
													 issues a position move command */

	//void openFilterRpcPort( const QString& name ) { filterRpcInterface.open(name.toStdString().c_str()); }		//!< Start a YARP port that provides an RPC interface to the RobotModel::Model
	//void closeFilterRpcPort() { filterRpcInterface.close(); }												//!< Closes the RPC interface to the RobotModel::Model
	
	//void setWaypoint();
private:
	
	QVector< QVector<qreal> >	originalPose,	//!< A colliding configuration
								targetPose;		//!< A safe configuration
	QVector< QVector< VirtualSkin::CircularBuffer::Item > > history;	//!< The history of the last n poses of the whole robot
};
#endif
/** @} */
