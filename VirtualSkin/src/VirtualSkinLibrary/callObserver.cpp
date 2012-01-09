/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file CallObserver.cpp Implementation file for the CallObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "callObserver.h"

using namespace VirtualSkin;

CallObserver::CallObserver( RobotFilter* r, const int b) :
	IObserver(), robotFilter(r), bodyPart(b), responseObserver(NULL) {
	// No special action to take for construction
}

CallObserver::~CallObserver() {
	// Nothing to destruct here
}

void CallObserver::onDataObserved(yarp::os::Bottle &b) {
	
	if (NULL != responseObserver) {
		responseObserver->activeCall = yarp::os::Bottle(b);
	}
}

void CallObserver::setResponseObsever(ResponseObserver *o) {
	responseObserver = o;
}
