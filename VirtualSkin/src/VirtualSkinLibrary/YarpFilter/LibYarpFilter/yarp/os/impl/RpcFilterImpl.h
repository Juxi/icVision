// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RpcFilterImpl.h Header file for the \c RpcFilterImpl class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

/** \addtogroup YarpFilter
 *	@{
 */

#ifndef RPCFILTERIMPL_H_
#define RPCFILTERIMPL_H_

#include <yarp/os/all.h>

#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include "../IObserver.h"
#include "../IReplier.h"

namespace yarp {
	namespace os {
		namespace impl {
			class RpcFilterImpl;
		}
	}
}

/**
 * Implementation details for the \c RpcFilter class.
 * The provided services implement the inner workings of RpcFilters.
 *
 * \sa RpcFilter
 *
 * \attention This class is not intended to be used by regular users of
 * YARP!
 */
class yarp::os::impl::RpcFilterImpl: public PortReader {

public:
	/**
	 * \todo Describe state of a newly created instance
	 */
	RpcFilterImpl();

	virtual ~RpcFilterImpl();

	/**
	 * \todo Document
	 */
	bool open(ConstString targetPortName);

	/**
	 * \todo Document
	 */
	void close();

	/**
	 * Sets the cut status of the filter.
	 * @param cut The new cut status.
	 */
	void cutConnection(bool cut = true);

	/**
	 * Sets the observer to be informed when a call is received from a client.
	 * @param o The observer to be set. May be \c NULL.
	 */
	void setCallObserver(IObserver *o);

	/**
	 * Sets the observer to be informed when a response to a call is received
	 * from the target port.
	 * @param o The observer to be set. May be \c NULL.
	 */
	void setResponseObserver(IObserver *o);

	/**
	 * Sets the replier to be asked for replies to a call from a client.
	 * This replier will only be asked for replies to be passed on to the
	 * calling client when the connection is cut.
	 * @param r The replier to be set. May be \c NULL.
	 */
	void setReplier(IReplier *r);

	/**
	 * Injects a call to the target port.
	 * @param b The call to be injected.
	 * @return \c true if the call is passed on to the target port.
	 */
	bool injectCall(const Bottle &b);

	/**
	 * Implements the PortReader interface.
	 * This callback function is called whenever a call is received on the
	 * clientside port.
	 * @param connection Provides access to read the call.
	 * @return true iff the call and the corresponding reponse(s) can be read
	 * 		resp. forwarded succesfully. <em><b>This does not imply that the
	 * 		call could be processed successfully by the target</b></em>.
	 */
	virtual bool read(ConnectionReader& connection);

private:
	// a name for the connection to the target port
	static ConstString connectionName;

	// the name of the target port to which the filter will be connected
	ConstString targetPortName;

	// used internally to set up the connection to the target port
	yarp::os::impl::OutputProtocol *m_out_prot;
	yarp::os::impl::Route m_out_route;

	// cut state of the filter
	bool isCut;

	// two optional observers, one for the calls coming from the clients and
	// one for the responses from the target port
	IObserver *callObserver;
	IObserver *responseObserver;

	// an optional replier that will asked for responses to a calling client
	// iff the connection is cut
	IReplier *replier;

	// internal functions
	bool setupOutputConnection();
	bool forwardCall(Bottle call, ConnectionWriter *writer = NULL);
	bool returnResponse(Bottle response, ConnectionWriter *writer);
	
};
#endif
/** @} */
