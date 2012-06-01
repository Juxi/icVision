// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "redFilterModule.h"

using namespace yarp::os;
using namespace yarp::sig;

RedFilterModule::RedFilterModule() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("RedLocator");
}

RedFilterModule::~RedFilterModule() { 
}


/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
bool RedFilterModule::configure(yarp::os::Searchable& config)
{
	if( icFilterModule::configure(config) == false) 
		return false;
	
	// Parse and define all specific elements to this module
	// ...

	return true ;      // let the RFModule know everything went well
}


/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool RedFilterModule::updateModule()
{
	if(! isRunning ) return false;
	
	// taken from the yarp tutorials	
	ImageOf<PixelBgr> *left_image = leftInPort.read();  // read an image
	if (left_image != NULL) { // check we actually got something
		double xMean = 0;
		double yMean = 0;
		int pixCnt = 0;
		for (int x=0; x < left_image->width(); x++) {
			for (int y=0; y < left_image->height(); y++) {
				PixelBgr& pixel = left_image->pixel(x,y);
				// very simple test for redishness
				// make sure blue level exceeds blue and green by a factor
				if ( pixel.r > pixel.b * 1.5 + 10
			      && pixel.r > pixel.g * 1.5 + 10 ) {
					// there's a redish pixel at (x,y)!
					// let's find the average location of these pixels
					xMean += x;
					yMean += y;
					pixCnt++;
				}
			}
		}
		if (pixCnt > 0) {
			xMean /= pixCnt;
			yMean /= pixCnt;
		}
		if (pixCnt > 5 ){ //(left_image->width() / 20) * (left_image->height() / 20) ) {
			printf("Best guess of red target is: %g %g\n", xMean, yMean);
			Vector& target = outputPort_ImagePosition.prepare();
			target.resize(3);
			target[0] = 1;	// found something!
			target[1] = xMean;
			target[2] = yMean;
			outputPort_ImagePosition.write();
		} else {
			Vector& target = outputPort_ImagePosition.prepare();
			target.resize(3);
			target[0] = 0;	// found nothing
			target[1] = 0;
			target[2] = 0;
			outputPort_ImagePosition.write();
		}
	}
	
	return isRunning;
}


/*
* Interrupt function.
*/
//bool RedFilterModule::interruptModule()
//{
//	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
//	leftInPort.interrupt();
////	rightInPort.interrupt();	
//	outputPort_ImagePosition.interrupt();
////	moduleOutputPort.interrupt();
//	return true;
//}



//bool RedFilterModule::open(yarp::os::Searchable& config)
//{
//	if( ! configure(config) ) return false;
//
//	std::cout << "Starting the IM-CLeVeR RedLocator module" << std::endl;
//
//	return true;
//}

///*
//* Close function, to perform cleanup.
//*/
//bool RedFilterModule::close()
//{
//	std::cout << "Closing and cleaning up the module!" << std::endl;
//
////	moduleOutputPort.close();
//	leftInPort.close();
////	rightInPort.close();	
//	outputPort_ImagePosition.close();
//	
//	return true;
//}
