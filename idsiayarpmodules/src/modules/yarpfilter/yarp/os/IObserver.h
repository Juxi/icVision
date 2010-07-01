// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file IObserver.h Header file for the IObserver interface.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef IOBSERVER_H_
#define IOBSERVER_H_

#include <yarp/os/all.h>

namespace yarp {
	namespace os {
		class IObserver;
	}
}

/**
 * Interface that must be implemented to be able to be informed about
 * information flowing in and out of filters
 * (RpcFilter as well as StreamFilter).
 */
class yarp::os::IObserver {

public:

	/**
	 * This function is called by filters to inform the implementing objects
	 * on data flowing in or out a the calling filter
	 * (RpcFilter as well as StreamFilter).
	 */
	virtual void onDataObserved(yarp::os::Bottle &b) = 0;
};

#endif /* IOBSERVER_H_ */
