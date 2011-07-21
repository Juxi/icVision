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
#define POSE_BUFFER_SIZE 100				//!< Length of the pose history stored by VirtualSkin::StateObserver
#define ALL_CLEAR_WAIT 500					//!< Time to wait (milliseconds) for control to be restored after the robot reaches a safe configuration (this creates some buffer zone)
//#define REFLEX_SPEED 30					//!< The velovity to use for the reflexive position move command
//#define POSITION_MOVE_TIMEOUT 10			//!< Max time (seconds) to wait for the robot to finish the reflexive position move
//#define NEGLIGIBLE_ANGLE .5				//!< (degrees)
//#define NEGLIGIBLE_FRACTION 0.05
//#define NEGLIGIBLE_PERCENT 100*NEGLIGIBLE_FRACTION


/**
 *	VOCABS
 */
// commands for objects
#define VOCAB_MK VOCAB2('m','k')			//!< Make a new object
#define VOCAB_SET VOCAB3('s','e','t')		//!< Set the position of an object
#define VOCAB_DEF VOCAB3('d','e','f')
#define VOCAB_GET VOCAB3('g','e','t')		//!< Returns the 4x4 rototranslation matrix for an object
#define VOCAB_ROT VOCAB3('r','o','t')		//!< Rotate an object
#define VOCAB_REM VOCAB2('r','m')			//!< Remove an object
#define VOCAB_LS VOCAB2('l','s')			//!< list the objects in the world

// object types
#define VOCAB_OBSTACLE VOCAB3('o','b','s')	//!< an obstacle that triggers reflexive response 
#define VOCAB_TARGET VOCAB3('t','g','t')
//#define VOCAB_BODY_PART VOCAB3('b','d','y')
//#define VOCAB_NO_TYPE VOCAB2('g','h','o')	//!< gho for ghost

// commands for primitives
#define VOCAB_APP VOCAB3('a','p','p')		//!< Append a primitive to an object
#define VOCAB_PSET VOCAB4('s','e','t','p')	//!< Set the position of a primitive relative to its parent object
#define VOCAB_PROT VOCAB4('r','o','t','p')	//!< Rotate a primitive relative to its parent object
#define VOCAB_PREM VOCAB3('r','m','p')		//!< Remove a primitive from its parent object

// geometries
#define VOCAB_CSG VOCAB3('c','s','g')		//!< Constructive Solid Geometry object
#define VOCAB_SPH VOCAB3('s','p','h')		//!< Sphere
#define VOCAB_CYL VOCAB3('c','y','l')		//!< Cylinder
#define VOCAB_BOX VOCAB3('b','o','x')		//!< Box

// these are just for compatibility with the iCubSimulator
#define VOCAB_SSPH VOCAB4('s','s','p','h')	//!< Static Sphere (exactly the same as a sphere but with a different naming scheme)
#define VOCAB_SCYL VOCAB4('s','c','y','l')	//!< Static Cylinder (exactly the same as a cylinder but with a different naming scheme)
#define VOCAB_SBOX VOCAB4('s','b','o','x')	//!< Static Box (exactly the same as a box but with a different naming scheme)

// admin
#define VOCAB_CLEAR VOCAB3('c','l','r')		//!< Clears the world model and resets all counters

/** @} */
