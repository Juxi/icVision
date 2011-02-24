/*
 *  vectorRpcPort.h
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef YARPRPCPORT_H
#define YARPRPCPORT_H

#include <yarp/os/all.h>
#include <QThread>

namespace RobotModel {

class YarpRpcPort: public QThread
{
	Q_OBJECT
		
public:
	YarpRpcPort();
	virtual ~YarpRpcPort();
	
	void useDebug() { debug = true; }
	void noDebug() { debug = false; }
	
	void setName( const QString& name ) { portName = name; }
	
	virtual void run();
	virtual bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply ) = 0;
	void showBottle( yarp::os::Bottle& anUnknownBottle, int indentation = 0 );
	
	void stop();
	void restart();

private:
	yarp::os::Network yarp;
	yarp::os::BufferedPort<yarp::os::Bottle> port;
	bool keepListening,debug;
	QString portName;

};

}

#endif