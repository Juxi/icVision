/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file utils.h Header file for a bunch of utility functions.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <yarp/os/all.h>

inline int minimum(int a, int b) {
	return (a < b ? a : b);
}

inline int minimum(int a, int b, int c) {
	return (a < b ? minimum(a, c) : minimum(b, c));
}

inline int maximum(int a, int b) {
	return (a > b ? a : b);
}

inline int maximum(int a, int b, int c) {
	return (a > b ? maximum(a, c) : maximum(b, c));
}

inline unsigned int argMinimum(int a, int b) {
	return (a < b ? 0 : 1);
}

inline unsigned int argMinimum(int a, int b, int c) {
	return (a < c ? argMinimum(a, b) : 1 + argMinimum(b, c));
}

inline unsigned int argMaximum(int a, int b) {
	return (a > b ? 0 : 1);
}

inline unsigned int argMaximum(int a, int b, int c) {
	return (a > c ? argMaximum(a, b) : 1 + argMaximum(b, c));
}

void rgb2hsv(int red, int green, int blue, int &hue, int& saturation,
		int &value);

void showBottle(yarp::os::Bottle& anUnknownBottle, int indentation = 0);

#endif /* UTILS_H_ */
