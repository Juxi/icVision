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
	virtual void collisionResponse();			//!< Provides reflexive response to collision
												/**< Moves the robot back to a past "safe" state */
	virtual void responseComplete();			//!< Waits for the commands from collisionResponse() to finish
												/**< This example is straightforward, as collisionResponse()
													 issues a position move command */

private:
	
	QVector< QVector<qreal> >	originalPose,	//!< A colliding configuration
								targetPose;		//!< A safe configuration
	QVector< QVector< QVector<qreal> > > history;	//!< The history of the last n poses of the whole robot

};
#endif
/** @} */
