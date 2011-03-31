/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef YARPRPCPORT_H
#define YARPRPCPORT_H

#include <yarp/os/all.h>
#include <QThread>

namespace VirtualSkin {

/** \brief A call/response style YARP port
 */
class YarpRpcPort: public QThread
{

public:
	YarpRpcPort();				//!< Nothing special to do here
	virtual ~YarpRpcPort();		//!< Calls close() if necessary
	
	void useDebug() { debug = true; }	//!< Causes the port to print bottles to the terminal
	void noDebug() { debug = false; }	//!< Prevents the port from printing bottles
	
	bool open( const QString& name );	//!< Starts the RPC port running in a separate thread
	
	void close();		//!< Stop the thread
	void restart();		//!< Restart the thread

private:
	yarp::os::Network yarp;
	yarp::os::BufferedPort<yarp::os::Bottle> port;
	bool keepListening,debug;
	//QString portName;
	
	void run();																			//!< Don't call this call start() instead
	virtual bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply ) = 0;	//!< Handle incomming RPC calls by calling functions
	void showBottle( yarp::os::Bottle& anUnknownBottle, int indentation = 0 );			//!< Print a yarp::os::bottle to the terminal

};
}
#endif
/** @} */