// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "blue_detector.h"
//#include "../evolved_filter_module.h"
//#include "GpImage.h"

// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

BlueCupDetector::BlueCupDetector() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("BlueDetector");
}

BlueCupDetector::~BlueCupDetector() { 
	
}

GpImage* BlueCupDetector::runFilter() {	
	// filter blue things on icub
	GpImage* node0 = new GpImage(-8.22974890470505, ImageWidth, ImageHeight);
	GpImage* node1 = InputImages[4];
	GpImage* node2 = node0->avg(node1);
	GpImage* node4 = node2->threshold(64);
	GpImage* node6 = node4->erode(5);
	GpImage* node42 = node6->SmoothMedian(5);
	GpImage* node79 = node42->gauss(9);
	GpImage* node99 = node79->dilate(5);

	delete(node0);
//	delete(node1);
	delete(node2);
	delete(node4);
	delete(node6);
	delete(node42);
	delete(node79);

	return node99;
}
	   
void BlueCupDetector::setUsedInputs() {
	UsedInputs.Add(4);
}
