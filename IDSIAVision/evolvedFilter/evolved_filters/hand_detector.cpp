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
	
	GpImage* DefaultValue = new GpImage(0, ImageWidth, ImageHeight);
	GpImage* node0 = InputImages[0];
	GpImage* node1 = node0->avg();
	GpImage* node4 = InputImages[5];
	GpImage* node6 = node4->add( node1);
	GpImage* node10 = node6; //NOP
	GpImage* node27 = node10->Normalize();
	GpImage* node30 = node27->LocalMin(3);
	GpImage* node44 = node30->Shift(0,-6);
	GpImage* node49 = node44->dilate(7);
	/* Uncomment this section out to see the filter running step-by-step
	 node0.Save("traceNode0000.png");  
	 node1.Save("traceNode0001.png");  
	 node4.Save("traceNode0004.png");  
	 node6.Save("traceNode0006.png");  
	 node10.Save("traceNode0010.png");  
	 node27.Save("traceNode0027.png");  
	 node30.Save("traceNode0030.png");  
	 node44.Save("traceNode0044.png");  
	 node49.Save("traceNode0049.png");  
	 */
	delete(DefaultValue);
//	delete(node0);
	delete(node1);
	delete(node4);
	delete(node6);
//	delete(node10);
	delete(node27);
	delete(node30);
	delete(node44);
	//Threshold = 145.408
	GpImage* OutputValue = node49->threshold(145.408f); //This line is the fitness functions thresholding
	return OutputValue;
	
	
	
//	GpImage* DefaultValue = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node1 = DefaultValue->min(-0.5042096);
//	GpImage* node2 = InputImages[4];
//	GpImage* node3 = node2->laplace(3);
//	GpImage* node4 = node3->absdiff( node2);
//	GpImage* node5 = node1->add( node2);
//	GpImage* node6 = node4->add( node2);
//	GpImage* node7 = node6->SmoothBilateral(7);
//	GpImage* node10 = node5->LocalMax(4);
//	GpImage* node11 = node7->threshold(64);
//	GpImage* node12 = node11->gauss(7);
//	GpImage* node17 = node10->Shift(-6, 0);
//	GpImage* node19 = node12->gauss(13);
//	GpImage* node31 = node19->SmoothBilateral(11);
//	GpImage* node49 = node17->avg( node31);
//	/* Uncomment this section out to see the filter running step-by-step
//	 node0.Save("traceNode0000.png");  
//	 node1.Save("traceNode0001.png");  
//	 node2.Save("traceNode0002.png");  
//	 node3.Save("traceNode0003.png");  
//	 node4.Save("traceNode0004.png");  
//	 node5.Save("traceNode0005.png");  
//	 node6.Save("traceNode0006.png");  
//	 node7.Save("traceNode0007.png");  
//	 node10.Save("traceNode0010.png");  
//	 node11.Save("traceNode0011.png");  
//	 node12.Save("traceNode0012.png");  
//	 node17.Save("traceNode0017.png");  
//	 node19.Save("traceNode0019.png");  
//	 node31.Save("traceNode0031.png");  
//	 node49.Save("traceNode0049.png");  
//	 */
//	delete(DefaultValue);
//	delete(node1);
//	delete(node3);
//	delete(node4);
//	delete(node5);
//	delete(node6);
//	delete(node7);
//	delete(node10);
//	delete(node11);
//	delete(node12);
//	delete(node17);
//	delete(node19);
//	delete(node31);
//	//Threshold = 111.8229
//	GpImage* OutputValue = node49->thresholdInv(111.8229f); //This line is the fitness functions thresholding
//	return OutputValue;	
	
//	// filter 
//	GpImage* DefaultValue = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node0 = DefaultValue;//->Shift(0,5);
//	GpImage* node1 = node0->max( node0);
//	GpImage* node2 = InputImages[4];
//	GpImage* node3 = InputImages[4];
//	GpImage* node4 = node2->mulc(-2.65320111066103);
//	GpImage* node5 = node1->max( node3);
//	GpImage* node6 = node4->absdiff( node5);
//	GpImage* node8 = node6->SmoothBilateral(11);
//	GpImage* node9 = node8->addc(2.13409115932882);
//	GpImage* node13 = node9->mulc(1.75158553943038);
//	GpImage* node49 = node13->dilate(1);
//
//	/* Uncomment this section out to see the filter running step-by-step
//	 node0.Save("traceNode0000.png");  
//	 node1.Save("traceNode0001.png");  
//	 node2.Save("traceNode0002.png");  
//	 node3.Save("traceNode0003.png");  
//	 node4.Save("traceNode0004.png");  
//	 node5.Save("traceNode0005.png");  
//	 node6.Save("traceNode0006.png");  
//	 node8.Save("traceNode0008.png");  
//	 node9.Save("traceNode0009.png");  
//	 node13.Save("traceNode0013.png");  
//	 node49.Save("traceNode0049.png");  
//	 */
//	
//	delete(DefaultValue);
//	delete(node1);
//	delete(node4);
//	delete(node5);
//	delete(node6);
//	delete(node8);
//	delete(node9);
//	delete(node13);
//	//Threshold = 244.8114
//	GpImage* OutputValue = node49->thresholdInv(244.8114f); //This line is the fitness functions thresholding
//	return OutputValue;
//	
//	
//	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node0 = InputImages[4];
//	GpImage* node1 = node0->min(node0);
//	GpImage* node3 = node0->ShiftRight();
//	GpImage* node4 = node0->dilate(5);
//	GpImage* node13 = node4->gauss(13);
//	GpImage* node15 = node13->ShiftDown();
//	GpImage* node27 = node3->SmoothMedian(5);
//	GpImage* node43 = node1->Normalize();
//	GpImage* node57 = node27->ShiftRight();
//	GpImage* node65 = node57->gauss(3);
//	GpImage* node89 = node15->erode(1);
//	GpImage* node96 = node65->add(node43);
//	GpImage* node99 = node89->max(node96);
//		/* Uncomment this section out to see the filter running step-by-step
//		 node0.Save("traceNode0000.png");
//		 node1.Save("traceNode0001.png");
//		 node3.Save("traceNode0003.png");
//		 node4.Save("traceNode0004.png");
//		 node13.Save("traceNode0013.png");
//		 node15.Save("traceNode0015.png");
//		 node27.Save("traceNode0027.png");
//		 node43.Save("traceNode0043.png");
//		 node57.Save("traceNode0057.png");
//		 node65.Save("traceNode0065.png");
//		 node89.Save("traceNode0089.png");
//		 node96.Save("traceNode0096.png");
//		 node99.Save("traceNode0099.png");
//		 */
//		delete(node987654321);
////		delete(node0);
//		delete(node1);
//		delete(node3);
//		delete(node4);
//		delete(node13);
//		delete(node15);
//		delete(node27);
//		delete(node43);
//		delete(node57);
//		delete(node65);
//		delete(node89);
//		delete(node96);	
//	
//	
//	GpImage* node100 = node99->thresholdInv(50);
//	delete node99;
//	return node100;
}

void HandDetector::setUsedInputs() {
	UsedInputs.Add(0);		
	UsedInputs.Add(5);
}
