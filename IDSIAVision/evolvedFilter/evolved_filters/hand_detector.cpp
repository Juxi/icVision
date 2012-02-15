// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "hand_detector.h"

using namespace yarp::os;
using namespace yarp::sig;

HandDetector::HandDetector() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("HandDetector");
}

HandDetector::~HandDetector() { 	
}

GpImage* HandDetector::runFilter() {	
	// filter 

	
	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage* node0 = InputImages[4];
	GpImage* node1 = node0->min(node0);
	GpImage* node3 = node0->ShiftRight();
	GpImage* node4 = node0->dilate(5);
	GpImage* node13 = node4->gauss(13);
	GpImage* node15 = node13->ShiftDown();
	GpImage* node27 = node3->SmoothMedian(5);
	GpImage* node43 = node1->Normalize();
	GpImage* node57 = node27->ShiftRight();
	GpImage* node65 = node57->gauss(3);
	GpImage* node89 = node15->erode(1);
	GpImage* node96 = node65->add(node43);
	GpImage* node99 = node89->max(node96);
		/* Uncomment this section out to see the filter running step-by-step
		 node0.Save("traceNode0000.png");
		 node1.Save("traceNode0001.png");
		 node3.Save("traceNode0003.png");
		 node4.Save("traceNode0004.png");
		 node13.Save("traceNode0013.png");
		 node15.Save("traceNode0015.png");
		 node27.Save("traceNode0027.png");
		 node43.Save("traceNode0043.png");
		 node57.Save("traceNode0057.png");
		 node65.Save("traceNode0065.png");
		 node89.Save("traceNode0089.png");
		 node96.Save("traceNode0096.png");
		 node99.Save("traceNode0099.png");
		 */
		delete(node987654321);
//		delete(node0);
		delete(node1);
		delete(node3);
		delete(node4);
		delete(node13);
		delete(node15);
		delete(node27);
		delete(node43);
		delete(node57);
		delete(node65);
		delete(node89);
		delete(node96);	
	
	
	GpImage* node100 = node99->thresholdInv(50);
	delete node99;
	return node100;
}

void HandDetector::setUsedInputs() {
	UsedInputs.Add(4);		
}
