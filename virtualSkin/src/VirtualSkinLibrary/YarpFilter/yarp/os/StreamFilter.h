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

class yarp::os::StreamFilter: public TypedReaderCallback<Bottle> {

public:
	StreamFilter();
	virtual ~StreamFilter();

	bool open(ConstString inPortName, ConstString outPortName);
	void close();

	void cutConnection(bool cut = true);
	void setObserver(IObserver *o);
	void injectData(const Bottle &b);

private:
	BufferedPort<Bottle> inPort;
	yarp::os::impl::StreamFilterImpl impl;
	bool isOpen;
};

#endif 
/** @} */