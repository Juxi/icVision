// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _TEABOX_DETECTOR_MODULE_H_
#define _TEABOX_DETECTOR_MODULE_H_

#include <string>
using namespace std;

#include <cv.h>
#include <highgui.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include "../evolved_filter_module.h"
#include "../GpImage.h"
#include "../boolList.h"

using namespace yarp::os;
using namespace yarp::sig;

class TeaBoxDetector : public EvolvedFilterModule {
protected:
	virtual void setUsedInputs();	
	
public:
	TeaBoxDetector();
	~TeaBoxDetector();
	
	GpImage* runFilter();
};

#endif
