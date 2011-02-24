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

class RobotFilter: public QThread
{
	Q_OBJECT

public:
	RobotFilter();
	virtual ~RobotFilter();
	
	void   setRobot(RobotModel::Robot* r) { robot = r; }
	RobotModel::Robot* getRobot() { return robot; }
	void   setPortName( const QString& name ) { statusPort.setName(name); }
	
	bool open();
	void close();
	
	virtual void run();

signals:
	//! Signifies that the new pose has been computed
    /*!  */
    void dataReceived();
	void haveControl();
	void returnControl();
	
public slots:
	void takeControl();

private:
	bool isOpen;
	RobotModel::Robot* robot;
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
	QVector<RobotModel::PartController*>	partControllers;
	QVector< QVector<qreal> > targetPose;
	YarpStreamPort statusPort;
	
	friend class StateObserver;
};

#endif /* ROBOTFILTER_H_ */
