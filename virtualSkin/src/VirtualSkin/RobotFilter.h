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

#include <QObject>
#include <QVector>
#include <yarp/os/ControlBoardFilter.h>

#include "robot.h"
#include "partController.h"
#include "CallObserver.h"
#include "StateObserver.h"
#include "ResponseObserver.h"
#include "collisionDetector.h"
#include "skinWindow.h"

namespace VirtualSkin {

class RobotFilter
{

public:
	RobotFilter( bool visualize = false );
	virtual ~RobotFilter();
	
	void setRobot( RobotModel::Robot* r );
	void setWorld( RobotModel::World *w );
	RobotModel::Robot* getRobot() { return robot; }
	RobotModel::World* getWorld() { return world; }
	
	void setStatusPortName( const QString& name ) { statusPort.setName(name); }
	void setCollisionPortName( const QString& name ) { statusPort.setName(name); }
	
	bool computePose() { return collisionDetector.computePose(); }
	
	template <class someStateObserver, class someCallObserver, class someResponseObserver>
	bool open()
	{
		if ( isOpen ) { close(); }
		
		// check whether a network is available
		if ( !yarp::os::Network::checkNetwork() )
		{
			printf("ROBOT FILTER ERROR: yarp network unavailable...\n");
			return false;
		}
		
		// check whether there is a robotModel
		if ( !robot )
		{
			printf("ROBOT FILTER ERROR: no robot model has been set... call setRobot(Robot&) before open()\n");
			return false;
		}
		
		const QString deviceBaseName( robot->getName() );
		const QString filterBaseName( robot->getName() + "F" );
		
		// TODO: create CommandObserver
		// TODO: create Replier
		yarp::os::ControlBoardFilter *p_cbf;	// create the port filter
		someStateObserver *p_so;					// create an observer for the encoder readings
		someCallObserver *p_co;
		someResponseObserver *p_ro;					// create an observer for the rpc responses
		
		QString targetName;
		QString filterName;
		
		for (int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++)
		{
			p_cbf = new yarp::os::ControlBoardFilter();
			
			filterName = "/" + robot->getName() + "F/" + *(robot->getPartName(bodyPart));
			targetName = "/" + robot->getName() + "/" + *(robot->getPartName(bodyPart));
			
			printf("----------------------------------------------------------------\n");
			printf( "connecting to %s:%s\n", robot->getName().toStdString().c_str(), robot->getPartName(bodyPart)->toStdString().c_str() );
			
			if ( p_cbf->open(filterName.toStdString().c_str(), targetName.toStdString().c_str()) )
			{
				cbFilters.append(p_cbf);
				
				// create and set state observer
				p_so = new someStateObserver(this, bodyPart);
				p_cbf->setStateObserver(p_so);
				stateObservers.append(p_so);
				
				// create and set call observer
				p_co = new someCallObserver(robot, bodyPart);
				p_cbf->setCallObserver(p_co);
				callObservers.append(p_co);
				
				// create and set response observer
				p_ro = new someResponseObserver(robot, bodyPart);
				p_co->setResponseObsever(p_ro);
				p_cbf->setResponseObserver(p_ro);
				responseObservers.append(p_ro);
				
			}
			else
			{
				printf("Failed to find YARP port: %s\n", targetName.toStdString().c_str());
				delete p_cbf;
				close();
				return false;
			}
		} 
		
		isOpen = true;
		statusPort.setBottle( yarp::os::Bottle("1") );
		statusPort.start();
		
		return true;
	}
	void close();
	
	virtual void extraOpenStuff() = 0;
	virtual void collisionHandler() = 0;
	
	void takeControl();

protected:
	
	CollisionDetector	collisionDetector;
	SkinWindow			*skinWindow;
	
	RobotModel::Robot		*robot;
	RobotModel::World		*world;
	
	yarp::os::Bottle	stop_command;
	
	bool isOpen;
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
	
	// for collision recovery
	//QVector<RobotModel::PartController*>	partControllers;
	//QVector< QVector<qreal> > originalPose, targetPose;
	YarpStreamPort statusPort;
	
	//friend class StateObserver;
};
}
#endif
