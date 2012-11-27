/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef CONTROLLERRPCINTERFACE_H
#define CONTROLLERRPCINTERFACE_H

#include <QThread>
//#include <QVector3D>
//#include <QMatrix4x4>
#include <yarp/os/all.h>



class Controller;
	
/** \brief Provides an RPC interface (via YARP) to RobotModel::World
 *
 *	This interface is compatible with that of the iCub simulator in terms of the syntax of RPC commands, the kinds of geometries supported and the
 *  way those geometries are named.  However in addition to individual primitives, the World supports CompositeObjects, made up of unions of PrimitiveObjects.
 */
class ControllerRpcInterface : public QThread
{
		
public:

	ControllerRpcInterface( Controller* );			//!< Nothing special to do here
	virtual ~ControllerRpcInterface();	//!< Nothing special to do here
	
	//void setModel( MoBeE::YarpModel* m ) { model = m; }	//!< Sets the RobotModel::World on which this interface operates
	void open( yarp::os::ConstString& portName );
	void close();

private:
	
    #define VOCAB_LIST VOCAB4('l','i','s','t')
    #define VOCAB_GET_MARKER VOCAB3('g','e','t')
    //#define VOCAB_GET_N VOCAB4('g','e','t','n')
    
	void run();
	bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );
    
    bool getMarkerList( yarp::os::Bottle& reply );
    bool getMarker( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );
    //bool getMarkerPoint( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );
    //bool getMarkerNormal( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );
	
	yarp::os::Network	yarp;
	yarp::os::RpcServer	port;
    Controller* controller;
	bool keepRunning;
	
	//uint s, c, b, ss, sc, sb; // counters for naming objects
	
};
#endif
/** @} */