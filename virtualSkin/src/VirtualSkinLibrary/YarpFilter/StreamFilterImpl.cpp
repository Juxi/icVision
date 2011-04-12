// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StreamFilterImpl.cpp Implementation file for the \c StreamFilterImpl
 * class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "./yarp/os/impl/StreamFilterImpl.h"

using namespace yarp::os;
using yarp::os::impl::StreamFilterImpl;

StreamFilterImpl::StreamFilterImpl() :
	TypedReaderCallback<Bottle> (), outPort(), isOpen(false), isCut(false),
			observer(NULL) {
}

StreamFilterImpl::~StreamFilterImpl() {
	close();
}

bool StreamFilterImpl::open(ConstString outPortName) {
	if (outPort.open(outPortName) == true) {
		isOpen = true;
		return true;
	}
	return false;
}

void StreamFilterImpl::close() {
	outPort.close();
	isOpen = false;
}

void StreamFilterImpl::cutConnection(bool cut) {
	isCut = cut;
}

void StreamFilterImpl::setObserver(IObserver *o) {
	observer = o;
}

void StreamFilterImpl::injectData(const Bottle &b) {
	Bottle &out = outPort.prepare();
	out.clear();
	out.append(b);
	outPort.write();
}

void StreamFilterImpl::onRead(Bottle &b) {
	if (false == isCut) {
		injectData(b);
	}
	if (NULL != observer) {
		observer->onDataObserved(b);
	}
}
