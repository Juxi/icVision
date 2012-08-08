/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef REFLEXDETECTOR_H
#define REFLEXDETECTOR_H

//#include "yarpStreamPort.h"
#include "worldRpcInterface.h"
#include "model.h"

namespace VirtualSkin
{
	class YarpModel;
	class YarpRobot;
	class WorldRpcInterface;
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

	YarpRobot* loadYarpRobot( const QString& fileName, bool verbose );
	
	void openWorldRpcPort( const QString& name ) { worldRpcInterface.open(name); }		//!< Start a YARP port that provides an RPC interface to the RobotModel::Model
	void closeWorldRpcPort() { worldRpcInterface.close(); }												//!< Closes the RPC interface to the RobotModel::Model
	//bool WorldRpcPortIsOpen() { return rpcIsOpen; }
	
	void showBottle( yarp::os::Bottle& anUnknownBottle, int indentation = 0 );			//!< Print a yarp::os::bottle to the terminal
	WorldRpcInterface& getRpcInterface() { return worldRpcInterface; }

private:
	
	void computePosePrefix();											//!< Clears the yarp::os::bottle bottle
	void computePoseSuffix();											//!< Publishes the yarp::os::bottle bottle
	void collisionHandlerAddendum( KinematicModel::PrimitiveObject*,
								   KinematicModel::PrimitiveObject*,
								   const DT_CollData* );				//!< This puts the results of collision detection into the yarp::os::bottle bottle

	//yarp::os::Network	yarp;				//!< Identifies the yarp network
	//yarp::os::RpcServer	worldPort;
	//WorldRpcReader		rpcReader;
	//bool				rpcIsOpen;
	
	//yarp::os::Bottle	response;
	
	
	WorldRpcInterface	worldRpcInterface;		//!< The YARP RPC port that provides an interface to the RobotModel::World
	
};
#endif
/** @} */
