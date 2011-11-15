// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include <time.h>	
#include "evolved_filter_module.h"

#include "GpImage.h"


// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

EvolvedFilterModule::EvolvedFilterModule() :
	inDebugMode(false)
{
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("EvolvedFilter");
	gray = red = green = blue =	h =	s = v = NULL;	
	UsedInputs.Add(4);
}

EvolvedFilterModule::~EvolvedFilterModule() { 
	
	// cleanup input files
	if( gray != NULL) {
		cvReleaseImage(&gray );		
		cvReleaseImage(&red  );		
		cvReleaseImage(&green);				
		cvReleaseImage(&blue );			
		cvReleaseImage(&h    );				
		cvReleaseImage(&s    );		
		cvReleaseImage(&v    );
		
		gray = red = green = blue =	h =	s = v = NULL;	
	}		
}


/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
//bool EvolvedFilterModule::configure(yarp::os::ResourceFinder &rf)
bool EvolvedFilterModule::configure(yarp::os::Searchable& config)
{
	/* Process all parameters from both command-line and .ini file */
	robotName = (config.find("robot")).toString();
	if( robotName.empty() ) {
		robotName = "icubSim";
		std::cout << "WARNING! No robot name found using " << robotName << std::endl;
	}
	
	if ( config.check("debug") ) {
		inDebugMode = true;
		std::cout << "DEBUG: Debug mode enabled!" << std::endl;
	} else {
		std::cout << "DEBUG: Debug mode disabled!" << std::endl;		
	}

	if ( config.check("scale") ) {
		scalingFactor = (config.find("scale")).asDouble();
	} else {
		scalingFactor = 1.0;
	}
	std::cout << "Scaling input images with factor: " << scalingFactor << std::endl;		

	
	isReadingFileFromHDD = false;
	fileName = (config.find("readFile")).toString();
	if(! fileName.empty() ) {
		// read file
		isReadingFileFromHDD = true;
		
		std::cout << "FileName specified: " << fileName << std::endl;
		std::cout << "WARNING! Running only once on this image!" << std::endl;
	}
	
	
	/*
	 * attach a port of the same name as the module (prefixed with a /) to the module
	 * so that messages received from the port are redirected to the respond method
	 */
	
	handlerPortName =  "/";
	handlerPortName += getName();         // use getName() rather than a literal
	
	if (! handlerPort.open(handlerPortName.c_str())) {
		cout << getName() << ": Unable to open port " << handlerPortName << endl;
		return false;
	}
	
	attach(handlerPort); 
	
	string inputPortName, serverPortName;
			
	inputPortName = "/";		serverPortName = "/";
	inputPortName += getName(); serverPortName += robotName;
	inputPortName += "/left";	serverPortName += "/cam/left";
//	inputPortName += "/right";	serverPortName += "/cam/right";	

	// trying to connect to the left camera
	if(! leftInPort.open( inputPortName.c_str() )){
		cout << getName() << ": Unable to open port " << inputPortName << endl;
		return false;
	}
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		cout << getName() << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << endl;
		return false;
	}

	// trying to connect to the right camera
	inputPortName = "/";		serverPortName = "/";
	inputPortName += getName(); serverPortName += robotName;
	inputPortName += "/right";	serverPortName += "/cam/right";
//	if(! rightInPort.open( inputPortName.c_str() )){
//		cout << getName() << ": Unable to open port " << inputPortName << endl;		
//		return false;
//	}
//	
//	printf("Trying to connect to %s\n", inputPortName.c_str());
//	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
//		cout << getName() << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << endl;
//		return false;
//	}
	
	
	outputPortName = "/";
	outputPortName += getName();
	outputPortName += "/output:o";
	if(! outputPort_Image.open( outputPortName.c_str() )){
		return false;
	}
	
	
	// instantiate the worker
		//reach = new ReachingWorker(robotName, partName);
	
	return true ;      // let the RFModule know everything went well
}

double EvolvedFilterModule::getPeriod()
{
   return 0.05;	// we need something higher than 0.0 else it is too fast?!
   //module periodicity (seconds)
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool EvolvedFilterModule::updateModule()
{
	if( ! inDebugMode) {
		putchar('.'); std::cout.flush();
	} else {
		std::cout << "DEBUG: Run filter!" << std::endl;			
	}

	clock_t start = clock();

	IplImage* in;
	
	if( isReadingFileFromHDD ) {
		in = cvLoadImage(fileName.c_str());
	} else {	
		// read image from the port
		ImageOf<PixelBgr> *left_image = leftInPort.read();  // read an image
		if (left_image == NULL) { 
			std::cout << "ERROR: Could not read from port '" << leftInPort.getName() << "'!" << std::endl;
			return false;
		}
		in = (IplImage*) left_image->getIplImage();
	}
	ImageWidth  = in->width * scalingFactor;
	ImageHeight = in->height * scalingFactor;	
	
	static int index = 0;
	if( inDebugMode) {
		std::cout << "DEBUG: Got input image!" << std::endl;	
		GpImage* inputImg = new GpImage(in);
		char fileIn[80];
		sprintf(fileIn, "input-frame-%05d.png", index);
		inputImg->Save(fileIn);
	}
	
	// set which images from the input we need for the filter
	this->setUsedInputs();
	
	// create input images to the filter
	createInputImages(in);

	if( inDebugMode ) {
		// DEBUG test output of the creating
		std::cout << "DEBUG: Created the GP input images!" << std::endl;		
		int i = 0;
		for(std::vector<GpImage*>::iterator it = InputImages.begin(); it != InputImages.end(); it++ ) {
			if((*it)) {
				std::string fileName = "input-";
				fileName += '0' + i++;
				fileName += ".png";
				(*it)->Save(fileName);
			}
		}
	}
	
	// run filter
	GpImage* filteredImg = this->runFilter();
		
	if( inDebugMode ) {
		//DEBUG
		filteredImg->Save("output.png");
		std::cout << "DEBUG: Now yarping..." << std::endl;		
	}

	// output filtered image
	IplImage* rgb = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_32F, 3);	
	IplImage* out8 = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);	
	IplImage* out_single = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 1);	
	
	bool wewantoverlay = false;
	bool wewanttothreshold = true;	
	if(wewantoverlay) {
		// overlay original	
		//cvAdd(InputImages[0]->Image, filteredImg->Image, InputImages[0]->Image);
		IplImage* r = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		IplImage* g = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		IplImage* b = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);

		cvCvtColor(InputImages[0]->Image, rgb, CV_GRAY2RGB);

		cvSplit(rgb, b, g, r, NULL);
		cvAdd(r, filteredImg->Image, r);
		cvMerge(b, g, r, NULL, rgb);	
		
		cvReleaseImage(&r);
		cvReleaseImage(&g);	
		cvReleaseImage(&b);	
	}else{
		cvCvtColor(filteredImg->Image, rgb, CV_GRAY2RGB);
	}
	
	cvConvertScale(rgb, out8, 1.0, 0.0);

	// if we want to threshold the output! there we go	
	if(wewanttothreshold) {
		cvThreshold(out8, out8, 250.0, 255.0, CV_THRESH_BINARY);
		
		
		// Blob detection
		cvCvtColor(out8, out_single, CV_RGB2GRAY);
		//Linked list of connected pixel sequences in a binary image
		CvSeq* seq;
		
		//Array of bounding boxes
		vector<CvRect> boxes;
		
		//Memory allocated for OpenCV function operations
		CvMemStorage* storage = cvCreateMemStorage(0);
		cvClearMemStorage(storage);
		
		//Find connected pixel sequences within a binary OpenGL image (diff), starting at the top-left corner (0,0)
		cvFindContours(out_single, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
		
		//Iterate through segments
		for(; seq; seq = seq->h_next) {
			//Find minimal bounding box for each sequence
			CvRect boundbox = cvBoundingRect(seq);
			boxes.push_back(boundbox);
		}
		
		cvReleaseMemStorage(&storage);

		if(boxes.size() > 0) {
			CvRect r = boxes[0];
			std::cout << "BOX Info: " << r.x << "," << r.y << "," << r.width << "," << r.height  << std::endl;
			CvPoint p1, p2;
			p1.x = r.x; p1.y = r.y;
			p2.x = r.x + r.width; p2.y = r.y + r.height;
			cvRectangle(out8, p1, p2, CV_RGB(255,0,0), 3, 8, 0 );
		}
		
	}

	ImageOf<PixelBgr>& output = outputPort_Image.prepare();
	output.wrapIplImage(out8); //output.copy ( *left_image );
    outputPort_Image.write();	
		
	// cleanup 
	cvReleaseImage(&out_single);	
	cvReleaseImage(&out8);
	cvReleaseImage(&rgb);	
	delete filteredImg;
	
	
	InputImages.clear();
	
	clock_t end = clock();	
	double diffms = (end-start)*1000/CLOCKS_PER_SEC;///CLOCKS_PER_SEC;	
	std::cout << "Filter ran for: " << diffms << " ms" << std::endl;
	
	// return	(when we read from HDD we only run once!)
	return !isReadingFileFromHDD;
//	return true;
}

void EvolvedFilterModule::createInputImages(IplImage* in) {
	InputImages.clear();
	
	// only initalize the memory for the images once!
	if( gray == NULL ) {
		gray  = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		red   = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		green = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		blue  = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		h     = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		s     = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		v     = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);	
	}
	
	// to float
	IplImage* in32  = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 3);	
	cvConvertScale(in, in32, 1.0, 0.0);
	
	// scale if wanted
	IplImage* in32_scaled;
	if( scalingFactor != 1.0) {
		in32_scaled = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 3);	
		cvResize(in32, in32_scaled);
	} else {
		in32_scaled = in32;
	}

	if( UsedInputs.uses(0) ) {
		// to gray
		cvCvtColor(in32_scaled, gray, CV_BGR2GRAY);
		InputImages.push_back(new GpImage(gray));
	} else {
		InputImages.push_back(NULL); // gray
	}
	

	if( UsedInputs.uses(1) || UsedInputs.uses(2) || UsedInputs.uses(3) ) {	
		// from BGR to RGB
		cvSplit(in32_scaled, blue, green, red, NULL);
		InputImages.push_back(new GpImage(red));
		InputImages.push_back(new GpImage(green));
		InputImages.push_back(new GpImage(blue));		
	} else {
		InputImages.push_back(NULL); // red
		InputImages.push_back(NULL); // green
		InputImages.push_back(NULL); // blue
	}	
	
	if( UsedInputs.uses(4) || UsedInputs.uses(5) || UsedInputs.uses(6) ) {	
		// to HSV 
		IplImage* hsvIn = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 3);
		cvCvtColor(in32_scaled, hsvIn, CV_BGR2HSV);
		cvSplit(hsvIn, h, s, v, NULL);
		cvReleaseImage(&hsvIn);
		InputImages.push_back(new GpImage(h));
		InputImages.push_back(new GpImage(s));
		InputImages.push_back(new GpImage(v));			
	} else {
		InputImages.push_back(NULL); // h
		InputImages.push_back(NULL); // s
		InputImages.push_back(NULL); // v
	}		
	
	// in32_scaled and in32 are the same if scale is 1.0
	cvReleaseImage(&in32_scaled);
	if( scalingFactor != 1.0) {
		cvReleaseImage(&in32);
	}
	
}


//GpImage* EvolvedFilterModule::runFilter() {
//	GpImage* node1  = InputImages[0];
//	GpImage* node2  = node1->subc(12.0094858501107);
//	GpImage* node3  = node1->ShiftUp();
//	GpImage* node4  = node3->max(node1);
//	GpImage* node5  = InputImages[4];
//	GpImage* node6  = node2->absdiff(node4);
//	GpImage* node7  = node5->threshold(64);
//	GpImage* node8  = node3->absdiff(node6);
//	GpImage* node9  = node7; //NOP
//	GpImage* node11 = node9->SmoothMedian(5);
//	GpImage* node12 = node8->gauss(9);
//	GpImage* node13 = node8->threshold(64);
//	GpImage* node15 = node13->erode(2);
//	GpImage* node16 = node15->mul(node11);
//	GpImage* node18 = node9; //NOP
//	GpImage* node19 = InputImages[4];
//	GpImage* node21 = node7->dilate(1);
//	GpImage* node22 = node16->ShiftRight();
//	GpImage* node24 = node22->unsharpen(9);
//	GpImage* node26 = node12->subc(2.83571027964354);
//	GpImage* node34 = node24->mul(node21);
//	GpImage* node36 = node34->SmoothMedian(5);
//	GpImage* node38 = node36->SmoothMedian(5);
//	GpImage* node39 = node38->ShiftDown();
//	GpImage* node43 = node18->sub(node39);
//	GpImage* node44 = node24->mul(node43);
//	GpImage* node46 = node44->mul(node44);
//	GpImage* node47 = node26->dilate(4);
//	GpImage* node57 = node19->dilate(3);
//	GpImage* node60 = node47->sub(node57);
//	GpImage* node71 = node46->addc(1.16124618053436);
//	GpImage* node85 = node60->threshold(64);
//	GpImage* node99 = node71->min(node85);
//	
//	/*
//	 node1.Save("traceNode0001.png");  
//	 node2.Save("traceNode0002.png");  
//	 node3.Save("traceNode0003.png");  
//	 node4.Save("traceNode0004.png");  
//	 node5.Save("traceNode0005.png");  
//	 node6.Save("traceNode0006.png");  
//	 node7.Save("traceNode0007.png");  
//	 node8.Save("traceNode0008.png");  
//	 node9.Save("traceNode0009.png");  
//	 node11.Save("traceNode0011.png");  
//	 node12.Save("traceNode0012.png");  
//	 node13.Save("traceNode0013.png");  
//	 node15.Save("traceNode0015.png");  
//	 node16.Save("traceNode0016.png");  
//	 node18.Save("traceNode0018.png");  
//	 node19.Save("traceNode0019.png"); */
//	node21->Save("traceNode0021.png"); /* 
//	 node22.Save("traceNode0022.png");  
//	 node24.Save("traceNode0024.png");  
//	 node26.Save("traceNode0026.png");  
//	 node34.Save("traceNode0034.png");  
//	 node36.Save("traceNode0036.png");  
//	 node38.Save("traceNode0038.png");  
//	 node39.Save("traceNode0039.png");  
//	 node43.Save("traceNode0043.png");  
//	 node44.Save("traceNode0044.png");  
//	 node46.Save("traceNode0046.png");  
//	 node47.Save("traceNode0047.png");  */
//	 node57->Save("traceNode0057.png"); /*
//	 node60.Save("traceNode0060.png");  
//	 node71.Save("traceNode0071.png");  
//	 node85.Save("traceNode0085.png");  
//	 node99.Save("traceNode0099.png");  
//	 */
//
//	delete node2 ;
//	delete node3 ;
//	delete node4 ;
//	delete node6 ;
//	delete node7 ;
//	delete node8 ;
//	delete node11;
//	delete node12;
//	delete node13;
//	delete node15;
//	delete node16;
//	delete node21;
//	delete node22;
//	delete node24;
//	delete node26;
//	delete node34;
//	delete node36;
//	delete node38;
//	delete node39;
//	delete node43;
//	delete node44;
//	delete node46;
//	delete node47;
//	delete node57;
//	delete node60;
//	delete node71;
//	delete node85;	
//	
//	return node99;
//}
//

// filter table in sim
GpImage* EvolvedFilterModule::runFilter() {	
//	std::cout << "DEBUG: Created the GP input images!" << std::endl;		
	
	//GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage* node23 = InputImages[5];
	GpImage* node26 = InputImages[2];

	GpImage* node32 = node26; //NOP
	GpImage* node44 = node32; //NOP
	GpImage* node49 = node23->mul(node44);
	GpImage* node57 = node49; //NOP
	GpImage* node99 = node57->threshold(64);

	//delete(node987654321);
	//delete(node23);
	//delete(node26); //	delete(node32);
					//	delete(node44);
	delete(node49);	// 	delete(node57);
	
	return node99;
}

	   
void EvolvedFilterModule::setUsedInputs() {
	UsedInputs.Add(2);
	UsedInputs.Add(5);	
}


//	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node0 = node987654321->ShiftDown();
//	GpImage* node1 = node0->sobelx(11);
//	GpImage* node2 = node1->subc(4.58097606152296);
//	GpImage* node3 = InputImages[4];
//	GpImage* node4 = node3->erode(2);
//	GpImage* node5 = node2->mul(node2);
//	GpImage* node11 = node4->erode(1);
//	GpImage* node12 = node11->avg(node5);
//	GpImage* node13 = node12->threshold(64);
//	GpImage* node14 = node13->avg(node0);
//	GpImage* node19 = node14->erode(5);
//	GpImage* node23 = node19->SmoothMedian(5);
//	GpImage* node28 = node23->dilate(4);
//	GpImage* node30 = node28->unsharpen(7);
//	GpImage* node55 = node30->gauss(7);
//	GpImage* node99 = node55->dilate(5);
	
	//	GpImage node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	//	GpImage node0 = InputImages[6];
	//	GpImage node1 = node0.add(node0);
	//	GpImage node2 = node1.dilate(1);
	//	GpImage node3 = node0.dilate(5);
	//	GpImage node5 = node3.laplace(15);
	//	GpImage node6 = node5.dilate(5);
	//	GpImage node8 = node6.Normalize();
	//	GpImage node11 = node8.subc(15.9085604557768);
	//	GpImage node14 = node11.min(node2);
	//	GpImage node27 = node14.addc(0.130839884281158);
	//	GpImage node30 = node8.subc(-3.01183433854021);
	//	GpImage node41 = node27.Normalize();
	//	GpImage node58 = node41.subc(-2.72319238772616);
	//	GpImage node69 = node30.sub(node58);
	//	GpImage node99 = node69.dilate(4);
	
	//	node99 is output
//}
/*
* Message handler. Just echo all received messages.
*/
bool EvolvedFilterModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	std::cout << "Message Received: (echo is on)" << endl;
	std::cout << command.toString() << endl;
	return true;
}

/*
* Interrupt function.
*/
bool EvolvedFilterModule::interruptModule()
{
	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
	leftInPort.interrupt();
//	rightInPort.interrupt();	
	outputPort_Image.interrupt();
//	moduleOutputPort.interrupt();
	return true;
}



bool EvolvedFilterModule::open(yarp::os::Searchable& config)
{
	if( ! configure(config) ) return false;

	std::cout << "Starting the IM-CLeVeR Evolved Filter Vision module" << std::endl;

	return true;
}

/*
* Close function, to perform cleanup.
*/
bool EvolvedFilterModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;

//	moduleOutputPort.close();
	leftInPort.close();
//	rightInPort.close();	
	outputPort_Image.close();
	
	return true;
}
