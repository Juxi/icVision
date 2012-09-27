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

/** \addtogroup YarpFilter
 *	@{
 */

#ifndef IOBSERVER_H_
#define IOBSERVER_H_

#include <yarp/os/all.h>

namespace yarp {
	namespace os {
		class IObserver;
	}
}

/** \brief An interface that defines a handler for data arriving on YARP ports
 * 
 * The handler is called by filters (RpcFilter and StreamFilter) and provides access to the data flowing through the filter.
 * See MoBeE::StateObserver, MoBeE::CallObserver and MoBeE::ResponseObserver
 */
class yarp::os::IObserver {

public:

	virtual ~IObserver(){}									//!< Nothing to do here
	virtual void onDataObserved(yarp::os::Bottle &b) = 0;	//!< The handler function 
	
};
#endif
/** @} */