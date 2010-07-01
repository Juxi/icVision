/*
 *  camfeature.h
 *  camview
 *
 *  Created by Alexander Förster on 1/28/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef GRABBERFEATURE_ISDEF
#define GRABBERFEATURE_ISDEF

#include <dc1394/control.h>

#include <string>

using namespace std;

class GrabberFeature {
public:
	enum Feature {
		Brightness = DC1394_FEATURE_BRIGHTNESS,
		Exposure = DC1394_FEATURE_EXPOSURE,
		Sharpness = DC1394_FEATURE_SHARPNESS,
		WhiteBalance = DC1394_FEATURE_WHITE_BALANCE,
		Hue = DC1394_FEATURE_HUE,
		Saturation = DC1394_FEATURE_SATURATION,
		Gamma = DC1394_FEATURE_GAMMA,
		Shutter = DC1394_FEATURE_SHUTTER,
		Gain = DC1394_FEATURE_GAIN,
		Iris = DC1394_FEATURE_IRIS
	//	    DC1394_FEATURE_FOCUS,
	//	    DC1394_FEATURE_TEMPERATURE,
	//	    DC1394_FEATURE_TRIGGER,
	//	    DC1394_FEATURE_TRIGGER_DELAY,
	//	    DC1394_FEATURE_WHITE_SHADING,
	//	    DC1394_FEATURE_FRAME_RATE,
	//	    DC1394_FEATURE_ZOOM,
	//	    DC1394_FEATURE_PAN,
	//	    DC1394_FEATURE_TILT,
	//	    DC1394_FEATURE_OPTICAL_FILTER,
	//	    DC1394_FEATURE_CAPTURE_SIZE,
	//	    DC1394_FEATURE_CAPTURE_QUALITY
	};

	enum FeatureMode {
		Unknown = 0,
		Manual = DC1394_FEATURE_MODE_MANUAL,
		Auto = DC1394_FEATURE_MODE_AUTO,
		One = DC1394_FEATURE_MODE_ONE_PUSH_AUTO
	};

	enum FeatureSwitch {
		OFF = DC1394_OFF, ON = DC1394_ON
	};

	// This constructor is intended to be used for features exclusively controlled over
	// the YARP::dev::IFrameGrabberControls interface
	GrabberFeature(Feature id, bool present, double value0, double value1,
			unsigned int values);

	~GrabberFeature();

	string getName();

	Feature id;
	bool present;
	bool switchable;
	bool on;
	bool readable;
	double value0;
	double value1;
	double value2;
	double min_value;
	double max_value;
	FeatureMode mode;
	unsigned int possible_modes;
	unsigned int values;
	unsigned int setable;

private:
	static string names[10];
};

#endif
