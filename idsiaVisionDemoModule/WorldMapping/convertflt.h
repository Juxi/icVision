#ifndef ConvertFlt_H
#define ConvertFlt_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGCONVERT 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class ConvertFlt {

public:
  ConvertFlt();
  ~ConvertFlt();

  IplImage* convert(IplImage* img);

private:

  IplImage* mp_cvoutputimg;

};


#endif


