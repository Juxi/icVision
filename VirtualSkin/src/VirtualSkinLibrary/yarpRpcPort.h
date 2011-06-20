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

namespace VirtualSkin
{
	class YarpRpcPort;
}

/** \brief A call/response style YARP port
 */
class VirtualSkin::YarpRpcPort: public QThread
{

public:
	YarpRpcPort();									//!< Nothing special to do here
	virtual ~YarpRpcPort();							//!< Calls close() if necessary
	
	void useDebug( bool b = true ) { debug = b; }	//!< Controls whether or not to print bottles to the terminal
	
	bool open( const QString& name );				//!< Starts the RPC port running in a separate thread
	void close();									//!< Stop the thread and closes the port
	void restart();									//!< Restart the thread

private:
	yarp::os::Network yarp;							//!< Identifies the yarp network
	yarp::os::RpcServer port;						//!< The port we are wrapping
	//yarp::os::BufferedPort<yarp::os::Bottle> port;	
	bool	keepListening,							//!< Provides a graceful way to control when the run() method returns
			debug;									//!< Controls whether or not to print bottles to the terminal
	
	void run();																			//!< Don't call this call start() instead
	virtual bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply ) = 0;	//!< Handle incomming RPC calls by calling functions
	void showBottle( yarp::os::Bottle& anUnknownBottle, int indentation = 0 );			//!< Print a yarp::os::bottle to the terminal

};
#endif
/** @} */