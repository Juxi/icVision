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

#ifndef IREPLIER_H_
#define IREPLIER_H_

#include <yarp/os/all.h>

namespace yarp {
	namespace os {
		class IReplier;
	}
}

/**
 * Interface that must be implemented to be able to inject responses into
 * RpcFilter s.
 *
 * \todo Use auto_ptr to return the bottle  (I always get the error ISO C++
 * 		forbids declaration of 'auto_ptr' with no type)
 */
class yarp::os::IReplier {

public:
	virtual ~IReplier(){}
	virtual Bottle* getNextReponse() = 0;
};

#endif /* IREPLIER_H_ */
