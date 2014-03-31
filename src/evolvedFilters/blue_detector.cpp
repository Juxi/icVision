// Copyright: (C) 2011-2014 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "blue_detector.h"

// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

// default constructor
BlueCupDetector::BlueCupDetector() {
	// moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("BlueDetector");

    // default MoBeE object name
	m_MoBeEObjectName = "cup1";
}

// new constructor
BlueCupDetector::BlueCupDetector(const char *name) {
	setName("BlueDetector");
    m_MoBeEObjectName = name;
}

BlueCupDetector::~BlueCupDetector() { 
	
}

icImage* BlueCupDetector::runFilter() {	
	// filter blue things on icub
	icImage* node0 = new icImage(-8.22974890470505, ImageWidth, ImageHeight);
	icImage* node1 = InputImages[4];
	icImage* node2 = node0->avg(node1);
	icImage* node4 = node2->threshold(64);
	icImage* node6 = node4->erode(5);
	icImage* node42 = node6->SmoothMedian(5);
	icImage* node79 = node42->gauss(9);
	icImage* node99 = node79->dilate(5);

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
