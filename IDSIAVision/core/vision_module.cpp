// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "vision_module.h"

using namespace yarp::os;
using namespace yarp::sig;

VisionModule::VisionModule() : isStarted (false) {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("Vision");
}

VisionModule::~VisionModule() { 
}


/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
//bool VisionModule::configure(yarp::os::ResourceFinder &rf)
bool VisionModule::configure(yarp::os::Searchable& config)
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
	if(! rightInPort.open( inputPortName.c_str() )){
		cout << getName() << ": Unable to open port " << inputPortName << endl;		
		return false;
	}
	
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		cout << getName() << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << endl;
		return false;
	}
	
	
	outputPortName = "/";
	outputPortName += getName();
	outputPortName += "/output:o";
	if(! moduleOutputPort.open( outputPortName.c_str() )){
		return false;
	}
	
	
	// instantiate the worker
		//reach = new ReachingWorker(robotName, partName);
	
	return true ;      // let the RFModule know everything went well
}

double VisionModule::getPeriod()
{
   return 0.0;
   //module periodicity (seconds)
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool VisionModule::updateModule()
{
//	imagePort.open("/tutorial/image/in");  // give the port a name
//	targetPort.open("/tutorial/target/out");
//	Network::connect("/icubSim/cam/left","/tutorial/image/in");
	ImageOf<PixelRgb> *left_image = leftInPort.read();  // read an image
	if (left_image != NULL) { // check we actually got something
		double xMean = 0;
		double yMean = 0;
		int pixCnt = 0;
		for (int x=0; x < left_image->width(); x++) {
			for (int y=0; y < left_image->height(); y++) {
				PixelRgb& pixel = left_image->pixel(x,y);
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
		if (pixCnt > (left_image->width() / 20) * (left_image->height() / 20) ) {
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
	return true;
}


/*
* Message handler. Just echo all received messages.
*/
bool VisionModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	std::cout << "Message Received: (echo is on)" << endl;
	std::cout << command.toString() << endl;
//	if( command.get(0).asString() == "quit" || command.get(0).asString()=="exit" ) {
//		close();
//		return false;
//	}
//
//	reply.clear();
//	reply.addString("The command is not valid! Try: start|stop|set");	
//
//	if( command.get(0).asString() == "stop" || command.get(0).asString() == "wait" ){
//		
//		if( reach ) {
//			isStarted = false;
//			lastTgtObjName = "";
//			stop();
//
//			reply.clear();
//			reply.addString("OK");
//		}
//		else {
//			reply.clear();
//			reply.addString("ReachingWorker not ready yet!");
//		}
//		
//	}
//	
//	if( command.get(0).asString() == "start" || command.get(0).asString() == "launch" ){
//
//		if(reach) {
//			isStarted = true;
//			reply.clear();
//			reply.addString("OK");
//		}
//		else {
//			reply.clear();			
//			reply.addString("ReachingWorker not ready yet!");
//		}
//
//	}
//	
//	if( command.get(0).asString() == "set"){
//		
//		if( command.get(1).asString() == "tgt" || command.get(1).asString() == "target" ) {
//			userSetTargetName = command.get(2).asString();
//			reply.clear();	
//			if( userSetTargetName == "" ) {
//				reply.addString("using latest object in the world as target!");
//			} else {
//				reply.addString(userSetTargetName.c_str());					
//				reply.addString(" is now the target!");	
//			}
//
//
//		} else 
//			
//		if( command.get(1).asString() == "offset" ) {
//			if(reach) {
//				if( command.get(2).isDouble() ) {
//					reach->setOffset( command.get(2).asDouble() );
//					reply.clear();
//					reply.addString("OK");			
//				} else {
//					reply.clear();
//					reply.addString("ERROR: Could not parse double! the syntax should be: set offset <double>");	
//				}
//			} else {
//				reply.clear();
//				reply.addString("ReachingWorker not ready yet!");	
//			}
//		} else 
//		
//		if( command.get(1).asString() == "policy" ) {
//			if(reach) {
//				
//				if( command.get(2).asString() == "above" ) {
//					reach->setPolicy( ReachingWorker::FROM_ABOVE | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");
//					
//				} else if( command.get(2).asString() == "below" ) {
//					reach->setPolicy( ReachingWorker::FROM_BELOW | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");	
//					
//				} else if( command.get(2).asString() == "left" ) {
//					reach->setPolicy( ReachingWorker::FROM_LEFT  | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");	
//					
//				} else if( command.get(2).asString() == "right" ) {
//					reach->setPolicy( ReachingWorker::FROM_RIGHT | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");	
//					
//				} else {
//					reply.clear();
//					reply.addString("ERROR: Could not parse policy! the syntax should be: set policy <above|below|left|right>");	
//				}
//			} else {
//				reply.clear();
//				reply.addString("ReachingWorker not ready yet!");	
//			}
//		} else {
//			
//			// not a correct command?!
//			reply.clear();
//			reply.addString("Could not parse command try set [tgt|policy|offset]!");	
//		}
//
//		

	return true;
}

/*
* Interrupt function.
*/
bool VisionModule::interruptModule()
{
	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
	leftInPort.interrupt();
	rightInPort.interrupt();	
	outputPort_ImagePosition.interrupt();
	moduleOutputPort.interrupt();
	return true;
}



bool VisionModule::open(yarp::os::Searchable& config)
{
	if( ! configure(config) ) return false;

	std::cout << "Starting the IM-CLeVeR Vision module" << std::endl;

	return true;
}

/*
* Close function, to perform cleanup.
*/
bool VisionModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;

	moduleOutputPort.close();
	leftInPort.close();
	rightInPort.close();	
	outputPort_ImagePosition.close();
	
	return true;
}

