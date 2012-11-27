/*
 *  vectorStreamPort.cpp
 *  MoBeE
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "controller.h"
#include "controllerRpcInterface.h"

ControllerRpcInterface::ControllerRpcInterface( Controller* c ) : controller(c),
                                                                    keepRunning(false)
{
}
ControllerRpcInterface::~ControllerRpcInterface()
{
}

void ControllerRpcInterface::open( yarp::os::ConstString& portName )
{
	//if ( isOpen ) { close(); }
	if ( !controller ) { printf("ControllerRpcInterface must have a pointer to a valid controller"); return; }
	if ( !yarp::os::Network::checkNetwork() ) { printf("ControllerRpcInterface says... yarp network unavailable..."); return; }
	
	port.open(portName);
	keepRunning = true;
	start();
}

void ControllerRpcInterface::close()
{
	printf("Closing ControllerRpcInterface ");
	keepRunning = false;
	port.interrupt();
	while ( isRunning() )
	{ 
		printf("."); 
		msleep(100);
	}
	printf("\n");
	port.close();
}

void ControllerRpcInterface::run()
{
	yarp::os::Bottle cmd,response;
	while ( keepRunning )
	{
		port.read(cmd,true);
		response.clear();
		if ( cmd.size() > 0 )
        {
            handler(cmd,response);
            port.reply(response);
        }
	}
}

bool ControllerRpcInterface::handler( const yarp::os::Bottle& command, yarp::os::Bottle& reply ) 
{
    int cmd  = command.get(0).asVocab();
    bool ok;
    
	switch (cmd)
	{
        case VOCAB_LIST:
            ok = getMarkerList(reply);
            break;
        case VOCAB_GET_P:
            ok = getMarkerPoint(command,reply);
            break;
        case VOCAB_GET_N:
            ok = getMarkerNormal(command,reply);
            break;
		default:
            reply.addString("Unknown RPC command");
            return false;
	}
    if (!ok)
        reply.addString("Handler encountered a problem :-(");
	return true;
}

bool ControllerRpcInterface::getMarkerPoint( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply )
{
    QVector3D point;
    bool ok = controller->getMarkerPosition( QString(cmd.get(1).asString()), point );
    if (!ok) return false;
    else {
        reply.addDouble(point.x());
        reply.addDouble(point.y());
        reply.addDouble(point.z());
    }
    return true;
}

bool ControllerRpcInterface::getMarkerNormal( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply )
{
    QVector3D norm;
    bool ok = controller->getMarkerNormal( QString(cmd.get(1).asString()), norm );
    if (!ok) return false;
    else {
        reply.addDouble(norm.x());
        reply.addDouble(norm.y());
        reply.addDouble(norm.z());
    }
    return true;
}

bool ControllerRpcInterface::getMarkerList( yarp::os::Bottle& reply )
{
    QList<QString> markerList;
    bool ok = controller->getMarkerNames(markerList);
    if (!ok) return false;
    else {
        QList<QString>::iterator i;
        for (i=markerList.begin(); i!=markerList.end(); ++i) {
            reply.addString((*i).toLocal8Bit().data());
        }
    }
    return true;
}
