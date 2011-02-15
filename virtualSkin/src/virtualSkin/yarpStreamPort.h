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
#include <iostream>

using namespace yarp::os;

class YarpStreamPort: public QThread
{
	Q_OBJECT
		
public:
	YarpStreamPort();
	~YarpStreamPort();
	
	void setName( const QString& name ) { portName = name; }
	void setBottle( const Bottle& aBottle );

	void run();
	
	void stop();
	void restart();

protected:
	QMutex mutex;
	Bottle bottle;
	
private:
	Network yarp;
	Port port;
	bool keepRunning;
	QString portName;

};

#endif
