// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _SALIENCY_H_
#define _SALIENCY_H_

#include <iostream> 
#include <cstdlib>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "cv.h"
#include "envision_lib.h"

class SaliencyWorker {
public:
	SaliencyWorker();
	~SaliencyWorker();
	IplImage** getSaliencyMap(const IplImage*, const IplImage*);

private:
	IplImage** output_image;

};

#endif 
