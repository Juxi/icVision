// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "glass_detector.h"

// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

GlassDetector::GlassDetector() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("GlassDetector");
}

GlassDetector::~GlassDetector() { 	
}

GpImage* GlassDetector::runFilter() {	
		GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
		GpImage* node0 = node987654321->subc(-5.24657171964645);
		GpImage* node1 = InputImages[3];
		GpImage* node2 = node1->ShiftUp();
		GpImage* node3 = node2->ShiftUp();
		GpImage* node4 = node3->dilate(6);
		GpImage* node7 = node4->ShiftUp();
		GpImage* node8 = node0->exp();
		GpImage* node10 = node7->ShiftUp();
		GpImage* node11 = node10->ShiftUp();
		GpImage* node12 = node11->ShiftUp();
		GpImage* node13 = node12->ShiftUp();
		GpImage* node14 = node13; //NOP
		GpImage* node16 = node3->dilate(5);
		GpImage* node19 = node16; //NOP
		GpImage* node20 = node14->ShiftUp();
		GpImage* node21 = node19->SmoothBlur(7);
		GpImage* node23 = node8->max( node21);
		GpImage* node25 = node20->ShiftUp();
		GpImage* node27 = node25->avg( node23);
		GpImage* node35 = node27->ShiftLeft();
		GpImage* node49 = node35->ShiftUp();
		/* Uncomment this section out to see the filter running step-by-step */
		 node0->Save("traceNode0000.png");  
		node1->Save("traceNode0001.png");  
		node2->Save("traceNode0002.png");  
		node3->Save("traceNode0003.png");  
		node4->Save("traceNode0004.png");  
		node7->Save("traceNode0007.png");  
		node8->Save("traceNode0008.png");  
		node10->Save("traceNode0010.png");  
		node11->Save("traceNode0011.png");  
		node12->Save("traceNode0012.png");  
		node13->Save("traceNode0013.png");  
		node14->Save("traceNode0014.png");  
		node16->Save("traceNode0016.png");  
		node19->Save("traceNode0019.png");  
		node20->Save("traceNode0020.png");  
		node21->Save("traceNode0021.png");  
		node23->Save("traceNode0023.png");  
		node25->Save("traceNode0025.png");  
		node27->Save("traceNode0027.png");  
		node35->Save("traceNode0035.png");  
		node49->Save("traceNode0049.png");  
	/**/
		delete(node987654321);
		delete(node0);
		delete(node2);
		delete(node3);
		delete(node4);
		delete(node7);
		delete(node8);
		delete(node10);
		delete(node11);
		delete(node12);
		delete(node13);
//		delete(node14);
		delete(node16);
//		delete(node19);
		delete(node20);
		delete(node21);
		delete(node23);
		delete(node25);
		delete(node27);
		delete(node35);
		//Threshold = 189.4934
		GpImage* OutputValue = node49->threshold(189.4934f); //This line is the fitness functions thresholding
		return OutputValue;
}	
	
	
	//	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node0 = InputImages[2];
//	GpImage* node1 = node0->ShiftRight();
//	GpImage* node2 = node1->dilate(5);
//	GpImage* node3 = node2->avg(node1);
//	GpImage* node4 = node2->ShiftUp();
//	GpImage* node6 = node3->ShiftUp();
//	GpImage* node7 = node6->addc(-14.7281675003469);
//	GpImage* node12 = node7->mulc(2.04168974235654);
//	GpImage* node14 = node1->unsharpen(9);
//	GpImage* node17 = node14->avg(node12);
//	GpImage* node23 = node4->absdiff(node12);
//	GpImage* node26 = node4->laplace(9);
//	GpImage* node29 = node17->unsharpen(15);
//	GpImage* node33 = node23->dilate(1);
//	GpImage* node39 = node29->ShiftUp();
//	GpImage* node42 = node26->add(node33);
//	GpImage* node46 = node39->erode(4);
//	GpImage* node99 = node46->min(node42);
	/* Uncomment this section out to see the filter running step-by-step
	 node0.Save("traceNode0000.png");  
	 node1.Save("traceNode0001.png");  
	 node2.Save("traceNode0002.png");  
	 node3.Save("traceNode0003.png");  
	 node4.Save("traceNode0004.png");  
	 node6.Save("traceNode0006.png");  
	 node7.Save("traceNode0007.png");  
	 node12.Save("traceNode0012.png");  
	 node14.Save("traceNode0014.png");  
	 node17.Save("traceNode0017.png");  
	 node23.Save("traceNode0023.png");  
	 node26.Save("traceNode0026.png");  
	 node29.Save("traceNode0029.png");  
	 node33.Save("traceNode0033.png");  
	 node39.Save("traceNode0039.png");  
	 node42.Save("traceNode0042.png");  
	 node46.Save("traceNode0046.png");  
	 node99.Save("traceNode0099.png");  
	 */
	
	//	delete node0;
//	delete node1;
//	delete node2;
//	delete node3;
//	delete node4;
//	delete node6;
//	delete node7;
//	delete node12;
//	delete node14;
//	delete node17;
//	delete node23;
//	delete node26;
//	delete node29;
//	delete node33;
//	delete node39;
//	delete node42;
//	delete node46;
//	
//	return node99;

void GlassDetector::setUsedInputs() {
	UsedInputs.Add(0);
	UsedInputs.Add(3);
}

