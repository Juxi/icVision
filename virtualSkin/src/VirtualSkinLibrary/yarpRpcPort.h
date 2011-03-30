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

/** \brief A simple call/response style YARP port
 */
class YarpRpcPort: public QThread
{

public:
	YarpRpcPort();				//!< Nothing special to do here
	virtual ~YarpRpcPort();		//!< Calls close() if necessary
	
	void useDebug() { debug = true; }	//!< Causes the port to print bottles to the terminal
	void noDebug() { debug = false; }	//!< Prevents the port from printing bottles
	
	bool open( const QString& name );	//!< Starts the RPC port running in a separate
	
	void run();
	virtual bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply ) = 0;
	void showBottle( yarp::os::Bottle& anUnknownBottle, int indentation = 0 );
	
	void stop();
	void restart();

private:
	yarp::os::Network yarp;
	yarp::os::BufferedPort<yarp::os::Bottle> port;
	bool keepListening,debug;
	//QString portName;

};
}
#endif
/** @} */