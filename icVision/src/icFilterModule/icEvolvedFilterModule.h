// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _EVOLVEDFILTERMODULE_MODULE_H_
#define _EVOLVEDFILTERMODULE_MODULE_H_

#include "icFilterModule.h"
#include "boolList.h"
#include <yarp/os/Stamp.h>

using namespace yarp::os;
using namespace std;
using namespace yarp::sig;

class EvolvedFilterModule : public icFilterModule {
protected:
	
	static const int MAX_TRIES = 5;
	
	// the pointers to the images
	IplImage* gray;
	IplImage* red;
	IplImage* green;
	IplImage* blue;
	IplImage* h;
	IplImage* s;
	IplImage* v;
	
	// member vars
	bool m_outOfSyncWarning;
	int m_ImageWidth, m_ImageHeight;
	
	
	// for evolved filter
	boolList UsedInputs;
	virtual void setUsedInputs();	
	
	std::vector<icImage*> InputImages;
	void createInputImages(IplImage *in);
	
	// move to something else ...
//	void calculateAndSetObjectWorldPosition(CvPoint, CvPoint, CvPoint, CvPoint);
//	void readEncoderPositions();
	
	// output of position information over network
	bool writePositionBottle(const CvPoint fp1, const CvPoint fp2, Stamp outTS);
		
	// image memory containers for the buffered port 
	IplImage *rawImageToWrite, *outputImageToWrite;	
	
    bool checkTS(double TSLeft, double TSRight, double th=0.08);
	

public:
	EvolvedFilterModule();
	virtual ~EvolvedFilterModule();
	
	bool updateModule();
	
	virtual icImage* runFilter();
	
	// already defined in icfilter ... 
	// bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	
	bool interruptModule();
	bool close			();
	
};

#endif
