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

#include "robotinterface.h"

#include <QObject>
#include <QVector>
#include <yarp/os/ControlBoardFilter.h>
#include <yarp/os/IObserver.h>

/**
 * \todo The following enhancements have to be done soon
 *   - add an observer for the command port
 *   - add the ability to cut the filter
 *
 * \todo The following enhancements can be implemented when required
 *   - add the ability to control the robot when the filter is cut
 *   - add the ability to simulate the robot when the filter is cut
 */
class RobotFilter: public QObject {
Q_OBJECT

public:
	RobotFilter(QObject *parent = 0);
	virtual ~RobotFilter();

	bool open(RobotInterface &robot);
	void close();

private:
	bool isOpen;

	// A vector of ControlBoardFilters - one for each of the kinematic branches
	// of the robot. These filters have a 1-to-1 correspondence to the YARP
	// (server) control board drivers of the robot.
	QVector<yarp::os::ControlBoardFilter *> cbFilters;

	// A vector of IObservers. Each kinematic branch has 4 observers.
	// One observes the position or velocity commands that flow into the YARP
	// command port of the corresponding (server) control board driver, one
	// that observers the encoder readings that flow from the state port, one
	// that observes the calls sent to the rpc port and a one that observes
	// the replies to these commands (also on the rpc port).
	// ---
	// NOTE: in this first implementation the observers for the command port
	//       are not yet implemented.
	QVector<yarp::os::IObserver *> observers;
};

#endif /* ROBOTFILTER_H_ */
