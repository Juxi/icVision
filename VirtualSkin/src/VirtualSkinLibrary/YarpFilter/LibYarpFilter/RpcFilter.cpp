// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RpcFilter.cpp Implementation file for the RpcFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "./yarp/os/RpcFilter.h"

using namespace yarp::os;
using yarp::os::RpcFilter;

RpcFilter::RpcFilter() :
	impl(), clientsidePort(), isOpen(false) {
}

RpcFilter::~RpcFilter() {
	close();
}

bool RpcFilter::open(ConstString targetPortName, ConstString clientsidePortName)
{
	if (isOpen == false)
	{
		if (impl.open(targetPortName) == true)
		{
			if (clientsidePort.open(clientsidePortName) == true)
			{
				clientsidePort.setReader(impl);
				isOpen = true;
				printf("Success!!\n");
				return true;
			}
			else
			{
				impl.close();
			}
		}
	}
	return false;
}

bool RpcFilter::close()
{
	if (isOpen == true)
	{
		clientsidePort.interrupt();
		clientsidePort.close();
		impl.close();
		isOpen = false;
		return true;
	}
	return false;
}

void RpcFilter::cutConnection(bool cut) {
	impl.cutConnection(cut);
}

void RpcFilter::setCallObserver(IObserver *o) {
	impl.setCallObserver(o);
}

void RpcFilter::setResponseObserver(IObserver *o) {
	impl.setResponseObserver(o);
}

void RpcFilter::setReplier(IReplier *r) {
	impl.setReplier(r);
}

bool RpcFilter::injectCall(const Bottle &b) {
	return impl.injectCall(b);
}
