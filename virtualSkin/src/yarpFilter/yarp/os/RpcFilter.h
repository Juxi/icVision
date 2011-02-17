// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RpcFilter.h Header file for the RpcFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef RPCFILTER_H_
#define RPCFILTER_H_

#include <yarp/os/all.h>
#include "impl/RpcFilterImpl.h"
#include "IObserver.h"
#include "IReplier.h"

namespace yarp {
	namespace os {
		class RpcFilter;
	}
}

/**
 * A filter that allows to observe and cut RPC communication to a target YARP
 * port.
 * It provides the following capabilities:
 * - Calls can be observed by setting a <em>call observer</em>.
 * - Responses can be observed by setting a <em>response observer</em>.
 * - The RPC communication between clients and the target port can be cut.
 * - When the communication is cut
 *   - calls to the target port can be injected, and
 *   - responses to calling clients can be injected by setting a @em replier.
 *
 * These capabilities allow to
 * - observe the flow of calls and responses to/from a target port, and
 * - decouple a target port from its clients and replace it by e.g. a simulator.
 */
class yarp::os::RpcFilter {

public:

	/**
	 * Constructs an \c RpcFilter object.
	 * To make the filter operational \c open has to be called.
	 */
	RpcFilter();

	virtual ~RpcFilter();

	/**
	 * \todo Document this function
	 */
	bool open(ConstString targetPortName, ConstString clientsidePortName);

	/**
	 * \todo Document this function
	 */
	bool close();

	/**
	 * Sets the cut status of the filter.
	 * If the cut status is \c false then
	 * - calls from clients are passed to the target port,
	 * - responses from the target port are passed to the calling client, and
	 * - no calls may be injected to the target port.
	 * Otherwise
	 * - no call from the clients are passed to the target port,
	 * - no responses from the target port are passed to any client,
	 * - calls may be injected to the target port, and
	 * - if a replier is set it will be used to get replies to calls from
	 *   the clients.
	 * @param cut The new cut status.
	 */
	void cutConnection(bool cut = true);

	/**
	 * Sets the observer to be informed when a call is received from a client.
	 * This observer will be informed about incoming calls regardless of the
	 * cut status of the filter.
	 * @param o The call observer to be set. May be \c NULL.
	 */
	void setCallObserver(IObserver *o);

	/**
	 * Sets the observer to be informed when a response to a call is received
	 * from the target port.
	 * This observer will be informed about responses from the target port
	 * regardless of the cut status of the filter.
	 *
	 * \attention There may be more than one response to a single call!
	 * @param o The response observer to be set. May be \c NULL.
	 */
	void setResponseObserver(IObserver *o);

	/**
	 * Sets the replier to be asked for replies to a call from a client.
	 * This replier will only be asked for replies to be passed on to the
	 * calling client when the connection is cut.
	 *
	 * \attention The validity of the responses is the sole responsibility of
	 * the replier. The filter does not carry out any validation activity
	 * whatsoever!
	 * @param r The replier to be set. May be \c NULL.
	 */
	void setReplier(IReplier *r);

	/**
	 * Injects a call to the target port.
	 * The call will only be passed to the target port when the connection is
	 * cut. A set call observer is \e not informed about injected calls.
	 *
	 * \attention The validity of the call is the sole responsibility of the
	 * caller. The filter does not carry out any validation activity
	 * whatsoever!
	 * @param b The call to be injected.
	 * @return \c true if the call is passed on to the target port.
	 */
	bool injectCall(const Bottle &b);

	/**
	 * \todo Implement and document this function IS THIS REALLY NECESSARY
	 */
	//	virtual bool configure(Searchable &config);

private:
	// implements the core functionality of the filter
	yarp::os::impl::RpcFilterImpl impl;

	// the port that builds the interface to the calling clients
	Port clientsidePort;

	// is the filter open?  (that is, is it connected?)
	bool isOpen;
};

#endif /* RPCFILTER_H_ */
