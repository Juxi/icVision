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

class yarp::os::ControlBoardFilter {

public:
	ControlBoardFilter();

	virtual ~ControlBoardFilter();

	bool open(ConstString filterName, ConstString targetName);
	void close();

	void cutConnection(bool cut = true);

	void setStateObserver(IObserver *o);
	void setCommandObserver(IObserver *o);
	void setCallObserver(IObserver *o);
	void setResponseObserver(IObserver *o);
	void setReplier(IReplier *r);

	void injectState(const Bottle &b);
	void injectCommand(const Bottle &b);
	void injectCall(const Bottle &b);

private:
	StreamFilter stateFilter;
	StreamFilter commandFilter;
	RpcFilter callFilter;

	bool isOpen;
};

#endif
/** @} */
