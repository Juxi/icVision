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
#include <QMutex>

namespace VirtualSkin {

class YarpStreamPort: public QThread
{
	
public:
	YarpStreamPort();
	virtual ~YarpStreamPort();

	void open( const QString& name );
	void close();
	bool isOpen() { return portIsOpen; }
	
	//void write( const yarp::os::Bottle& aBottle );
	
	void setBottle( const yarp::os::Bottle& aBottle );
	void run();
	void stop();
	//void restart();

protected:
	QMutex mutex;
	yarp::os::Bottle bottle;
	
private:
	yarp::os::Network yarp;
	yarp::os::Port port;
	bool portIsOpen,keepRunning;

};
}
#endif
/** @} */
