// Copyright: (C) 2011-2014 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _BLUEDETECTOR_MODULE_H_
#define _BLUEDETECTOR_MODULE_H_

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

class BlueCupDetector : public EvolvedFilterModule {
protected:
	virtual void setUsedInputs();	
	
public:
    // Constructors
    BlueCupDetector();
    BlueCupDetector(const char *name);

    // Destructor
	~BlueCupDetector();
	
	icImage* runFilter();
};

#endif
