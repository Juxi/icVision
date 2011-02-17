/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file utils.h Implementation file for a bunch of utility functions.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "utils.h"

void rgb2hsv(int red, int green, int blue, int &hue, int& saturation,
		int &value) {
	int max = maximum(red, green, blue);
	int min = minimum(red, green, blue);
	unsigned int maxarg = argMaximum(red, green, blue);
	unsigned int minarg = argMinimum(red, green, blue);

	// calculate hue
	if (minarg == maxarg) {
		hue = 0;
	} else if (maxarg == 0) // red
	{
		hue = 60 * (green - blue) / (max - min);
		if (hue < 0)
			hue += 360.0;
	} else if (maxarg == 1) //green
	{
		hue = 60 * (blue - red) / (max - min) + 120;
	} else // blue
	{
		hue = 60 * (red - green) / (max - min) + 240;
	}
	while (hue >= 360) {
		hue -= 360;
	}
	hue = hue * 256 / 360;

	// calculate value
	value = max;

	// calculate saturation
	if (max == 0) {
		saturation = 0;
	} else {
		saturation = 255 * (max - min) / max; // == 1-(min/max)
	}
}

void showBottle(yarp::os::Bottle& anUnknownBottle, int indentation) {
	for (int i = 0; i < anUnknownBottle.size(); i++) {
		for (int j = 0; j < indentation; j++) {
			printf(" ");
		}
		printf("[%d]: ", i);
		yarp::os::Value& element = anUnknownBottle.get(i);
		switch (element.getCode()) {
		case BOTTLE_TAG_INT:
			printf("int %d\n", element.asInt());
			break;
		case BOTTLE_TAG_DOUBLE:
			printf("float %g\n", element.asDouble());
			break;
		case BOTTLE_TAG_STRING:
			printf("string \"%s\"\n", element.asString().c_str());
			break;
		case BOTTLE_TAG_BLOB:
			printf("binary blob of length %d\n", element.asBlobLength());
			break;
		case BOTTLE_TAG_VOCAB:
			printf("vocab [%s]\n",
					yarp::os::Vocab::decode(element.asVocab()).c_str());
			break;
		default:
			if (element.isList()) {
				yarp::os::Bottle *lst = element.asList();
				printf("list of %d elements\n", lst->size());
				showBottle(*lst, indentation + 2);
			} else {
				printf("unrecognized type\n");
			}
			break;
		}
	}
}
