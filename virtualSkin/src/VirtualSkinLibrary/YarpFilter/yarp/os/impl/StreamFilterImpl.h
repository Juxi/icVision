// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StreamFilterImpl.h Header file for the \c StreamFilterImpl class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

/** \addtogroup YarpFilter
 *	@{
 */

#ifndef STREAMFILTERIMPL_H_
#define STREAMFILTERIMPL_H_

#include <yarp/os/all.h>
#include "../IObserver.h"

namespace yarp {
	namespace os {
		namespace impl {
			class StreamFilterImpl;
		}
	}
}

/**
 * Implementation details for the \c StreamFilter class.
 * The provided services implement the inner workings of StreamFilters.
 *
 * \sa StreamFilter
 *
 * \attention This class is not intended to be used by regular users of
 * YARP!
 */
class yarp::os::impl::StreamFilterImpl: public TypedReaderCallback<Bottle> {

public:
	/**
	 * \todo Describe state of a newly created instance
	 */
	StreamFilterImpl();

	virtual ~StreamFilterImpl();

	/**
	 * \todo Document
	 */
	bool open(ConstString outPortName);

	/**
	 * \todo Document
	 */
	void close();

	void cutConnection(bool cut = true);
	void setObserver(IObserver *o);
	void injectData(const Bottle &b);
	virtual void onRead(Bottle &b);

private:
	BufferedPort<Bottle> outPort;
	bool isOpen;
	bool isCut;
	IObserver *observer;
};
#endif
/** @} */
