// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "reaching_module.h"
#include "reaching.h"


ReachingModule::ReachingModule() {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("Reaching");
	reach = NULL;
}


ReachingModule::~ReachingModule() { 
	if(reach) delete reach;
}

double ReachingModule::getPeriod()
{
   return 5;
   //module periodicity (seconds)
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool ReachingModule::updateModule()
{
	//if (! port.) return false;

	cout << "checking for new target objects .. ";
	
	// sending command and receiving response
	yarp::os::Bottle cmd;
	cmd.addString("ls");
	yarp::os::Bottle response;
	port.write(cmd, response);
	
	// does an object exist? 
	if( response.size() > 0) {
		
		cout << response.size() << " object(s) found";
		
		// last object (by definition, should be target)
		// at least for now!!
		std::string objName = response.get(response.size() - 1).asString().c_str();
//	do it only once changes
//		if(objName == lastTgtObjName) {
//			cout << ": last is the same" << endl;
//			return true;
//		}
//		lastTgtObjName = objName;
		
		// get information about the object from rpc
		cmd.clear();
		cmd.addString("get");
		cmd.addString(objName.c_str());
		port.write(cmd, response);
		
		cout << objName << ": " << response.toString().c_str() << "" << endl;
				
		double x = response.get(13).asDouble();
		double y = response.get(14).asDouble();	
		double z = response.get(15).asDouble();
	
		reach->setPosition(x, y, z);
		reach->goToPosition();
		
	} else {
		cout << "no objects found" << endl;
		lastTgtObjName = "";
		
		// stop motion?
		reach->stopMotion();
	}


	//Use YARP port to get images
/*	if ( imageInLeft.getInputCount() > 0 &&
		 imageInRight.getInputCount() > 0	){
		// std::cout << "There are images available from the left cam... " << std::endl;
		iCubLeft = imageInLeft.read();  // read an image		
		imageLeft = cv::Mat((IplImage*)iCubLeft->getIplImage(),false);
		iCubRight = imageInRight.read();  // read an image		
		imageRight = cv::Mat((IplImage*)iCubRight->getIplImage(),false);

		// TODO
		// do something :)
		//outputImageLeft = imageLeft.clone();
		IplImage **h = Reaching->getReachingMap((IplImage*)iCubLeft->getIplImage(), (IplImage*)iCubRight->getIplImage());
		
		outputImages[0] = h[0];
		outputImages[1] = h[1];

		if(!outputImages[0].empty()) {
			//Send output images ... just place to outputImageLeft or outputImageRight what do you want to see
			yarp::sig::ImageOf<yarp::sig::PixelBgr> &imageLeftTmp = imageOutLeft.prepare();
			
			imageLeftTmp.resize(outputImages[0].cols,outputImages[0].rows);

			IplImage image_iplLeftimage = (IplImage) outputImages[0];

			cvCopy( &image_iplLeftimage, (IplImage *) imageLeftTmp.getIplImage());
			
			imageOutLeft.write();
		} else std::cout << "empty image... ?! " << std::endl;
    }
*/
   return true;
}


/*
* Message handler. Just echo all received messages.
*/
bool ReachingModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	std::cout << "Message Received: (echo is on)" << endl;
	if (command.get(0).asString() == "quit" || command.get(0).asString()=="exit"){
		close();
		return false;
	} else {
		reply.clear();
		reply.addString("The command is not valid");
	}
	return true;
}

/*
* Interrupt function.
*/
bool ReachingModule::interruptModule()
{
	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
	port.interrupt();
	handlerPort.interrupt();
	moduleOutputPort.interrupt();
	return true;
}

/*
* Configure function. Receive a previously initialized
* resource finder object. Use it to configure your module.
* Open port and attach it to message handler.
*/
//bool ReachingModule::configure(yarp::os::ResourceFinder &rf)
bool ReachingModule::configure(yarp::os::Searchable& config)
{
	/* Process all parameters from both command-line and .ini file */
	robotName = (config.find("robot")).toString();
	if ( robotName.empty() ) {
		robotName = "icubSimF";
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

	//set-up input/output ports
	inputPortName = "/" + robotName + "/world";
    string clientPortName = "/";
	clientPortName += getName();
	clientPortName += "/world-client";
	if(! port.open( clientPortName.c_str() )){
		return false;
	}
	
	outputPortName = "/";
	outputPortName += getName();
	outputPortName += "/output:o";
	if(! moduleOutputPort.open( outputPortName.c_str() )){
		return false;
	}

	// trying to connect to the rpc server (world interface)
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
		cout << getName() << ": Unable to connect to port " /*<< clientPortName.c_str() << " to "*/ << inputPortName.c_str() << endl;
		return false;
	}
	
	// instantiate the worker
	reach = new ReachingWorker(robotName);

	return true ;      // let the RFModule know everything went well
}



bool ReachingModule::open(yarp::os::Searchable& config)
{
	if( ! configure(config) ) return false;

	std::cout << "Starting the IM-CLeVeR Reaching module" << std::endl;

	return true;
}


/*
* Close function, to perform cleanup.
*/
bool ReachingModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;

	moduleOutputPort.close();
	handlerPort.close();
	port.close();
	
	delete reach;
	reach = NULL;

	return true;
}

