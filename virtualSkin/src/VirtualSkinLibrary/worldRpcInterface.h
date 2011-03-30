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
#include <yarp/os/all.h>
#include "yarpRpcPort.h"

namespace RobotModel {
	class World;
	class CompositeObject;
	class PrimitiveObject;
}

namespace VirtualSkin {
	
/** \brief Provides an RPC interface (via YARP) to RobotModel::World
 *
 *	This interface is compatible with that of the iCub simulator in terms of the syntax of RPC commands, the kinds of geometries supported and the
 *  way those geometries are named.  However in addition to individual primitives, the World supports CompositeObjects, made up of unions of PrimitiveObjects.
 */

class WorldRpcInterface: public YarpRpcPort
{
		
public:

	/** \addtogroup YarpVocabs
	 *	@{
	 *  TODO: investigate absolute vs relative positions/orientations as they apply to the iCub simulator
	 */
	// commands for objects
	#define VOCAB_MK VOCAB2('m','k')			//!< Make a new object
	#define VOCAB_SET VOCAB3('s','e','t')		//!< Set the position of an object
	#define VOCAB_ROT VOCAB3('r','o','t')		//!< Rotate an object
	#define VOCAB_REM VOCAB2('r','m')			//!< Remove an object
	
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
	
	RobotModel::CompositeObject* getObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );												//!< Return a pointer to an object by looking up its name
	RobotModel::PrimitiveObject* getPrimitive( const RobotModel::CompositeObject* Object, const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Return a pointer to a primitive by looking up its name
	QString getName( const yarp::os::Bottle& cmd, int& n  );																								//!< Parse the RPC command and get a name string out
	
	RobotModel::World* world;	//!< Pointer to the world model on which this interface operates
};
}
#endif
/** @} */