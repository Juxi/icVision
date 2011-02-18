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

#include "CallObserver.h"

CallObserver::CallObserver( RobotModel::Robot* r, const int b) :
	IObserver(), robot(r), branch(b), responseObserver(NULL) {
	// No special action to take for construction
}

CallObserver::~CallObserver() {
	// Nothing to destruct here
}

void CallObserver::onDataObserved(yarp::os::Bottle &b) {
	//std::cout << "RPC Call --> bodyPart:" << branch << " yarpBottle: " << b.toString() << std::endl;
	
	if (NULL != responseObserver) {
		responseObserver->activeCall = yarp::os::Bottle(b);
	}
}

void CallObserver::setResponseObsever(ResponseObserver *o) {
	responseObserver = o;
}
