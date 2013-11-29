// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "simulatedRobot.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

icSimulatedRobot::icSimulatedRobot() {
	setName("icSimulatedRobot");
    fileName = "_00000007.ppm";
}

icSimulatedRobot::~icSimulatedRobot() {
}

// void icFilterModule::printDebug(const char* str)
// {
// 	printf("[icVision::Filter:%s] %s\n", getName().c_str(), str);
// }

//bool icSimulatedRobot::open(yarp::os::Searchable& config) {
// 	if( ! configure(config) ) return false;
// 	std::cout << "Starting up the icVision Simulated Robot: " << getName() << std::endl;
// 	return true;
// }


bool icSimulatedRobot::interrupt() {
	std::cout << "INTERRUPTING" <<std::endl;

    leftOutPort.interrupt();
    rightOutPort.interrupt();
	//posOutputPort.interrupt();
	
	return true;
}


bool icSimulatedRobot::close() {
	std::cout << getName() << ": Closing the module... " <<std::endl;
    
	// close ports
	leftOutPort.close();
	rightOutPort.close();
	//posOutputPort.close();
	return true;
}


/*
 * Message handler. RPC and from the terminal
 */
bool icSimulatedRobot::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	// debug
	printf("GOT MESSAGE: %s\n", command.toString().c_str());
	
	// QUIT
	if( command.get(0).asString() == "quit" ||
	    command.get(0).asString() == "exit" || 
	    command.get(0).asString() == "stop" )
	{
		interrupt();
		return false;
	}
	
	reply.clear();
	reply.addString("The command is not valid! Try: quit");	

	return true;
}

bool icSimulatedRobot::open(yarp::os::Searchable& config) {
	if( ! configure(config) ) return false;
	return true;
}

/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
bool icSimulatedRobot::configure(yarp::os::Searchable& config)
{
	// Parse and define all specific elements to this module
	// ...

	// TODO add file to read from in configuration

	robotName = (config.find("robot")).toString();
	if( robotName.empty() ) {
		robotName = "icSim";
		std::cout << "WARNING! No robot name found using " << robotName << std::endl;
	}
	
//	if ( config.check("debug") ) {
//		inDebugMode = true;
//		std::cout << "DEBUG: Debug mode enabled!" << std::endl;
//	} else {
//		std::cout << "DEBUG: Debug mode disabled!" << std::endl;		
//	}

//	if ( config.check("scale") ) {
//		scalingFactor = (config.find("scale")).asDouble();
//		std::cout << "Scaling input images with factor: " << scalingFactor << std::endl;		
//	} else {
//		scalingFactor = 1.0;
//	}

    /////////////////////////////////////
    // connecting YARP ports
    
	outputPortName = "/" + robotName + "/";

	// opening (simulated) left camera port
    string appendix = "cam/left";
	if(! leftOutPort.open( 	(outputPortName + appendix).c_str() )){
		cout << getName() << ": Unable to open port " << (outputPortName + appendix) << endl;
		return false;
	}
    //    cout << getName() << ": Opening port " << (outputPortName + appendix) << endl;

    // opening (simulated) left camera port
    appendix = "cam/right";
	if(! rightOutPort.open( (outputPortName + appendix).c_str() )){
		cout << getName() << ": Unable to open port " << (outputPortName + appendix) << endl;
		return false;
	}
    //    cout << getName() << ": Opening port " << (outputPortName + appendix) << endl;

	// TODO check this
	// attachTerminal();
	// attach((Port &)rpcPort);
	return true ;      // let the RFModule know everything went well
}

double icSimulatedRobot::getPeriod()
{
	//module periodicity (seconds)
	return 0.1;	
}

//void icSimulatedRobot::runOnBothImages() {
//	runOnLeft = true;
//	runOnRight = true;
//}


/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool icSimulatedRobot::updateModule()
{
	ImageOf<PixelBgr> rawOutput;
    //printDebug(fileName.c_str());
	
    read(rawOutput, ("left" + fileName).c_str());
    leftOutPort.prepare() = rawOutput;
	leftOutPort.writeStrict();

    read(rawOutput, ("right" + fileName).c_str());
    rightOutPort.prepare() = rawOutput;
	rightOutPort.writeStrict();

//	ImageOf<PixelBgr>& rawOutput = leftOutPort.prepare();
//	rawOutput.wrapIplImage(out8);
//	leftOutPort.writeStrict();
    
	
	return true; //isRunning;
}
