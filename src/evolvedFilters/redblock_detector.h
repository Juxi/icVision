// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _REDBLOCK_DETECTOR_MODULE_H_
#define _REDBLOCK_DETECTOR_MODULE_H_

#include <string>
using namespace std;

#include <cv.h>
#include <highgui.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include "../icFilterModule/icEvolvedFilterModule.h"
#include "../icFilterModule/icImage.h"
#include "../icFilterModule/boolList.h"

using namespace yarp::os;
using namespace yarp::sig;

class RedBlockDetector : public EvolvedFilterModule {
protected:
	virtual void setUsedInputs();	
	
public:
	RedBlockDetector();
	~RedBlockDetector();
	
	icImage* runFilter();
};

#endif
