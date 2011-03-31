// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StreamFilter.cpp Implementation file for the \c StreamFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "./yarp/os/StreamFilter.h"

using namespace yarp::os;
using yarp::os::StreamFilter;

StreamFilter::StreamFilter() :
	TypedReaderCallback<Bottle> (), inPort(), impl() {
}

StreamFilter::~StreamFilter() {
	close();
}

bool StreamFilter::open(ConstString inPortName, ConstString outPortName) {
	if (true == impl.open(outPortName)) {
		if (true == inPort.open(inPortName)) {
			inPort.useCallback(impl);
			isOpen = true;
			return true;
		} else {
			impl.close();
		}
	}
	return false;
}

void StreamFilter::close() {
	inPort.disableCallback();
	inPort.close();
	impl.close();
	isOpen = false;
}

void StreamFilter::cutConnection(bool cut) {
	impl.cutConnection(cut);
}

void StreamFilter::setObserver(IObserver *o) {
	impl.setObserver(o);
}

void StreamFilter::injectData(const Bottle &b) {
	impl.injectData(b);
}
