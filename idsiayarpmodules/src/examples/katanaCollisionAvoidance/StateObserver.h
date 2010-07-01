/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StateObserver.h Header file for the StateObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef STATEOBSERVER_H_
#define STATEOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "robot.h"

class StateObserver: public yarp::os::IObserver {

public:
	StateObserver(Robot &r, const int b);
	virtual ~StateObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);

private:
	Robot &robot;
	const int branch;
};

#endif /* STATEOBSERVER_H_ */
