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

#include "ResponseObserver.h"

#include <QtGlobal>

ResponseObserver::ResponseObserver(RobotInterface &r, const int b) :
	IObserver(), robot(r), branch(b) {
	// No special action to take for construction
}

ResponseObserver::~ResponseObserver() {
	// Nothing to destruct here
}

void ResponseObserver::onDataObserved(yarp::os::Bottle &b) {
	// A matching bottle has the following structure
	//   [is] [dons] 1 [ok]
	// but the vocab values could also be encoded as strings
	if (4 == b.size()) {
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

			if (v1Str == "dons") {
				yarp::os::Value& v2 = b.get(2);
				int readyFlag = v2.asInt();
				if (1 == readyFlag) {
					yarp::os::Value& v3 = b.get(3);
					yarp::os::ConstString v3Str = v3.asString();
					if (BOTTLE_TAG_VOCAB == v3.getCode()) {
						v3Str = yarp::os::Vocab::decode(v3.asVocab());
					}

					if (v3Str == "ok") {
						printf("Reached target position set\n");
					}
				}
			}
		}
	}
}
