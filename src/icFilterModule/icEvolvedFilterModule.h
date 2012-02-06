// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _EVOLVEDFILTERMODULE_MODULE_H_
#define _EVOLVEDFILTERMODULE_MODULE_H_

#include "icFilterModule.h"
#include "boolList.h"

using namespace yarp::os;
using namespace std;
using namespace yarp::sig;

class EvolvedFilterModule : public icFilterModule {
protected:
	
	// the pointers to the images
	IplImage* gray;
	IplImage* red;
	IplImage* green;
	IplImage* blue;
	IplImage* h;
	IplImage* s;
	IplImage* v;
	
	// for evolved filter
	boolList UsedInputs;
	virtual void setUsedInputs();	
	
	std::vector<icImage*> InputImages;
	void createInputImages(IplImage *in);
	
	// move to something else ...
//	void calculateAndSetObjectWorldPosition(CvPoint, CvPoint, CvPoint, CvPoint);
//	void readEncoderPositions();
	
	// image memory containers for the buffered port 
	IplImage *rawImageToWrite, *outputImageToWrite;	
	
public:
	EvolvedFilterModule();
	virtual ~EvolvedFilterModule();
	
	bool updateModule();
	
	virtual icImage* runFilter();
	
	bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	bool interruptModule();
	bool close			();
	
};

#endif
