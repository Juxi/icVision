// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file ControlBoardFilter.cpp Implementation file for the
 * \c ControlBoardFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "./yarp/os/ControlBoardFilter.h"
#include "./yarp/os/StreamFilter.h"
#include "./yarp/os/RpcFilter.h"

using namespace yarp::os;
using yarp::os::ControlBoardFilter;

ControlBoardFilter::ControlBoardFilter() :
	stateFilter(), commandFilter(), callFilter(), isOpen(false) {
}

ControlBoardFilter::~ControlBoardFilter() {
}

bool ControlBoardFilter::open(ConstString filterName, ConstString targetName)
{
	ConstString commandInPortName = filterName + "/command:i";
	ConstString commandOutPortName = filterName + "/command:o";
	commandFilter.open(commandInPortName, commandOutPortName);
	isOpen = Network::connect(commandOutPortName, targetName + "/command:i");

	if (true == isOpen) {
		ConstString stateInPortName = filterName + "/state:i";
		ConstString stateOutPortName = filterName + "/state:o";
		stateFilter.open(stateInPortName, stateOutPortName);
		isOpen = Network::connect(targetName + "/state:o", stateInPortName);

		if (true == isOpen) {
			ConstString callTargetPortName = targetName + "/rpc:i";
			ConstString callClientsidePortName = filterName + "/rpc:i";

			printf("RPC port name: %s\n",callTargetPortName.c_str());

			isOpen = callFilter.open( callTargetPortName, callClientsidePortName );
		}
	}

	return isOpen;
}

void ControlBoardFilter::close() {
	callFilter.close();
	stateFilter.close();
	commandFilter.close();
}

void ControlBoardFilter::cutConnection(bool cut) {
	stateFilter.cutConnection(false);
	commandFilter.cutConnection(cut);
	callFilter.cutConnection(cut);
}

void ControlBoardFilter::setStateObserver(IObserver *o) {
	stateFilter.setObserver(o);
}

void ControlBoardFilter::setCommandObserver(IObserver *o) {
	commandFilter.setObserver(o);
}

void ControlBoardFilter::setCallObserver(IObserver *o) {
	callFilter.setCallObserver(o);
}

void ControlBoardFilter::setResponseObserver(IObserver *o) {
	callFilter.setResponseObserver(o);
}

void ControlBoardFilter::setReplier(IReplier *r) {
	callFilter.setReplier(r);
}

void ControlBoardFilter::injectState(const Bottle &b) {
	stateFilter.injectData(b);
}

void ControlBoardFilter::injectCommand(const Bottle &b) {
	commandFilter.injectData(b);
}

void ControlBoardFilter::injectCall(const Bottle &b) {
	callFilter.injectCall(b);
}

//void ControlBoardFilter::setWaypoint()
//{
	
//}
