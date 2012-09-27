/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup ReflexDemo
 *	@{
 */

#ifndef FILTERRPCINTERFACE_H
#define FILTERRPCINTERFACE_H

#include <QThread>
#include <yarp/os/all.h>

namespace MoBeE
{
	class RobotFilter;
	class FilterRpcInterface;
}
	
/** \brief Provides an RPC interface (via YARP) to RobotModel::World
 *
 *	This interface is compatible with that of the iCub simulator in terms of the syntax of RPC commands, the kinds of geometries supported and the
 *  way those geometries are named.  However in addition to individual primitives, the World supports CompositeObjects, made up of unions of PrimitiveObjects.
 */
class MoBeE::FilterRpcInterface : public QThread
{
		
public:

	FilterRpcInterface();			//!< Nothing special to do here
	virtual ~FilterRpcInterface();	//!< Nothing special to do here
	
	void setFilter( RobotFilter* f ) { filter = f; }	//!< Sets the RobotModel::World on which this interface operates
	void open( const QString& portName );
	void close();
	
private:
	
	void run();
	bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );	//!< Handles incomming RPC commands (expressed as YARP bottles)

	yarp::os::Network	yarp;
	yarp::os::RpcServer	port;
	bool keepRunning;

	RobotFilter* filter;
};
#endif
/** @} */