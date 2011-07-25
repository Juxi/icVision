/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef REFLEXDETECTOR_H
#define REFLEXDETECTOR_H

#include "yarpStreamPort.h"
#include "worldRpcInterface.h"
#include "model.h"

namespace VirtualSkin
{
	class YarpModel;
}

/*! \brief Wraps RobotModel::Model with YARP functionality
 *
 * This class allows the results of collision detection to be published via ethernet and provides an RPC interface to the 
 * robot's environment (RobotModel::World) such that geometries can be added/updated/and removed remotely.
 *
 * To use this class:
 *   If you want to publish collision information on the network via YARP, call openCollisionPort(QString&).
 *	 To start the RPC interface to the RobotModel::World, call openWorldRpcPort(QString&).
 */
class VirtualSkin::YarpModel : public KinematicModel::Model
{

public:
	YarpModel( bool visualize = false );	//!< Connects the WorldRpcInterface to the RobotModel::World and calls the super-class constructor	
	~YarpModel();							//!< Just stops the YARP port if its running

	//void openCollisionPort( const QString& name ) { collisionPort.open(name); }	//!< Starts a YARP Port that streams the results of collision detection (see \ref yarpPorts)
	//void closeCollisionPort() { collisionPort.close(); }						//!< Close the port that streams collision information
	
	//void openWorldRpcPort( const QString& name ) { worldPort.open(name); }		//!< Start a YARP port that provides an RPC interface to the RobotModel::Model
	//void closeWorldRpcPort() { worldPort.close(); }								//!< Closes the RPC interface to the RobotModel::Model
	
	// open marker port
	// close marker port
	
	virtual void computePosePrefix();											//!< Clears the yarp::os::bottle bottle
	virtual void computePoseSuffix();											//!< Publishes the yarp::os::bottle bottle
	virtual void collisionHandlerAddendum( KinematicModel::PrimitiveObject*,
										   KinematicModel::PrimitiveObject*,
										   const DT_CollData* );					//!< This puts the results of collision detection into the yarp::os::bottle bottle
	
private:

	//QString				cPortName,		//!< The name of the YARP stream port that broadcasts the collision information
	//					wPortName;		//!< The name of the YARP stream port that provides the RPC interface to the RobotModel::World
	//YarpStreamPort		collisionPort;	//!< The YARP stream port that broadcasts the collision information
	//WorldRpcInterface	worldPort;		//!< The YARP RPC port that provides an interface to the RobotModel::World
	yarp::os::Bottle	bottle;			//!< The bottle containing the collision information, that is streamed by collisionPort
};
#endif
/** @} */