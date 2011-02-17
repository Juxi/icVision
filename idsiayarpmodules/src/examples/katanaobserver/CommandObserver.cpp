/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file CommandObserver.cpp Implementation file for the CommandObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "CommandObserver.h"
#include "utils.h"

CommandObserver::CommandObserver(RobotInterface &r, const int b) :
	IObserver(), robot(r), branch(b) {
	// No special action to take for construction
}

CommandObserver::~CommandObserver() {
	// Nothing to destruct here
}

void CommandObserver::onDataObserved(yarp::os::Bottle &b) {
	// A matching bottle has the following structure
	//   [poss] ( 0.0 0.0 0.0 0.0 0.0 0.0 )
	if (2 != b.size()) {
		return;
	}

	if (BOTTLE_TAG_LIST > b.get(0).getCode()) {
		return;
	}

	yarp::os::Value& v0_0 = b.get(0).asList()->get(0);
	yarp::os::ConstString cmdStr;
	if (BOTTLE_TAG_VOCAB == v0_0.getCode()) {
		cmdStr = yarp::os::Vocab::decode(v0_0.asVocab());
	} else if (BOTTLE_TAG_STRING == v0_0.getCode()) {
		cmdStr = v0_0.asString();
	}

	if (cmdStr != "poss") {
		return;
	}

	yarp::os::Value& v1 = b.get(1);
	if ((BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE) == v1.getCode()) {
		yarp::os::Bottle *posList = v1.asList();
		if (6 == posList->size()) {
			printf("Set new target position: (");
			for (int i = 0; i < 6; i++) {
				double pos = posList->get(i).asDouble();
				printf(" %0f", pos);
			}
			printf(" )\n");
			fflush(stdout);
		}
	}
}
