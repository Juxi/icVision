/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef WORLDRPCINTERFACE_H
#define WORLDRPCINTERFACE_H

#include <QVector3D>
#include <QMatrix4x4>
#include <yarp/os/all.h>
#include "yarpRpcPort.h"

namespace RobotModel
{
	class World;
	class CompositeObject;
	class PrimitiveObject;
}

namespace VirtualSkin
{
	class WorldRpcInterface;
}
	
/** \brief Provides an RPC interface (via YARP) to RobotModel::World
 *
 *	This interface is compatible with that of the iCub simulator in terms of the syntax of RPC commands, the kinds of geometries supported and the
 *  way those geometries are named.  However in addition to individual primitives, the World supports CompositeObjects, made up of unions of PrimitiveObjects.
 */
class VirtualSkin::WorldRpcInterface: public VirtualSkin::YarpRpcPort
{
		
public:

	WorldRpcInterface();			//!< Nothing special to do here
	virtual ~WorldRpcInterface();	//!< Nothing special to do here
	
	void setWorld( RobotModel::World* aWorld ) { world = aWorld; }	//!< Sets the RobotModel::World on which this interface operates

private:
	virtual bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );	//!< Handles incomming RPC commands (expressed as YARP bottles)
	
	void make( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Creates a new object
	void setPos( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Sets the position of an object
	void pSetPos( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Set the position of a primitive relative to its parent object
	void setRot( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Rotate an object
	void pSetRot( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Rotate a primitive relative to its parent object
	void append( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Append a primitive to an object
	
	void removeObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Remove and delete an object
	void removePrimitive( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Remove and delete a primitive
	
	void clearWorld();																		//!< Remove and delete everything in the world model and reset naming counters
	
	void getList(yarp::os::Bottle& reply);											//!< Get a list of objects in the world
	void getState(const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n);		//!< Get the 4x4 rototranslation matrix of an object
	
	RobotModel::CompositeObject* getObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );												//!< Return a pointer to an object by looking up its name
	RobotModel::PrimitiveObject* getPrimitive( const RobotModel::CompositeObject* Object, const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Return a pointer to a primitive by looking up its name
	QString getName( const yarp::os::Bottle& cmd, int& n  );																								//!< Parse the RPC command and get a name string out
	
	RobotModel::World* world;	//!< Pointer to the world model on which this interface operates
};
#endif
/** @} */