// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file IReplier.h Header file for the IReplier interface.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

/** \addtogroup YarpFilter
 *	@{
 */

#ifndef IREPLIER_H_
#define IREPLIER_H_

#include <yarp/os/all.h>

namespace yarp {
	namespace os {
		class IReplier;
	}
}

/** \brief An interface for an object that replies to RPC calls when the RpcFilter is cut
 *
 * \todo Use auto_ptr to return the bottle  (I always get the error ISO C++
 * 		forbids declaration of 'auto_ptr' with no type)
 */
class yarp::os::IReplier {

public:
	virtual ~IReplier(){}						//!< Nothing special to do here
	virtual Bottle* getNextReponse() = 0;		//!< Get a reply bottle
	
};
#endif
/** @} */