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
	// filter red block things on icub

//	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage* node0 = InputImages[2];
	GpImage* node1 = node0->ShiftRight();
	GpImage* node2 = node1->dilate(5);
	GpImage* node3 = node2->avg(node1);
	GpImage* node4 = node2->ShiftUp();
	GpImage* node6 = node3->ShiftUp();
	GpImage* node7 = node6->addc(-14.7281675003469);
	GpImage* node12 = node7->mulc(2.04168974235654);
	GpImage* node14 = node1->unsharpen(9);
	GpImage* node17 = node14->avg(node12);
	GpImage* node23 = node4->absdiff(node12);
	GpImage* node26 = node4->laplace(9);
	GpImage* node29 = node17->unsharpen(15);
	GpImage* node33 = node23->dilate(1);
	GpImage* node39 = node29->ShiftUp();
	GpImage* node42 = node26->add(node33);
	GpImage* node46 = node39->erode(4);
	GpImage* node99 = node46->min(node42);
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
	delete node1;
	delete node2;
	delete node3;
	delete node4;
	delete node6;
	delete node7;
	delete node12;
	delete node14;
	delete node17;
	delete node23;
	delete node26;
	delete node29;
	delete node33;
	delete node39;
	delete node42;
	delete node46;
	
	return node99;
}

void GlassDetector::setUsedInputs() {
	UsedInputs.Add(2);	
}
