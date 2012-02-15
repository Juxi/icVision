// Copyright: (C) 2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "icub_finger.h"

using namespace yarp::os;
using namespace yarp::sig;

iCubFingerFilter::iCubFingerFilter() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("HandDetector");
}

iCubFingerFilter::~iCubFingerFilter() { 	
}

GpImage* iCubFingerFilter::runFilter() {
		GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
		GpImage* node0 = new GpImage(-3.30217158095911, ImageWidth, ImageHeight);
		GpImage* node1 = node0->subc(6.93052005767822);
		GpImage* node2 = node1;
		GpImage* node3 = node2->unsharpen(7);
		GpImage* node4 = node3->erode(1);
		GpImage* node5 = node0->threshold(64);
		GpImage* node6 = node4->min();
		GpImage* node7 = node5->sobely(5);
		GpImage* node8 = node5->exp();
		GpImage* node9 = node6->min( node8);
		GpImage* node10 = node1->LocalAvg(2);
		GpImage* node11 = node9->LocalMin(2);
		GpImage* node14 = InputImages[6];
		GpImage* node15 = node11->mul( node10);
		GpImage*  node16 = node7->absdiff( node15);
		GpImage* node20 = InputImages[6];
		GpImage* node21 = node20->ShiftUp();
		GpImage* node25 = node21->max( node14);
		GpImage* node29 = node25->sobelx(3);
		GpImage* node32 = node25->max( node16);
		GpImage* node36 = node32->laplace(3);
		GpImage* node55 = node29->ShiftUp();
		GpImage* node57 = node36->absdiff( node55);
		GpImage* node99 = node57->gauss(15);
		delete(node987654321);
		delete(node0);
		delete(node1);
		delete(node3);
		delete(node4);
		delete(node5);
		delete(node6);
		delete(node7);
		delete(node8);
		delete(node9);
		delete(node10);
		delete(node11);
		delete(node15);
		delete(node16);
		delete(node21);
		delete(node25);
		delete(node29);
		delete(node32);
		delete(node36);
		delete(node55);
		delete(node57);
		//Threshold = 150.2654
		GpImage* OutputValue = node99->threshold(170.2654f); //This line is the fitness functions thresholding
		return OutputValue;
	
//		GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//		GpImage* node0 = InputImages[0]->subc(-4.34786148357671);
//		GpImage* node1 = InputImages[2];
//		GpImage* node3 = node1->ShiftLeft();
//		GpImage* node4 = node1->mul( node0);
//		GpImage* node5 = node3->ShiftUp();
//		GpImage* node6 = node1->exp();
//		GpImage* node9 = node5->gauss(13);
//		GpImage* node10 = node6->min( node4);
//		GpImage* node12 = node9->Normalize();
//		GpImage* node14 = node9->LocalAvg(2);
//		GpImage* node16 = node10->min();
//		GpImage* node19 = node14->sobelx(3);
//		GpImage* node20 = InputImages[2];
//		GpImage* node22 = node20->ShiftDown();
//		GpImage* node23 = node19->LocalMax(5);
//		GpImage* node24 = node16->avg( node9);
//		GpImage* node30 = node24->sub( node23);
//		GpImage* node34 = node22->Normalize();
//		GpImage* node35 = node34->avg();
//		GpImage* node37 = node12->min( node35);
//		GpImage*  node49 = node30->absdiff( node37);
		/* Uncomment this section out to see the filter running step-by-step
		 node0.Save("traceNode0000.png");  
		 node1.Save("traceNode0001.png");  
		 node3.Save("traceNode0003.png");  
		 node4.Save("traceNode0004.png");  
		 node5.Save("traceNode0005.png");  
		 node6.Save("traceNode0006.png");  
		 node9.Save("traceNode0009.png");  
		 node10.Save("traceNode0010.png");  
		 node12.Save("traceNode0012.png");  
		 node14.Save("traceNode0014.png");  
		 node16.Save("traceNode0016.png");  
		 node19.Save("traceNode0019.png");  
		 node20.Save("traceNode0020.png");  
		 node22.Save("traceNode0022.png");  
		 node23.Save("traceNode0023.png");  
		 node24.Save("traceNode0024.png");  
		 node30.Save("traceNode0030.png");  
		 node34.Save("traceNode0034.png");  
		 node35.Save("traceNode0035.png");  
		 node37.Save("traceNode0037.png");  
		 node49.Save("traceNode0049.png");  
		 */
//		delete(node987654321);
//		delete(node0);
////		delete(node1);
//		delete(node3);
//		delete(node4);
//		delete(node5);
//		delete(node6);
//		delete(node9);
//		delete(node10);
//		delete(node12);
//		delete(node14);
//		delete(node16);
//		delete(node19);
////		delete(node20);
//		delete(node22);
//		delete(node23);
//		delete(node24);
//		delete(node30);
//		delete(node34);
//		delete(node35);
//		delete(node37);
//		//Threshold = 154.5391
//		GpImage* OutputValue = node49->threshold(154.5391f); //This line is the fitness functions thresholding
//		return OutputValue;
	
	//	// filter 
////	GpImage *node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage *node0 = InputImages[0];
//	GpImage *node1 = node0->ResizeThenGabor(-8,2, 0.2 );
//	GpImage *node2 = node0->max(node1);
//	GpImage *node3 = node2->SmoothBilateral(5);
//	GpImage *node4 = node3->unsharpen(11);
//	GpImage *node12 = node4->dilate(1);
//	GpImage *node23 = node12->threshold(-2.53432965278625);
//	GpImage *node44 = node23->erode(2);
//	GpImage *node71 = node44->ShiftUp();
//	GpImage *node99 = node71->SmoothMedian(3);
//	
//	
//	/* Uncomment this section out to see the filter running step-by-step
//	 node0.Save("traceNode0000.png");  
//	 node1.Save("traceNode0001.png");  
//	 node2.Save("traceNode0002.png");  
//	 node3.Save("traceNode0003.png");  
//	 node4.Save("traceNode0004.png");  
//	 node8.Save("traceNode0008.png");  
//	 node9.Save("traceNode0009.png");  
//	 node12.Save("traceNode0012.png");  
//	 node23.Save("traceNode0023.png");  
//	 node44.Save("traceNode0044.png");  
//	 node71.Save("traceNode0071.png");  
//	 node99.Save("traceNode0099.png");  
//	 */
//	
////	delete node0;
//	delete node1;
//	delete node2;
//	delete node3;
//	delete node4;
//	delete node12;
//	delete node23;
//	delete node44;
//	delete node71;
//	
//	//Threshold = 137.6676
//	GpImage* node100 = node99->thresholdInv(137.6676);
//	delete node99;
//	return node100;
	return node99;
}

void iCubFingerFilter::setUsedInputs() {
//	UsedInputs.Add(0);		
	
	UsedInputs.Add(6);
}
