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

/** \brief A MoBeE::RobotFilter with reflexive collision response
 *
 * This Filter forwards commands from the client (a control program) until the internal robot model detects a dangerous configuration.
 * At that time, the client is cut off from the robot, the Filter moves the robot back to a "safe" configuration, and then control is returned to the client program.
 */
class ReflexFilter : public MoBeE::RobotFilter
{

public:
	
	ReflexFilter( KinematicModel::Robot* r, bool visualize = true );	//!< Nothing special to do here
	virtual ~ReflexFilter();											//!< Nothing special to do here
	virtual void collisionResponse();									//!< Waits for the commands from stopRobot() to finish
																		/**< This example is straightforward, as stopRobot()
																				issues a position move command */
	bool stillMoving();
};
#endif
/** @} */
