/*
 *  vectorStreamPort.cpp
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "filterRpcInterface.h"
#include "robotFilter.h"
#include "virtualskinexception.h"
#include "constants.h"

using namespace VirtualSkin;

FilterRpcInterface::FilterRpcInterface() : keepRunning(false)
{
	
}
FilterRpcInterface::~FilterRpcInterface()
{
}

void FilterRpcInterface::open( const QString& portName )
{
	//if ( isOpen ) { close(); }
	if ( !filter ) { throw VirtualSkin::VirtualSkinException("FilterRpcInterface must have a valid ReflexFilter. Use setFilter(ReflexFilter*)"); }
	if ( !yarp::os::Network::checkNetwork() ) { throw( VirtualSkin::VirtualSkinException("yarp network unavailable...") ); }
	
	port.open(portName.toStdString().c_str());
	keepRunning = true;
	start();
}

void FilterRpcInterface::close()
{
	printf("Closing FilterRpcInterface ");
	keepRunning = false;
	port.interrupt();
	while ( isRunning() )
	{ 
		printf("."); 
		msleep(1); 
	}
	printf("\n");
	port.close();
}

void FilterRpcInterface::run()
{
	yarp::os::Bottle cmd,response;
	
	while ( keepRunning )
	{
		port.read(cmd,true);
		response.clear();
		if ( cmd.size() > 0 )
		{
			//if (debug) { showBottle(cmd); }
			handler(cmd,response);
			port.reply(response);
			//if (debug) { printf("reply: %s\n",response.toString().c_str()); }
		}
		msleep(YARP_PERIOD);
	}
}

bool FilterRpcInterface::handler( const yarp::os::Bottle& command, yarp::os::Bottle& reply ) 
{
	//printf("FilterRpcInterface Handler Called!\n");
	
	int n = 0;	// identifier of the current bottle element
	int cmd;	// the command (see command vocabs in header)
	
	yarp::os::ConstString prefix = command.get(n).asString();
	
	if ( prefix=="help" )
	{
		reply.addString("No help for YOU!!!");
		return true;
	}
	if ( prefix=="setWP" )
	{
		filter->setWaypoint();
		reply.addString("set a waypoint!");
		return true;
	}
	else
	{
		reply.addString("Unknown RPC command");
		return false;
	}
}
