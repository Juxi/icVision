/*
 *  vectorStreamPort.h
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef YARPSTREAMPORT_H
#define YARPSTREAMPORT_H

#include <yarp/os/all.h>
#include <QThread>
#include <QMutex>

namespace VirtualSkin {

class YarpStreamPort: public QThread
{
	Q_OBJECT
		
public:
	YarpStreamPort();
	~YarpStreamPort();
	
	void setName( const QString& name ) { portName = name; }
	void setBottle( const yarp::os::Bottle& aBottle );

	void run();
	
	void stop();
	void restart();

protected:
	QMutex mutex;
	yarp::os::Bottle bottle;
	
private:
	yarp::os::Network yarp;
	yarp::os::Port port;
	bool keepRunning;
	QString portName;

};
}
#endif
