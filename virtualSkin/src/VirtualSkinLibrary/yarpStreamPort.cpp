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

YarpStreamPort::YarpStreamPort() : portIsOpen(false), keepRunning(true)
{
}
YarpStreamPort::~YarpStreamPort()
{
	if ( portIsOpen ) { close(); }
}

void YarpStreamPort::setBottle( const yarp::os::Bottle& aBottle )
{
	bottle = aBottle;
}

void YarpStreamPort::open( const QString& name )
{
	port.open( name.toStdString().c_str() );
	portIsOpen = true;
	start();
}

void YarpStreamPort::close()
{
	if ( isRunning() ) { stop(); }
	bottle.clear();
	port.close();
	portIsOpen = false;
}

void YarpStreamPort::run() 
{
	if ( portIsOpen )
	{
		while ( keepRunning )
		{
			port.write( bottle );
			usleep(YARP_PERIOD_us);
		}
	}
}

/*void YarpStreamPort::write( const yarp::os::Bottle& aBottle )
{
	bottle = aBottle;
	port.write( bottle );
}*/

void YarpStreamPort::stop()
{
	keepRunning = false;
	while (isRunning()) {}
}

/*void YarpStreamPort::restart()
{
	if ( isRunning() ) { stop(); }
	keepRunning = true;
	start();
}*/