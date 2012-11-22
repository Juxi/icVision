/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file ResponseObserver.cpp Implementation file for the ResponseObserver
 * class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "responseObserver.h"
#include <QtGlobal>

using namespace MoBeE;

ResponseObserver::ResponseObserver(RobotFilter* r, const int b) : IObserver(), robotFilter(r), bodyPart(b) {
	// No special action to take for construction
}

ResponseObserver::~ResponseObserver() {
	// Nothing to destruct here
}

void ResponseObserver::onDataObserved(yarp::os::Bottle &b) {
	// A matching bottle has the following structure
	//   [is] [enc] 0.0 [ok]
	// but the vocab values could also be encoded as strings
	
	if (4 == b.size() && 3 == activeCall.size()) {
		yarp::os::Value& v0 = b.get(0);
		yarp::os::ConstString v0Str = v0.asString();
		if (BOTTLE_TAG_VOCAB == v0.getCode()) {
			v0Str = yarp::os::Vocab::decode(v0.asVocab());
		}

		if (v0Str == "is") {
			yarp::os::Value& v1 = b.get(1);
			yarp::os::ConstString v1Str = v1.asString();
			if (BOTTLE_TAG_VOCAB == v1.getCode()) {
				v1Str = yarp::os::Vocab::decode(v1.asVocab());
			}

			if (v1Str == "enc") {
				yarp::os::Value& c0 = activeCall.get(0);
				yarp::os::ConstString c0Str = c0.asString();
				if (BOTTLE_TAG_VOCAB == c0.getCode()) {
					c0Str = yarp::os::Vocab::decode(c0.asVocab());
				}

				if (c0Str == "get") {
					yarp::os::Value& c1 = activeCall.get(1);
					yarp::os::ConstString c1Str = c1.asString();
					if (BOTTLE_TAG_VOCAB == c1.getCode()) {
						c1Str = yarp::os::Vocab::decode(c1.asVocab());
					}

					if (c1Str == "enc") {
						yarp::os::Value& v2 = b.get(2);
						yarp::os::Value& c2 = activeCall.get(2);
						if (BOTTLE_TAG_DOUBLE == v2.getCode() && BOTTLE_TAG_INT == c2.getCode())
						{
							emit setPosition( bodyPart, (int) c2.asInt(), (qreal) v2.asDouble() );
						}
					}
				}
			}
		}
	}
}
