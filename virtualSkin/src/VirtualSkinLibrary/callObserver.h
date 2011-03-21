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
#include "responseObserver.h"

namespace VirtualSkin {
	
class RobotFilter;

class CallObserver: public yarp::os::IObserver {

public:
	CallObserver(RobotFilter* r, const int b);
	virtual ~CallObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);

	void setResponseObsever(ResponseObserver *o);

private:
	RobotFilter			*robotFilter;
	const int			bodyPart;
	ResponseObserver	*responseObserver;
};
	
}

#endif
