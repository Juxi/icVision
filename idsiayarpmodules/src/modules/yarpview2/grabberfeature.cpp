/*
 *  camfeature.cpp
 *  camview
 *
 *  Created by Alexander Förster on 1/28/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "grabberfeature.h"

GrabberFeature::GrabberFeature(Feature id, bool present, double value0,
		double value1, unsigned int values) :
	id(id), present(present), switchable(false), on(present),
			readable(present), value0(value0), value1(value1), value2(0.0),
			min_value(0.0), max_value(present ? 1.0 : 0.0), mode(Manual),
			possible_modes(0), values(values), setable(present) {
}

GrabberFeature::~GrabberFeature() {
}

string GrabberFeature::getName() {
	if ((unsigned int) id < (unsigned int) DC1394_FEATURE_MIN
			|| (unsigned int) id > (unsigned int) DC1394_FEATURE_MIN + 9) {
		return "";
	}
	return names[id - DC1394_FEATURE_MIN];
}

// private:
string GrabberFeature::names[] = { "BRIGHTNESS", "EXPOSURE", "SHARPNESS",
		"WHITE_BALANCE", "HUE", "SATURATION", "GAMMA", "SHUTTER", "GAIN",
		"IRIS" };
