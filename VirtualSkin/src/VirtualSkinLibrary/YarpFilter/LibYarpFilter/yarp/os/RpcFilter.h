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

/** \addtogroup YarpFilter
 *	@{
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

/** \brief A proxy for a YARP RPC port that can also filter data
 *
 * Creates a YARP RPC port to mimic the existing one and connects the two such that:
 * - Calls can be monitored by setting a CallObserver (see RpcFilterImpl).
 * - Responses can be monitored by setting a ResponseObserver (see RpcFilterImpl).
 * - The RPC communication between clients and the target port can be cut.
 * 
 * When the communication is cut:
 *   - calls to the target port can be injected, and
 *   - responses to calling clients can be injected by setting a Replier (see RpcFilterImpl).
 *
 *	\image html rpcfilter.png "RPC Filter Architecture"
 */
class yarp::os::RpcFilter {

public:

	RpcFilter();					//!< The filter is cut (not forwarding commands) when it is constructed
	virtual ~RpcFilter();			//!< Calls close() if necessary

	bool open(ConstString targetPortName, ConstString clientsidePortName);	//!< Creates a YARP port, connects it to the existing RPC port, and causes the filter to begin forwarding messages
																			/** \param targetPortName The name of the existing RPC port (that we want filtered).
																				\param clientsidePortName The name of the new (filtered) RPC port. */
	bool close();															//!< Stops forwarding messages, and closes the filtered port, returning the object to the state it was in just after construction

	void cutConnection(bool cut = true);	//!< Sets the cut status of the filter, which indicates whether or not the filter is forwarding messages.
											/**< If the parameter passed is \c false
											 * - calls from clients are passed to the target port
											 * - responses from the target port are passed to the calling client
											 * - no calls may be injected to the target port
											 * If the parameter passed is \c true
											 * - no calls from the clients are passed to the target port
											 * - no responses from the target port are passed to the clients
											 * - calls may be injected to the target port
											 * - if a Replier is set it will be used to send replies to calls from the clients.
											 * \param cut The new cut status.
											 */

	void setCallObserver(IObserver *o);		//!< Sets the observer to handle calls received from a client.
											/**< This observer will be informed about incoming calls regardless of the
											 * cut status of the filter.
											 * @param o The call observer to be set. May be \c NULL.
											 */

	void setResponseObserver(IObserver *o);	//!< Sets the observer to handle responses to RPC calls from clients
											/**< This observer will be informed about responses from the target port
											 * regardless of the cut status of the filter.
											 *
											 * \attention There may be more than one response to a single call!
											 * @param o The response observer to be set. May be \c NULL.
											 */
	
	void setReplier(IReplier *r);			//!< Sets the replier to send replies to a call from a client.
											/**< This replier will only be asked for replies to be passed on to the
											 * calling client when the connection is cut.
											 *
											 * \attention The validity of the responses is the sole responsibility of
											 * the replier. The filter does not carry out any validation activity
											 * whatsoever!
											 * @param r The replier to be set. May be \c NULL.
											 */

	bool injectCall(const Bottle &b);		//!< Injects a call to the target port.
											/**< The call will only be passed to the target port when the connection is
											 * cut. A set call observer is \e not informed about injected calls.
											 *
											 * \attention The validity of the call is the sole responsibility of the
											 * caller. The filter does not carry out any validation activity
											 * whatsoever!
											 * @param b The call to be injected.
											 * @return \c true if the call is passed on to the target port.
											 */

	/**
	 * \todo Implement and document this function
	 * kail says:  why? what's it for?
	 */
	//virtual bool configure(Searchable &config){ return true; }

private:
	
	yarp::os::impl::RpcFilterImpl impl;	//!< implements the core functionality of the filter	
	Port clientsidePort;				//!< the port that builds the interface to the calling clients
	bool isOpen;						//!< is the filter open?  (that is, is it forwarding messages?)
	
};
#endif
/** @} */
