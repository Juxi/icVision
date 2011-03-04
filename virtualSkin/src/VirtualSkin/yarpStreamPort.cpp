/*
 *  vectorStreamPort.cpp
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "constants.h"
#include "yarpStreamPort.h"

using namespace VirtualSkin;

YarpStreamPort::YarpStreamPort() : keepRunning(true)
{
}
YarpStreamPort::~YarpStreamPort()
{
	if ( isRunning() ) stop();
}

void YarpStreamPort::setBottle( const yarp::os::Bottle& aBottle )
{
	mutex.lock();
	bottle = aBottle;
	mutex.unlock();
}

void YarpStreamPort::run() 
{
	if ( portName.isEmpty() )
	{
		printf("ERROR STARTING YarpStreamPort: set the port name with setName( const QString& ) prior to calling start()\n");
		keepRunning = false;
	} else port.open( portName.toStdString().c_str() );
	
	while ( keepRunning )
	{
		mutex.lock();
			port.write( bottle );
        mutex.unlock();
		usleep(YARP_PERIOD);
	}
	
	port.close();
}

void YarpStreamPort::stop()
{
	keepRunning = false;
	while (isRunning()) {}
}

void YarpStreamPort::restart()
{
	keepRunning = true;
	start();
}