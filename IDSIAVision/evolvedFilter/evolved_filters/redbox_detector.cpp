// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "redbox_detector.h"

// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

RedBoxDetector::RedBoxDetector() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("RedBoxDetector");
}

RedBoxDetector::~RedBoxDetector() { 
	
}

GpImage* RedBoxDetector::runFilter() {	
	// filter red block things on icub

	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage*  node3 = InputImages[0];
	GpImage* node5 = node3->Normalize();
	GpImage* node6 = node5->gauss(3);
	GpImage* node7 = InputImages[1];
	GpImage* node11 = node7->threshold(64);
	GpImage* node13 = node11->Pow(0.5);
	GpImage*  node15 = node6->absdiff(node13);
	GpImage* node19 = node7;
	GpImage* node34 = node15->Normalize();
	GpImage*  node62 = node19->absdiff(node34);
	GpImage* node98 = node62->threshold(64);
	GpImage* node99 = node98->erode(2);
	
//		/* Uncomment this section out to see the filter running step-by-step
//		 node3->Save("traceNode0003.png");  
//		 node5->Save("traceNode0005.png");  
//		 node6->Save("traceNode0006.png");  
//		 node7->Save("traceNode0007.png");  
//		 node11->Save("traceNode0011.png");  
//		 node13->Save("traceNode0013.png");  
//		 node15->Save("traceNode0015.png");  
//		 node19->Save("traceNode0019.png");  
//		 node34->Save("traceNode0034.png");  
//		 node62->Save("traceNode0062.png");  
//		 node99->Save("traceNode0099.png");  
//		 */
	delete(node987654321);
//		delete(node3);
		delete(node5);
		delete(node6);
//		delete(node7);
		delete(node11);
		delete(node13);
		delete(node15);
//		delete(node19);
		delete(node34);
		delete(node62);
		delete(node98);	

	return node99;
}

void RedBoxDetector::setUsedInputs() {
	UsedInputs.Add(0);	
	UsedInputs.Add(1);
	UsedInputs.Add(2);	
	UsedInputs.Add(3);		
}
