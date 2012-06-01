// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _DISPARITY_MODULE_H_
#define _DISPARITY_MODULE_H_

#include <string>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "../icFilterModule/icFilterModule.h"
#include "../icFilterModule/icEvolvedFilterModule.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

class DisparityModule : public EvolvedFilterModule { //: public icFilterModule {
private:

public:
	DisparityModule();
	virtual ~DisparityModule();
	
	virtual bool updateModule();
		
	bool configure		(yarp::os::Searchable& config);
	
};

#endif
