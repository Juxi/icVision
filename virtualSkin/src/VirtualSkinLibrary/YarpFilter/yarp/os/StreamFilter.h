// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */

/** @file StreamFilter.h Header file for the \c StreamFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

/** \addtogroup YarpFilter
 *	@{
 */

#ifndef STREAMFILTER_H_
#define STREAMFILTER_H_

#include <yarp/os/all.h>
#include "IObserver.h"
#include "impl/StreamFilterImpl.h"

namespace yarp {
	namespace os {
		class StreamFilter;
	}
}

/** \brief A proxy for a YARP Stream port that can also filter data
 *
 * Creates a YARP stream port to mimic the existing one and connects the two such that:
 * - Incomming data can be monitored by setting an Observer (see StreamFilterImpl).
 * - The communication between the client and the target port can be cut.
 * - Data can be injected into the stream
 *
 *	\image html streamfilter.png "Stream Filter Architecture"
 */
class yarp::os::StreamFilter: public TypedReaderCallback<Bottle> {

public:
	
	StreamFilter();				//!< The filter is cut (not forwarding commands) when it is constructed
	virtual ~StreamFilter();	//!< Calls close() if necessary

	bool open(ConstString inPortName, ConstString outPortName);	//!< Creates two YARP ports, and causes the filter to begin forwarding messages between them
																/** \param inPortName The port where data arrives
																	\param outPortName The port where data is forwarded */
	void close();												//!< Stops forwarding messages, and closes the ports, returning the object to the state it was in just after construction

	void cutConnection(bool cut = true);						//!< Controls whether or not data is forwarded from the 'in' port to the 'out' port
																/**< \param cut cut=true stops the forwarding, cut=false starts it again*/
	void setObserver(IObserver *o);								//!< Set an Observer to handle data arriving on the 'in' port
	void injectData(const Bottle &b);							//!< Injects data into the stream

private:
	BufferedPort<Bottle> inPort;								//!< The 'in' port, where data arrives
	yarp::os::impl::StreamFilterImpl impl;						//!< Implements the core functionality of the StreamFilter
	bool isOpen;												//!< The cut status of the filter
	
};
#endif 
/** @} */