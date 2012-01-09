// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file ControlBoardFilter.h Header file for the \c ControlBoardFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

/** \addtogroup YarpFilter
 *	@{
 */

#ifndef CONTROLBOARDFILTER_H_
#define CONTROLBOARDFILTER_H_

#include <yarp/os/all.h>
#include "IObserver.h"
#include "IReplier.h"
#include "StreamFilter.h"
#include "RpcFilter.h"

namespace yarp {
	namespace os {
		class ControlBoardFilter;
	}
}

/** \brief A proxy for a YARP device driver that can also filter data
 *
 *	A YARP device driver has three ports:
 *		- /targetName/state:o
 *		- /targetName/command:i
 *		- /targetName/rpc:i
 *
 *	This class creates three more ports to provide a transparent interface to the device driver:
 * \verbatim
 * 		- /filterName/state:o
 *		- /filterName/command:i
 *		- /filterName/rpc:i
 *
 * \endverbatim
 *
 *	The filter provides a mechanism to cut the connection (effectively cutting off the user from the YARP device driver)
 *	as well as inject data into the exchange between user and device driver.
 * \note Gregor's original implementation has been modified such that when the filter is cut,
 *	it still allows the state port to transmit data
 *
 *	\image html controlboardfilter.png "Control Board Filter Architecture"
 */										
class yarp::os::ControlBoardFilter {

public:
	
	ControlBoardFilter();										//!< The filter is cut (not forwarding commands) when it is constructed
	virtual ~ControlBoardFilter();								//!< Nothing special to do here

	bool open(ConstString filterName, ConstString targetName);	//!< Creates YARP ports, connects them to eachother and to a YARP device driver and causes the filter to begin forwarding messages
																/** \param targetname The name of the device driver to connect to.
																	\param filterName The base name of the new interface to the device driver. */
	void close();												//!< Stops forwarding messages and closes the filtered ports, returning the object to the state it was in just after construction

	void cutConnection(bool cut = true);						//!< Causes the filter to stop forwarding data (effectively cutting off the user from the device driver)

	void setStateObserver(IObserver *o);		//!< Set a handler for data arriving on /filterName/state:i (called each time a yarp::os::bottle arrives)
	void setCommandObserver(IObserver *o);		//!< Set a handler for data arriving on /filterName/command:i (called each time a yarp::os::bottle arrives)
	void setCallObserver(IObserver *o);			//!< Set a handler for RPC calls arriving on /filterName/rpc:i (called each time a yarp::os::bottle arrives)
	void setResponseObserver(IObserver *o);		//!< Set a handler for RPC responses arriving on /filterName/rpc:o (called each time a yarp::os::bottle arrives)
	void setReplier(IReplier *r);				//!< Set a handler that replies to RPC calls arriving on /filterName/rpc:i while the filter is cut

	void injectState(const Bottle &b);			//!< Inject a yarp::os::bottle into the state stream on /filterName/state:o
	void injectCommand(const Bottle &b);		//!< Inject a yarp::os::bottle into the command stream on /filterName/command:o
	void injectCall(const Bottle &b);			//!< Inject an RPC call (yarp::os::bottle) on /filterName/rpc:o
	
	bool isCut() { return !isOpen; }
	
	//void setWaypoint();

private:
	StreamFilter stateFilter;		//!< A filter for the state port
	StreamFilter commandFilter;		//!< A filter for the command port
	RpcFilter callFilter;			//!< A filter for the RPC port

	bool isOpen;					//!< Indicates whether or not the filter is currently forwarding messages
};
#endif
/** @} */
