/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RobotFilter.h Header file for the RobotFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef ROBOTFILTER_H_
#define ROBOTFILTER_H_

#include <QMutex>
#include <QThread>
#include <QVector>
#include <yarp/os/ControlBoardFilter.h>

#include "robot.h"
#include "callObserver.h"
#include "stateObserver.h"
#include "responseObserver.h"
#include "yarpModel.h"
#include "yarpStreamPort.h"

namespace VirtualSkin {

	class RobotFilter : public QThread
{
	Q_OBJECT

public:
	RobotFilter( bool visualize = false );
	virtual ~RobotFilter();

	void openStatusPort( const QString& name ) { statusPort.open(name); }
	void startStatusPort()		{ statusPort.start(); }
	void stopStatusPort()		{ statusPort.stop(); }
	void restartStatusPort()	{ statusPort.restart(); }
	void closeStatusPort()		{ statusPort.close(); }
	
	//void setCollisionPortName( const QString& name ) { model.setCollisionPortName(name); }
	//void openCollisionPort() { model.openCollisionPort(); }
	//void closeCollisionPort() { model.closeCollisionPort(); }
	
	//void setWorldRpcPortName( const QString& name ) { model.setWorldRpcPortName(name); }
	//void openWorldRpcPort() { model.openWorldRpcPort(); }
	//void closeWorldRpcPort() { model.closeWorldRpcPort(); }
	
	template <class someStateObserver, class someCallObserver, class someResponseObserver>
	bool open( const QString& fileName )
	{
		if ( isOpen ) { close(); }
		
		// check whether a network is available
		if ( !yarp::os::Network::checkNetwork() )
		{
			printf("ROBOT FILTER ERROR: yarp network unavailable...\n");
			return false;
		}
		
		model.robot.open(fileName);
		model.start();
		
		const QString deviceBaseName( model.robot.getName() );
		const QString filterBaseName( model.robot.getName() + "F" );
		
		// TODO: create CommandObserver
		// TODO: create Replier
		yarp::os::ControlBoardFilter *p_cbf;	// create the port filter
		someStateObserver *p_so;					// create an observer for the encoder readings
		someCallObserver *p_co;
		someResponseObserver *p_ro;					// create an observer for the rpc responses
		
		QString targetName;
		QString filterName;
		
		for (int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
		{
			p_cbf = new yarp::os::ControlBoardFilter();
			
			filterName = "/" + model.robot.getName() + "F/" + *(model.robot.getPartName(bodyPart));
			targetName = "/" + model.robot.getName() + "/" + *(model.robot.getPartName(bodyPart));
			
			printf("----------------------------------------------------------------\n");
			printf( "connecting to %s:%s\n", model.robot.getName().toStdString().c_str(), model.robot.getPartName(bodyPart)->toStdString().c_str() );
			
			if ( p_cbf->open(filterName.toStdString().c_str(), targetName.toStdString().c_str()) )
			{
				cbFilters.append(p_cbf);
				
				// create and set state observer
				p_so = new someStateObserver(this, bodyPart);
				p_cbf->setStateObserver(p_so);
				stateObservers.append(p_so);
				
				// create and set call observer
				p_co = new someCallObserver(this, bodyPart);
				p_cbf->setCallObserver(p_co);
				callObservers.append(p_co);
				
				// create and set response observer
				p_ro = new someResponseObserver(this, bodyPart);
				p_co->setResponseObsever(p_ro);
				p_cbf->setResponseObserver(p_ro);
				responseObservers.append(p_ro);
				
				QObject::connect(p_so, SIGNAL(setPosition(int,const QVector<qreal>&)),	&model.robot, SLOT(setEncoderPosition(int,const QVector<qreal>&)) );
				QObject::connect(p_ro, SIGNAL(setPosition(int,int,qreal)),				&model.robot, SLOT(setEncoderPosition(int,int,qreal)) );
				
			}
			else
			{
				printf("Failed to find YARP port: %s\n", targetName.toStdString().c_str());
				delete p_cbf;
				close();
				return false;
			}
		} 
		
		extraOpenStuff();

		isOpen = true;
		statusPort.setBottle("1");
		
		return true;
	}
	
	void close();
	
	virtual void extraOpenStuff() = 0;
	virtual void collisionResponse() = 0;
	
	void run();
	
	YarpModel			model;
	
public slots:
	
	void takeControl();

protected:
	
	YarpStreamPort		statusPort;
	yarp::os::Bottle	stop_command;
	bool				isOpen,haveControl;
	
	QVector<bool> isBusy;
	
	// A vector of ControlBoardFilters - one for each of the body parts
	// of the robot. These filters have a 1-to-1 correspondence to the YARP
	// (server) control board drivers of the robot.
	QVector<yarp::os::ControlBoardFilter *> cbFilters;

	// A vector of IObservers. Each kinematic branch (body part) has 4 observers.
	// One observes the position or velocity commands that flow into the YARP
	// command port of the corresponding (server) control board driver, one
	// that observers the encoder readings that flow from the state port, one
	// that observes the calls sent to the rpc port and a one that observes
	// the replies to these commands (also on the rpc port).
	// ---
	// NOTE: in this first implementation the observers for the command port are not yet implemented.
	QVector<StateObserver*>		stateObservers;
	QVector<CallObserver*>		callObservers;
	QVector<ResponseObserver*>	responseObservers;
	
};
}
#endif
