// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include <yarp/os/Stamp.h>

#include "GazeModule.h"

using namespace yarp::os;
using namespace yarp::sig;

GazeModule::GazeModule() : m_gazePeriod(0.1) {
	setName("GazeModule");

	for(int i = 0; i < 4; i++) prev[i] = -100;


	// default prefix
	std::string portPrefix = "/";
	std::string icVisionName = "icVision";

	// FIX LATER
//	icVisionPortName = portPrefix + icVisionName + "/rpc:i";
//	std::cout << "Trying to connect to icVision Core at: " << icVisionPortName; // << std::endl;
	
//	// check if icVision core is running
//	if( icVisionCoreIsAvailable() ) {
//		std::cout << " ... FOUND! " << std::endl;
//		
		// change prefix to icVisionName for clearer overview in yarp
		portPrefix += icVisionName + "/";
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
	
	std::string gazeServerName;
	gazePortName  = portPrefix;			gazeServerName = "/iKinGazeCtrl";
	gazePortName += getName();
	gazePortName += "/gazeConnection";	gazeServerName += "/head/stereo:i";
	
	if(! gazePort.open( gazePortName.c_str() )){
		std::cout << "ERROR: could not connect port to " << gazePortName.c_str() << "!" << std::endl;
		exit(1);
	}	
	// trying to connect to the rpc ikin
	printf("Trying to connect to %s\n", gazeServerName.c_str());
	if(! yarp.connect(gazePortName.c_str(), gazeServerName.c_str()) )  {
		std::cout << getName() << ": Unable to connect to port "; 
		std::cout << gazeServerName.c_str() << std::endl;
		exit(1);
	}
	
	
	
	filterPortName  = portPrefix;			filterServerName = "/icVision/BlueDetector";
	filterPortName += getName();
	filterPortName += "/positionFromFilter";filterServerName += "/position:o";
	
	if(! filterPort.open( filterPortName.c_str() )){
		std::cout << "ERROR: could not connect port to " << filterPortName.c_str() << "!" << std::endl;
		exit(1);
	}	
	printf("Trying to connect to %s\n", filterServerName.c_str());
	if(! yarp.connect(filterServerName.c_str(), filterPortName.c_str() ))  {
		std::cout << getName() << ": Unable to connect to port "; 
		std::cout << filterServerName.c_str() << std::endl;
		exit(1);
	}	
		
//	// start RPC thread
//	rpcThread = new ModuleRPCThread(this);
//	rpcThread->start();		
	
	
	// set Teyes 0.3
	// bind pitch -25 5
	// bind roll -5 5
	// bind yaw -45 45
}

GazeModule::~GazeModule() { 
}


/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
bool GazeModule::configure(yarp::os::Searchable& config)
{
	
	// Parse and define all specific elements to this module
	// ...

	return true ;      // let the RFModule know everything went well
}

double GazeModule::getPeriod() {
	return m_gazePeriod;
}


bool GazeModule::updateModule() {
	std::cout << "in update " << std::endl;
	
	double WIDTH = 640.0, HEIGHT = 480.0;
	// TODO check which filter/object we are supposed to look at	

	// get position(s) from that port
	// TODO change read from /icVision/BlueDetector/output
//	Bottle *input = filterPort.read();
	Bottle *input;
	int tries = 0;
	yarp::os::Stamp stamp, now;
	now.update();	

	input = filterPort.read();	
	filterPort.getEnvelope(stamp);
	if(! checkTS(stamp.getTime(), now.getTime(), 1.0)) {
		std::cout << "stamp is not close enough" << stamp.getTime()-now.getTime();
		return true;
	}

	// quick sanity check
	if(!input) {
		std::cout << "Error! nothing received!" << std::endl;		
		return true;
	}	
	if(input->size() < 4) {
		std::cout << "Error! not enough inputs received from filter!" << std::endl;
		std::cout << "Read from filter port: " << filterServerName << " " << input->toString() << std::endl;

		return true;
	}
	if(input->size() > 4) {
		std::cout << "Warning! More than 4 inputs received from filter! Using first 4!!" << std::endl;
		std::cout << "Read from filter port: " << filterServerName << " " << input->toString() << std::endl;
	}

	// check if they are almost in the center already
//	std::cout << "prepare bottle " << std::endl;
//	first try for making it more robust
//	double cfactor = 0.1;
//	if(input->get(0).asInt() > WIDTH*(0.5-cfactor) && input->get(0).asInt() < WIDTH*(0.5+cfactor) ) {
//		std::cout << "val 0: " << input->get(0).asInt() << " is < " << WIDTH*(0.5-cfactor) << " or > " << WIDTH*(0.5+cfactor) <<std::endl;
//		return true;
//	}
//	if(input->get(1).asInt() < HEIGHT*(0.5-cfactor) && input->get(1).asInt() > HEIGHT*(0.5+cfactor) ) {
//		std::cout << "val 1: " << input->get(1).asInt() << " is < " << HEIGHT*(0.5-cfactor) << " or > " << HEIGHT*(0.5+cfactor) <<std::endl;
//	}
//	if(input->get(2).asInt() < WIDTH*(0.5-cfactor) && input->get(2).asInt() > WIDTH*(0.5+cfactor) ) {
//		std::cout << "val 2: " << input->get(2).asInt() << " is < " << WIDTH*(0.5-cfactor) << " or > " << WIDTH*(0.5+cfactor) <<std::endl;
//	}
//	if(input->get(3).asInt() < HEIGHT*(0.5-cfactor) && input->get(3).asInt() > HEIGHT*(0.5+cfactor) ) {
//		std::cout << "val 3: " << input->get(3).asInt() << " is < " << HEIGHT*(0.5-cfactor) << " or > " << HEIGHT*(0.5+cfactor) <<std::endl;
//	}
	
	//	second try for making it more robust
	bool changedEnough = false;
	for(int i = 0; i < 4; i++)
		if(abs(prev[i] - input->get(i).asInt()) > 20) changedEnough = true;

	if( changedEnough ) {

		for(int i = 0; i < 4; i++)
			prev[i] = input->get(i).asInt();

		// write to bottle
		Bottle &output = gazePort.prepare();//, response;
		output.clear();
//		output.addInt(input->get(0).asInt()/2+20);
//		output.addInt(input->get(1).asInt()/2);
//		output.addInt(input->get(2).asInt()/2+20);
//		output.addInt(input->get(3).asInt()/2);
		output.addInt(input->get(0).asInt());
		output.addInt(input->get(1).asInt());
		output.addInt(input->get(2).asInt());
		output.addInt(input->get(3).asInt());
		

		std::cout << "write bottle "  << output.toString() << std::endl;
		// send to gaze controller
		gazePort.write();
	}	
	// how to check if the gaze controller is moving?!
	
	return true;
}

bool GazeModule::checkTS(double TSLeft, double TSRight, double th) {
    double diff=fabs(TSLeft-TSRight);
    if(diff <th)
        return true;
    else return false;
}

/*
 * This is our main function. The bottle callback calls this one.
 */
//Bottle& GazeModule::calculatePosition(const Bottle &in, Stamp &stamp) {
//	std::cout << "Calculate Position!\ngot " << in.toString() << endl;
//	Bottle *Gazepos = new Bottle();;	
//	
//	double x, y, z;
//	x = y = z = 27.0;
//	
//	Bottle *p1 = in.get(0).asList();
//	Bottle *p2 = in.get(1).asList();	
//	
//	// calculate
//	calcuatePositionUsingSimonsMethod(&x, &y, &z,
//									  p1->get(0).asInt(), p1->get(1).asInt(),
//									  p2->get(0).asInt(), p2->get(1).asInt(),
//									  stamp );
//	
//	// create Gazepos Bottle
//	Gazepos->addDouble(x);
//	Gazepos->addDouble(y);
//	Gazepos->addDouble(z);
//	
//	return *Gazepos;
//}
//
//

bool GazeModule::interrupt () {
	// TODO interrupt all ports 
	return true;
}
 
bool GazeModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	// debug
	printf("GOT MESSAGE: %s\n", command.toString().c_str());
	
	// QUIT
	if( command.get(0).asString() == "set" ) {
		// PERIOD
		if( command.get(1).asString() == "per") {
			double d = command.get(2).asDouble();
			if(d > 0.005 && d < 10.0 ) {
				m_gazePeriod = d;
				reply.addString("ack");
			} else {
				reply.addString("nack");
				reply.addString("value of period not valid!");
			}
			return true;
		}
	}
	
	
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
