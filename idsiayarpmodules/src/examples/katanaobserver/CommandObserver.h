/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file CommandObserver.h Header file for the CommandObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef COMMANDOBSERVER_H_
#define COMMANDOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "robotinterface.h"

class CommandObserver: public yarp::os::IObserver {

public:
	CommandObserver(RobotInterface &r, const int b);
	virtual ~CommandObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);

private:
	RobotInterface &robot;
	const int branch;
};

#endif /* COMMANDOBSERVER_H_ */
