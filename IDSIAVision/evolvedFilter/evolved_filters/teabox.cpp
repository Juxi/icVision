// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "teabox.h"
//#include "../evolved_filter_module.h"
//#include "GpImage.h"

// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

TeaBoxDetector::TeaBoxDetector() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("TeaBox");
}

TeaBoxDetector::~TeaBoxDetector() { 
	
}

GpImage* TeaBoxDetector::runFilter() {	
//	GpImage *node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage *node0 = InputImages[0];
	GpImage *node1 = node0; //NOP
	GpImage *node2 = node1->Normalize();
	GpImage *node3 = InputImages[3];
	GpImage *node4 = node3->SmoothMedian(3);
	GpImage *node5 = InputImages[2];
	GpImage *node6 = node2->SmoothMedian(3);
	GpImage *node9 = node6->sub(node5);
	GpImage *node10 = node2->subc(6.36608573980629);
	GpImage *node11 = node6->max(node10);
	GpImage *node12 = node9->SmoothMedian(5);
	GpImage *node14 = node12->mulc(-4.01880989968777);
	GpImage *node16 = node14->ShiftDown();
	GpImage *node17 = node4; //NOP
	GpImage *node18 = node17->absdiff(node11);
	GpImage *node19 = node16->min(node18);
	GpImage *node21 = node19->ShiftRight();
	GpImage *node25 = node18->mulc(7.40299927382148);
	GpImage *node32 = node25->ResizeThenGabor(-2, 1.0 );	// changed from 0.2 to 1.0 (because of bug!)
	GpImage *node35 = node21->ShiftDown();
	GpImage *node41 = node35->threshold(64);
	GpImage *node45 = node41->sub(node32);
	GpImage *node66 = node45->dilate(5);
	GpImage *node88 = node66->gauss(5);
	GpImage *node99 = node88->ShiftDown();
	
	
	node0->Save("traceNode0000.png"); 
	node1->Save("traceNode0001.png"); 
	node2->Save("traceNode0002.png"); 
	node3->Save("traceNode0003.png"); 
	node4->Save("traceNode0004.png"); 
	node5->Save("traceNode0005.png"); 
	node6->Save("traceNode0006.png"); 
	node9->Save("traceNode0009.png"); 
	node10->Save("traceNode0010.png");
	node11->Save("traceNode0011.png");
	node12->Save("traceNode0012.png");
	node14->Save("traceNode0014.png");
	node16->Save("traceNode0016.png");
	node17->Save("traceNode0017.png");
	node18->Save("traceNode0018.png");
	node19->Save("traceNode0019.png");
	node21->Save("traceNode0021.png");
	node25->Save("traceNode0025.png");
	node32->Save("traceNode0032.png");
	node35->Save("traceNode0035.png");
	node41->Save("traceNode0041.png");
	node45->Save("traceNode0045.png");
	node66->Save("traceNode0066.png");
	node88->Save("traceNode0088.png");
	node99->Save("traceNode0099.png");

	
//	delete(node987node0);
//	delete(node0);
//	delete(node1);
	delete(node2);
//	delete(node3);
	delete(node4);
//	delete(node5);
	delete(node6);
	delete(node9);
	delete(node10);
	delete(node11);
	delete(node12);
	delete(node14);
	delete(node16);
//	delete(node17);
	delete(node18);
	delete(node19);
	delete(node21);
	delete(node25);
	delete(node32);
	delete(node35);
	delete(node41);
	delete(node45);
	delete(node66);
	delete(node88);

	return node99;
}
	   
void TeaBoxDetector::setUsedInputs() {
	UsedInputs.Add(0);
	UsedInputs.Add(2);	
	UsedInputs.Add(3);
}
