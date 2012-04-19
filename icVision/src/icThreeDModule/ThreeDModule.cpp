// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include <yarp/os/Stamp.h>

#include "ThreeDModule.h"

using namespace yarp::os;
using namespace yarp::sig;

ThreeDModule::ThreeDModule(iCubController *ctrl_in) {
	setName("ThreeDModule");
	if(ctrl_in == NULL) {
		std::cout << "ERROR: iCub controller == NULL" << std::endl;
		exit(1);
	}

	headIdx = 0;

	iCubCtrl = ctrl_in;
	
	
	// default prefix
	std::string portPrefix = "/";
	std::string icVisionName = "icVision";

//	icVisionPortName = portPrefix + icVisionName + "/rpc:i";
//	std::cout << "Trying to connect to icVision Core at: " << icVisionPortName; // << std::endl;
//	
//	// check if icVision core is running
//	if( icVisionCoreIsAvailable() ) {
//		std::cout << " ... FOUND! " << std::endl;
//		
//		// change prefix to icVisionName for clearer overview in yarp
//		portPrefix += icVisionName + "/";
//	} else {
//		std::cout << "\nWARNING! icVision Core not found!" << std::endl;		
//	}
	
	/////////////////////////////////////
	// YARP port connections	
//	if( icVisionCoreIsAvailable() ) {
//		// trying to connect to the rpc icVision core 
//		if (! icVisionPort.open((handlerPortName + "/icVisionConnection").c_str())) {
//			std::cout << getName() << ": Unable to open port " << (handlerPortName + "/icVisionConnection").c_str() << std::endl;
//			return false;
//		}
//		// trying to connect to the rpc icVision core 
//		printf("Trying to connect to %s\n", icVisionPortName.c_str());
//		if(! yarp.connect((handlerPortName + "/icVisionConnection").c_str(), icVisionPortName.c_str()) ) {
//			std::cout << getName() << ": Unable to connect to port "; 
//			std::cout << icVisionPortName.c_str() << std::endl;
//			return false;
//		}	
//	}	
	
	posPortName  = portPrefix;
	posPortName += getName();
	posPortName += "/position:i";
	if(! posPort.open( posPortName.c_str() )){
		std::cout << "ERROR: could not connect port to " << posPortName.c_str() << "!" << std::endl;
		exit(1);
	}	
	processor = new DataProcessor(this);
	posPort.setReader(*processor);
	
}

ThreeDModule::~ThreeDModule() { 
	delete processor;
}


/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
bool ThreeDModule::configure(yarp::os::Searchable& config)
{
	
	// Parse and define all specific elements to this module
	// ...

	return true ;      // let the RFModule know everything went well
}

bool ThreeDModule::updateModule() {
	readEncoders();
	return true;
}

/*
* This is our main function. The bottle callback calls this one.
*/
Bottle& ThreeDModule::calculatePosition(Bottle &in) {
	std::cout << "Calculate Position!\ngot " << in.toString() << endl;
	Bottle *out = new Bottle();
	
	// Todo calculate the shit!
	out->append(in);
	
	return *out;
}


bool ThreeDModule::readEncoders() {
//	std::cout << "Read encoders " << std::endl;

	Stamp ts;
	Port *yarp_port;
	// Head data
	if(! iCubCtrl->head->port) return false;
	else yarp_port = iCubCtrl->head->port;			
    Bottle input;
	yarp_port->read(input);
	yarp_port->getEnvelope(ts);
	
	if (input != NULL) {
		//std::cout << "got " << input.toString().c_str() << endl;
		//std::cout << "envelope time: " << ts.getTime() << endl;
//		if(! checkTS(headStamp[headIdx].getTime(), ts.getTime(), 0.05)) {
			headIdx++; 
			headIdx = headIdx % LIST_LENGTH;
			headStamp[headIdx] = ts;
			headState[headIdx] = input;			

//			std::cout << "Testing: ";
//			for (int i = 0; i < LIST_LENGTH; i++) {
//				Bottle &b = headState[(i + headIdx) % LIST_LENGTH];
//				if(b.isNull()) break;
//				//std::cout << b.size();
//				std::cout << b.get(0).asDouble() << ", ";//.get(0);
//			}
//			std::cout << std::endl;					
//		}


//		for(int i = 0; i < input.size(); i++) {
//			if(input.get(i).isDouble() && i < HEAD_JOINTS) {
//				headjnt_pos[i] = input.get(i).asDouble();
//			}
//		}
	}

	// Torso data
	if(! iCubCtrl->torso->port) return false;
	else yarp_port = iCubCtrl->torso->port;			

	yarp_port->read(input);
	yarp_port->getEnvelope(ts);

	if (input != NULL) {
//		std::cout << "got " << input.toString().c_str() << endl;
//		std::cout << "envelope time: " << ts.getTime() << endl;		
	
		//		if(! checkTS(headStamp[headIdx].getTime(), ts.getTime(), 0.05)) {
		torsoIdx++; 
		torsoIdx = torsoIdx % LIST_LENGTH;
		torsoStamp[torsoIdx] = ts;
		torsoState[torsoIdx] = input;			
		
//		std::cout << "Testing: ";
//		for (int i = 0; i < LIST_LENGTH; i++) {
//			Bottle &b = torsoState[(i + torsoIdx) % LIST_LENGTH];
//			if(b.isNull()) break;
//			//std::cout << b.size();
//			std::cout << b.get(0).asDouble() << ", ";//.get(0);
//		}
//		std::cout << std::endl;					
		

	}

	return true;
}

bool ThreeDModule::checkTS(double TSLeft, double TSRight, double th) {
    double diff=fabs(TSLeft-TSRight);
    if(diff <th)
        return true;
    else return false;
}

	
	

