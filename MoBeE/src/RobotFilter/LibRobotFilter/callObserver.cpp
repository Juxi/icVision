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
#include "robotFilter.h"

using namespace MoBeE;

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
	
	/*printf("BodyPart %d RECEIVED RPC CALL: %s\n", bodyPart, b.toString().c_str());
	if ( robotFilter->cbFilters.size() > bodyPart && robotFilter->cbFilters.at(bodyPart) )
	{
		if ( robotFilter->cbFilters.at(bodyPart)->isCut() ) printf("  ...intercepted\n");
		else printf("  ...forwarded\n");
	}*/

}

void CallObserver::setResponseObsever(ResponseObserver *o) {
	responseObserver = o;
}
