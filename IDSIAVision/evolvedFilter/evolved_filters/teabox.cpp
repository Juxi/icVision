// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

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
//	GpImage *node0 = InputImages[0];
//	GpImage *node1 = node0; //NOP
//	GpImage *node2 = node1->Normalize();
//	GpImage *node3 = InputImages[3];
//	GpImage *node4 = node3->SmoothMedian(3);
//	GpImage *node5 = InputImages[2];
//	GpImage *node6 = node2->SmoothMedian(3);
//	GpImage *node9 = node6->sub(node5);
//	GpImage *node10 = node2->subc(6.36608573980629);
//	GpImage *node11 = node6->max(node10);
//	GpImage *node12 = node9->SmoothMedian(5);
//	GpImage *node14 = node12->mulc(-4.01880989968777);
//	GpImage *node16 = node14->ShiftDown();
//	GpImage *node17 = node4; //NOP
//	GpImage *node18 = node17->absdiff(node11);
//	GpImage *node19 = node16->min(node18);
//	GpImage *node21 = node19->ShiftRight();
//	GpImage *node25 = node18->mulc(7.40299927382148);
//	GpImage *node32 = node25->ResizeThenGabor(-2, 1.0 );	// changed from 0.2 to 1.0 (because of bug!)
//	GpImage *node35 = node21->ShiftDown();
//	GpImage *node41 = node35->threshold(64);
//	GpImage *node45 = node41->sub(node32);
//	GpImage *node66 = node45->dilate(5);
//	GpImage *node88 = node66->gauss(5);
//	GpImage *node99 = node88->ShiftDown();
	
	//GpImage node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage *node0 = InputImages[0];
	GpImage *node1 = InputImages[1];
	GpImage *node2 = node0->erode(1);
	GpImage *node3 = node2->ShiftDown();
	GpImage *node4 = node0->absdiff(node2);
	GpImage *node5 = node0->avg(node2);
	GpImage *node7 = node5; //NOP
	GpImage *node8 = node1->erode(3);
	GpImage *node9 = node3->sub(node8);
	GpImage *node12 = node4->add(node4);
	GpImage *node13 = node7->min(node12);
	GpImage *node16 = node13->absdiff(node9);
	GpImage *node24 = node16->sub(node9);
	GpImage *node50 = node24->gauss(15);
	GpImage *node78 = node50->gauss(15);
	GpImage *node89 = node78->threshold(64);
	GpImage *node99 = node89->gauss(13);

	
//	delete(node0);	delete(node1);
	delete(node2);
	delete(node3);
	delete(node4);
	delete(node5);//	delete(node7);
	delete(node8);
	delete(node9);
	delete(node12);
	delete(node13);
	delete(node16);
	delete(node24);
	delete(node50);
	delete(node78);
	delete(node89);
//	delete(node99);
	
	return node99;
}
	   
void TeaBoxDetector::setUsedInputs() {
	UsedInputs.Add(0);
	UsedInputs.Add(1);	
//	UsedInputs.Add(3);
}
