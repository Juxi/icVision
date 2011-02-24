/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file ResponseObserver.h Header file for the ResponseObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef RESPONSEOBSERVER_H_
#define RESPONSEOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "robot.h"

class CallObserver;

class ResponseObserver: public yarp::os::IObserver {

public:
	ResponseObserver(RobotModel::Robot* r, const int b);
	virtual ~ResponseObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);

private:
	RobotModel::Robot* robot;
	const int branch;

	// the actual open RPC call
	yarp::os::Bottle activeCall;

	friend class CallObserver;
};

#endif /* RESPONSEOBSERVER_H_ */
