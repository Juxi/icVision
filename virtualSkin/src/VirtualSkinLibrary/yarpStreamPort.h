/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef YARPSTREAMPORT_H
#define YARPSTREAMPORT_H

#include <yarp/os/all.h>
#include <QThread>

namespace VirtualSkin {

/** \brief A YARP port that broadcasts a stream
*/
class YarpStreamPort: public QThread
{
	
public:
	YarpStreamPort();				//!< Nothing special to do here
	virtual ~YarpStreamPort();		//!< Nothing special to do here

	void open( const QString& name );	//!< Opens the port and starts the thread to stream bottles
	void close();						//!< Stops the thread and closes the port
	
	void setBottle( const yarp::os::Bottle& aBottle );	//!< Sets the value of the bottle that is streamed
	
private:
	yarp::os::Network yarp;	//!< Identifies the yarp network
	yarp::os::Port port;	//!< The port we are wrapping
	bool keepRunning;		//!< Provides a graceful way to control when the run() method returns

	//QMutex mutex;
	yarp::os::Bottle bottle;
	
	void run();		//!< Runs the thread
	void stop();	//!< Stops the thread

};
}
#endif
/** @} */
