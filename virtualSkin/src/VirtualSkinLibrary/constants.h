/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

/**
 *	Approximate time between stream packets from robot... 
 */
#define YARP_PERIOD 0.02					//!< seconds
#define YARP_PERIOD_ms YARP_PERIOD*1000		//!< milliseconds
#define YARP_PERIOD_us YARP_PERIOD_ms*1000	//!< microseconds

/**
 *	For the reflex behavior
 */
#define POSE_BUFFER_SIZE 100		//!< Length of the pose history stored by VirtualSkin::StateObserver
#define REFLEX_SPEED 30				//!< The velovity to use for the reflexive position move command
#define POSITION_MOVE_TIMEOUT 10	//!< Max time (seconds) to wait for the robot to finish the reflexive position move
#define NEGLIGIBLE_ANGLE 5			//!< Tolerance around target position (degrees) for reflexive position move command

/** @} */