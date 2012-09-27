/*
 *  vectorStreamPort.cpp
 *  MoBeE
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "constants.h"
#include "yarpStreamPort.h"

using namespace MoBeE;

YarpStreamPort::YarpStreamPort() : keepRunning(true)
{
}
YarpStreamPort::~YarpStreamPort()
{
	close(); 
}

void YarpStreamPort::setBottle( const yarp::os::Bottle& aBottle )
{
	QMutexLocker locker(&mutex);
	bottle = aBottle;
}

void YarpStreamPort::open( const QString& name )
{
	port.open( name.toStdString().c_str() );
	start();
}

void YarpStreamPort::close()
{
	if ( isRunning() ) { stop(); }
	bottle.clear();
	port.close();
}

void YarpStreamPort::run() 
{
	while ( keepRunning )
	{
		mutex.lock();
			port.write( bottle );
		mutex.unlock();
		usleep(YARP_PERIOD_us);
	}
}

void YarpStreamPort::stop()
{
	keepRunning = false;
	while (isRunning()) {}
}