// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "evolved_filter_module.h"

#include "GpImage.h"

// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

EvolvedFilterModule::EvolvedFilterModule() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("EvolvedFilter");
}

EvolvedFilterModule::~EvolvedFilterModule() { 
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
	// DEBUG
	putchar('.');
//	std::cout << "DEBUG: Run filter!" << std::endl;	

	// read image from the port
	ImageOf<PixelBgr> *left_image = leftInPort.read();  // read an image
	if (left_image == NULL) { 
		std::cout << "ERROR: Could not read from port '" << leftInPort.getName() << "'!" << std::endl;
		return false;
	}
	ImageWidth  = left_image->width();
	ImageHeight = left_image->height();	
	IplImage* in = (IplImage*) left_image->getIplImage();
	
	// DEBUG
//	std::cout << "DEBUG: Got input image!" << std::endl;	
	GpImage* inputImg = new GpImage(in);
	inputImg->Save("input.png");
	
	// create input images to the filter
	createInputImages(in);

	// DEBUG test output of the creating
//	std::cout << "DEBUG: Created the GP input images!" << std::endl;		
//	int i = 0;
//	for(std::vector<GpImage*>::iterator it = InputImages.begin(); it != InputImages.end(); it++ ) {
//		std::string fileName = "input-";
//		fileName += '0' + i++;
//		fileName += ".jpg";
//		(*it)->Save(fileName);
//	}
	
	// run filter
	GpImage* filteredImg = runFilter();
		
	//DEBUG
	filteredImg->Save("output.png");
	
//	// DEBUG
//	std::cout << "DEBUG: Yarping..." << std::endl;		

	// output filtered image
	ImageOf<PixelBgr>& output = outputPort_Image.prepare();

	IplImage* gray  = cvCreateImage(cvSize(filteredImg->Image->width, filteredImg->Image->height), IPL_DEPTH_32F, 3);	
	IplImage* out8 = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_8U, 3);			
	cvCvtColor(filteredImg->Image, gray, CV_GRAY2RGB);
	cvConvertScale(gray, out8, 1.0, 0.0);
	output.wrapIplImage(out8); //output.copy ( *left_image );
    outputPort_Image.write();	
		
	// cleanup 
	cvReleaseImage(&out8);
	cvReleaseImage(&gray);	
	delete filteredImg;
	
	InputImages.clear();
	
	// return
	return true;
}

void EvolvedFilterModule::createInputImages(IplImage* in) {
	InputImages.clear();
	
//	std::cout << "DEBUG: A..." << in->width <<"," << in->height << std::endl;			
	IplImage* gray  = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);
	IplImage* red   = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);
	IplImage* green = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);
	IplImage* blue  = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);
	IplImage* h     = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);
	IplImage* s     = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);
	IplImage* v     = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 1);	

//	std::cout << "DEBUG: B..." << std::endl;			
	// to float
	IplImage* in32  = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 3);	
	cvConvertScale(in, in32, 1.0, 0.0);
	
//	std::cout << "DEBUG: C..." << std::endl;				
	// to gray
	cvCvtColor(in32, gray, CV_BGR2GRAY);
	InputImages.push_back(new GpImage(gray));
	
	// from BGR to RGB
	cvSplit(in32, blue, green, red, NULL);
	InputImages.push_back(new GpImage(red));
	InputImages.push_back(new GpImage(green));
	InputImages.push_back(new GpImage(blue));	

	// to HSV
	IplImage* hsvIn = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 3);
	cvCvtColor(in32, hsvIn, CV_BGR2HSV);
	cvSplit(hsvIn, h, s, v, NULL);

	cvReleaseImage(&hsvIn);
	cvReleaseImage(&in32);
	
	InputImages.push_back(new GpImage(h));
	InputImages.push_back(new GpImage(s));
	InputImages.push_back(new GpImage(v));	
}


GpImage* EvolvedFilterModule::runFilter() {	
	// filter blue things on icub
//		GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage* node0 = new GpImage(-8.22974890470505, ImageWidth, ImageHeight);
	GpImage* node1 = InputImages[4];
	GpImage* node2 = node0->avg(node1);
	GpImage* node4 = node2->threshold(64);
	GpImage* node6 = node4->erode(5);
	GpImage* node42 = node6->SmoothMedian(5);
	GpImage* node79 = node42->gauss(9);
	GpImage* node99 = node79->dilate(5);
//		delete(node987654321);
	delete(node0);
	delete(node1);
	delete(node2);
	delete(node4);
	delete(node6);
	delete(node42);
	delete(node79);
	return node99;
}


// filter table in sim
//GpImage* EvolvedFilterModule::runFilter() {	
////	std::cout << "DEBUG: Created the GP input images!" << std::endl;		
//	
//	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node23 = InputImages[5];
//	GpImage* node26 = InputImages[2];
//
//	GpImage* node32 = node26; //NOP
//	GpImage* node44 = node32; //NOP
//	GpImage* node49 = node23->mul(node44);
//	GpImage* node57 = node49; //NOP
//	GpImage* node99 = node57->threshold(64);
//
//	delete(node987654321);
//	delete(node23);
//	delete(node26); //	delete(node32);
//					//	delete(node44);
//	delete(node49);	// 	delete(node57);
//	
//	return node99;
//}
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
