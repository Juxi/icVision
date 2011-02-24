/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file CallObserver.h Header file for the CallObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef CALLOBSERVER_H_
#define CALLOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "robot.h"
#include "ResponseObserver.h"

class CallObserver: public yarp::os::IObserver {

public:
	CallObserver(RobotModel::Robot* r, const int b);
	virtual ~CallObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);

	void setResponseObsever(ResponseObserver *o);

private:
	RobotModel::Robot* robot;
	const int branch;
	ResponseObserver *responseObserver;
};

#endif /* CALLOBSERVER_H_ */
