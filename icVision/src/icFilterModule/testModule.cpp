// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "testModule.h"

using namespace yarp::os;
using namespace yarp::sig;

TestModule::TestModule() {
	char s[50];
	double n = yarp::os::Time::now();
	sprintf(s, "Test%5.2f", n);
	setName(s);
}

TestModule::~TestModule() { 
}


/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
bool TestModule::configure(yarp::os::Searchable& config)
{
	printf("Test: %s\n", getName().c_str());
	if( icFilterModule::configure(config) == false) 
		return false;
	
	// Parse and define all specific elements to this module
	// ...

	return true ;      // let the RFModule know everything went well
}


/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool TestModule::updateModule()
{
	if(! isRunning ) return false;

	if(! isReadingFileFromHDD) return false;
	
	printDebug(fileName.c_str());
	
	IplImage* rgb = cvLoadImage(fileName.c_str());
	ImageWidth = rgb->width;
	ImageHeight = rgb->height;	
	IplImage* out8 = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);
	
	// convert the of filtered image to be outputted
	// cvCvtColor(in, rgb, CV_GRAY2RGB);
	cvConvertScale(rgb, out8, 1.0, 0.0);
	
	ImageOf<PixelBgr>& rawOutput = rawOutputPort.prepare();
	rawOutput.wrapIplImage(out8);
	rawOutputPort.writeStrict();	
	
	return isRunning;
}
