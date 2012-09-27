/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef WORLDRPCINTERFACE_H
#define WORLDRPCINTERFACE_H

#include <QThread>
#include <QVector3D>
#include <QMatrix4x4>
#include <yarp/os/all.h>

//#include "model.h"

namespace KinematicModel
{
	class CompositeObject;
	class PrimitiveObject;
}

namespace MoBeE
{
	class YarpModel;
	class WorldRpcInterface;
}
	
/** \brief Provides an RPC interface (via YARP) to RobotModel::World
 *
 *	This interface is compatible with that of the iCub simulator in terms of the syntax of RPC commands, the kinds of geometries supported and the
 *  way those geometries are named.  However in addition to individual primitives, the World supports CompositeObjects, made up of unions of PrimitiveObjects.
 */
class MoBeE::WorldRpcInterface : public QThread
{
		
public:

	WorldRpcInterface();			//!< Nothing special to do here
	virtual ~WorldRpcInterface();	//!< Nothing special to do here
	
	void setModel( MoBeE::YarpModel* m ) { model = m; }	//!< Sets the RobotModel::World on which this interface operates
	void open( const QString& portName );
	void close();
	
	bool parseSimRTBottle(const std::string name, const yarp::os::Bottle& command, int& n, QMatrix4x4 &rt);
	KinematicModel::CompositeObject* getObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Return a pointer to an object by looking up its name
	QString getName( const yarp::os::Bottle& cmd, int& n  );														//!< Parse the RPC command and get a name string out

private:
	
	void run();
	bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );	//!< Handles incomming RPC commands (expressed as YARP bottles)
	void make( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Creates a new object
	void set( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Sets the properties of an object
	void respClass( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Sets the properties of an object
	void setPos( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Sets the position of an object (called by set() above)
	void setRot( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Rotate an object
	void setRTfromSim ( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );	//!< Set rototranslation from simulator
	void startSimSyncer(const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n); // !< Read period from command, and start iCubSim synchronization thread
	void removeObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Remove and delete an object
	void grabObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );		//!< Attach an object to a marker
	void getList(yarp::os::Bottle& reply);											//!< Get a list of objects in the world
	void getState(const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n);		//!< Get the 4x4 rototranslation matrix of an object
	
	MoBeE::YarpModel* model;
	
	yarp::os::Network	yarp;
	yarp::os::RpcServer	port;
	bool keepRunning;
	
	uint s, c, b, ss, sc, sb; // counters for naming objects
	
};
#endif
/** @} */