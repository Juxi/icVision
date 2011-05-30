/*
 *  vectorStreamPort.cpp
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "yarpRpcPort.h"
#include "constants.h"

using namespace VirtualSkin;

YarpRpcPort::YarpRpcPort() : keepListening(true), debug(true)
{
	port.setStrict();
}
YarpRpcPort::~YarpRpcPort()
{
	port.close();
}

bool YarpRpcPort::open( const QString& name )
{
	// check whether a network is available
    if ( false == yarp::os::Network::checkNetwork() ) {
		printf("YARP RPC ERROR: yarp network unavailable...\n");
    	return false;
    }
	
	if ( port.open( name.toStdString().c_str() ) )
	{
		start();
		return true;
	}
	else { return 0; }
}

void YarpRpcPort::run() 
{
	//printf("Starting RPC server...\n");
	
	yarp::os::Bottle* cmd;
	yarp::os::Bottle& reply = port.prepare();
    while ( keepListening )
	{
		cmd = port.read(false);
		if (cmd!=NULL)
		{
			reply.clear();
			if (debug) { showBottle(*cmd); }
			if ( !handler(*cmd,reply) ) { reply.addString("Unknown Command! Type 'help' for a list of valid commands."); }
			port.writeStrict();
			if (debug) { printf("reply: %s\n",reply.toString().c_str()); }
		}
		usleep(YARP_PERIOD_us);
    }
	
	port.close();
}

void YarpRpcPort::showBottle( yarp::os::Bottle& anUnknownBottle, int indentation)
{
    for (int i=0; i<anUnknownBottle.size(); i++) {
        for (int j=0; j<indentation; j++) { printf(" "); }
        printf("[%d]: ", i);
        yarp::os::Value& element = anUnknownBottle.get(i);
        switch (element.getCode()) {
			case BOTTLE_TAG_INT:
				printf("int %d\n", element.asInt());
				break;
			case BOTTLE_TAG_DOUBLE:
				printf("float %g\n", element.asDouble());
				break;
			case BOTTLE_TAG_STRING:
				printf("string \"%s\"\n", element.asString().c_str());
				break;
			case BOTTLE_TAG_BLOB:
				printf("binary blob of length %d\n", element.asBlobLength());
				break;
			case BOTTLE_TAG_VOCAB:
				printf("vocab [%s]\n", yarp::os::Vocab::decode(element.asVocab()).c_str());
				break;
			default:
				if (element.isList()) {
					yarp::os::Bottle *lst = element.asList();
					printf("list of %d elements\n", lst->size());
					showBottle(*lst,indentation+2);
				} else {
					printf("unrecognized type\n");
				}
				break;
        }
    }
}

void YarpRpcPort::close()
{
	keepListening = false;
	while (isRunning()) { msleep(1); }
}

void YarpRpcPort::restart()
{
	close();
	keepListening = true;
	start();
}