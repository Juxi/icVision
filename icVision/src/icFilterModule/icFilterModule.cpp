// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "icFilterModule.h"

icFilterModule::icFilterModule() {
	// by default run on one image only
	runOnRight = true;
	runOnLeft = false;
	
	// for now stream both raw and regular output
	streamRawFilterOutput = true;
	streamProcessedFilterOutput = true;	
	
	// set the yarp ports to _not_ write in the background
//	outputPort_Image.enableBackgroundWrite(false);
//	rawOutputPort.enableBackgroundWrite(false);	
	
	moduleID = -1;
	isRunning = true;
	
	shallLocaliseInThreeD  = false;
	shallNotifyGazeCtrl = false;
}


void icFilterModule::printDebug(const char* str)
{
	printf("[icVision::Filter:%s] %s\n", getName().c_str(), str);
}


double icFilterModule::getPeriod()
{
	return 0.2;	// we need something higher than 0.0 else it is too fast?!
	// todo check this!!
	//module periodicity (seconds)
}


bool icFilterModule::open(yarp::os::Searchable& config)
{
	if( ! configure(config) ) return false;
	std::cout << "Starting up the icVision Filter Module: " << getName() << std::endl;
	return true;
}

bool icFilterModule::updateModule()
{
	return isRunning;
}

bool icFilterModule::interrupt()
{
	std::cout << "INTERRUPTING" <<std::endl;
//	icVisionPort.interrupt();
	leftInPort.interrupt();		
	rightInPort.interrupt(); 	
	rawOutputPort.interrupt();
	imgOutputPort.interrupt();
	posOutputPort.interrupt();
	
	return true;
}


bool icFilterModule::close()
{
	if(moduleID != -1) 
		deregisterModuleWithCore();

	// close ports
	icVisionPort.close();
	leftInPort.close();		
	rightInPort.close(); 	
	rawOutputPort.close();
	imgOutputPort.close();
	posOutputPort.close();
	
	return true;
}

/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 // TODO look at this rf or config stuff?!rus
 */
//bool EvolvedFilterModule::configure(yarp::os::ResourceFinder &rf)
bool icFilterModule::configure(yarp::os::Searchable& config)
{
	if( ! yarp.checkNetwork() ) {
		std::cout << "No YARP server found!" << std::endl;
		return false;
	}

	// default prefix
	std::string portPrefix = "/";

	/////////////////////////////////////
	/* Process all parameters from both command-line and .ini file */
	// change the module name (usually used as portname in yarp)
	std::string hName;
	hName = (config.find("name")).toString();
	if( ! hName.empty() ) setName(hName.c_str());
	

	///////// icVision related /////////
	
	// define the icVision core name (usually part of the port the icVision core is running)
	icVisionName = (config.find("core")).toString();
	if( icVisionName.empty() ) {
		icVisionName = "icVision";
	}

	// the name of the RPC port used by the core module
	icVisionPortName = portPrefix + icVisionName + "/rpc:i";
	std::cout << "Trying to connect to icVision Core at: " << icVisionPortName; // << std::endl;
	
	// check if icVision core is running
	if( icVisionCoreIsAvailable() ) {
		std::cout << " ... FOUND! " << std::endl;
		
		// change prefix to icVisionName for clearer overview in yarp
		portPrefix += icVisionName + "/";
	} else {
		std::cout << "\nWARNING! icVision Core not found!" << std::endl;		
	}
	
	/////////////////////////////////////
	
	// using input images from yarp or file
	isReadingFileFromHDD = false;
	fileName = (config.find("readFile")).toString();
	if(! fileName.empty() ) {
		isReadingFileFromHDD = true;
		
		std::cout << "FileName specified: " << fileName << std::endl;
		std::cout << "WARNING! Running only once on this image!" << std::endl;
	}
	
	if(! isReadingFileFromHDD ) {
		
		// this one should come from the icVision core
		robotName = (config.find("robot")).toString();
		if( robotName.empty() ) {
			robotName = "icubSim";
			std::cout << "WARNING! No robot name found using " << robotName << std::endl;
		}
	
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
	
	/////////////////////////////////////
	// YARP port connections
	// attaching a port of the same name as the module
	// to the module so that messages received from the port are redirected to the respond method
	handlerPortName = portPrefix;
	handlerPortName += getName();
	
	if (! handlerPort.open(handlerPortName.c_str())) {
		std::cout << getName() << ": Unable to open port " << handlerPortName << std::endl << std::endl;
		return false;
	}
	attach(handlerPort);
	
	if( icVisionCoreIsAvailable() ) {
		// trying to connect to the rpc icVision core 
		if (! icVisionPort.open((handlerPortName + "/icVisionConnection").c_str())) {
			std::cout << getName() << ": Unable to open port " << (handlerPortName + "/icVisionConnection").c_str() << std::endl;
			return false;
		}
		
		// trying to connect to the rpc icVision core 
		printf("Trying to connect to %s\n", icVisionPortName.c_str());
		if(! yarp.connect((handlerPortName + "/icVisionConnection").c_str(), icVisionPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port "; 
			std::cout << icVisionPortName.c_str() << std::endl;
			return false;
		}	
	}	
	
	// open the input ports from the cameras
	if( ! isReadingFileFromHDD ) {
		std::string inputPortName, serverPortName;
		
		inputPortName = portPrefix;	serverPortName = "/";
		inputPortName += getName(); serverPortName += robotName;
		inputPortName += "/left";	serverPortName += "/cam/left";
		
		// trying to connect to the left camera
		if(! leftInPort.open( inputPortName.c_str() )){
			std::cout << getName() << ": Unable to open port " << inputPortName << std::endl;
			return false;
		}
		printf("Trying to connect %s ... ", inputPortName.c_str());
		if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port " << serverPortName.c_str();
			std::cout << " with " << inputPortName.c_str() << std::endl;
			return false;
		}
		printf("done!\n");

		inputPortName = portPrefix;	serverPortName = "/";
		inputPortName += getName(); serverPortName += robotName;
		inputPortName += "/right";	serverPortName += "/cam/right";
		// trying to connect to the right camera
		if(! rightInPort.open( inputPortName.c_str() )){
			std::cout << getName() << ": Unable to open port " << inputPortName << std::endl;		
			return false;
		}
		
		printf("Trying to connect to %s ...", inputPortName.c_str());
		if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port " << serverPortName.c_str();
			std::cout << " with " << inputPortName.c_str() << std::endl;
			return false;
		}
		printf("done!\n");
		
	
		outputPortName  = portPrefix;
		outputPortName += getName();
		outputPortName += "/output:o";
		if(! imgOutputPort.open( outputPortName.c_str() )){
			return false;
		}

		rawOutputPortName  = portPrefix;
		rawOutputPortName += getName();
		rawOutputPortName += "/rawoutput:o";
		if(! rawOutputPort.open( rawOutputPortName.c_str() )){
			return false;
		}
		
		outputPortName  = portPrefix;
		outputPortName += getName();
		outputPortName += "/position:o";
		if(! posOutputPort.open( outputPortName.c_str() )){
			return false;
		}
	}
	
	/////////////////////////////////////


	if(shallLocaliseInThreeD) {
		// trying to connect to the left camera
		if(! threeDPort.open((handlerPortName + "/3DCon").c_str())) {
			std::cout << getName() << ": Unable to open port " << (handlerPortName + "/3DCon").c_str() << std::endl;
			return false;
		}
		
		outputPortName = "/icVision/ThreeDModule/position:i";
		// trying to connect to the rpc icVision 3d localisation 
		printf("Trying to connect to %s\n", outputPortName.c_str());
		if(! yarp.connect((handlerPortName + "/3DCon").c_str(), outputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port "; 
			std::cout << "/icVision/ThreeDModule/position:i" << std::endl;
			return false;
		}
		
		// HACK HACK TODO check
		// connect to rpc	
		// check whether we have F or not!! TODO
		std::string clientPortName = "/evolvedfilter";
		clientPortName += "/world-client";
		if(! vSkinPort.open( clientPortName.c_str() )){
			return false;
		}
		
		outputPortName = "/MoBeE/world";	
		//	inputPortName += robotName; 
		//	inputPortName += "F/world";
		
		// trying to connect to the rpc server (world interface)
		printf("Trying to connect to %s\n", outputPortName.c_str());
		if(! yarp.connect(clientPortName.c_str(), outputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port "; 
			std::cout << outputPortName.c_str() << std::endl;
			return false;
		}	
		
	}
	
//	// TEMP Solution TODO change
//	// connect to streams
//	
//	std::string portName = "/juxi/head/state:o";
//	std::string inputPortName = "/icub/head/state:o";	
//	if(! head_port.open( portName.c_str() )){
//		std::cout << getName() << ": Unable to open port " << portName << std::endl;		
//		return false;
//	}
//	
//	printf("Trying to connect to %s\n", portName.c_str());
//	if(! yarp.connect(inputPortName.c_str(), portName.c_str()) ) {
//		std::cout << getName() << ": Unable to connect to port " << inputPortName.c_str();
//		std::cout << " with " << portName.c_str() << std::endl;
//		return false;
//	}
//	
//	
//	portName = "/juxi/torso/state:o";
//	inputPortName = "/icub/torso/state:o";	
//	if(! torso_port.open( portName.c_str() )){
//		std::cout << getName() << ": Unable to open port " << portName << std::endl;
//		return false;
//	}
//	
//	printf("Trying to connect to %s\n", portName.c_str());
//	if(! yarp.connect(inputPortName.c_str(), portName.c_str()) ) {
//		std::cout << getName() << ": Unable to connect to port " << inputPortName.c_str();
//		std::cout << " with " << portName.c_str() << std::endl;
//		return false;
//	}
	
//	portIKinIn = new BufferedPort<Vector>;
//	portIKinIn->open("/cubeDetector/iKinIn");
//	Network::connect("/cubeDetector/iKinIn", "/eyeTriangulation/x:i");
//	portIKinIn->setStrict(true);
//	
//	portIKinOut = new BufferedPort<Bottle>;
//	portIKinOut->open("/cubeDetector/iKinOut");
//	Network::connect("/eyeTriangulation/X:o", "/cubeDetector/iKinOut");
//	portIKinOut->setStrict(true);
	
	
	
//	std::string clientPortName = "/evolvedfilter";
//	clientPortName += "/gaze-client-3D";
//	if(! gazeportPos.open( clientPortName.c_str() )){
//		return false;
//	}
//	//	
//	std::string inputPortName = "/iKinGazeCtrl/head/stereo:i";	
//	
//	printf("Trying to connect to %s\n", inputPortName.c_str());
//	if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
//		std::cout << getName() << ": Unable to connect to port "; 
//		std::cout << inputPortName.c_str() << std::endl;
//		return false;
//	}	
	
	
	
	/////////////////////////////////////
	
	// also attach terminal (parsed in rsponse)
//	attachTerminal();
	
	if( icVisionCoreIsAvailable() )
		registerModuleWithCore();	
	return true ;      // let the RFModule know everything went well
}

//	Defining on which image to run the filter
void icFilterModule::runOnOneImage(int leftOrRight) {
	if(leftOrRight > RIGHT_IMAGE){
		runOnLeft = true;
		runOnRight = false;
	} else {
		runOnLeft = false;
		runOnRight = true;
	}
}

//	Defining to run the filter on both images
void icFilterModule::runOnBothImages() {
	runOnLeft = true;
	runOnRight = true;
}

bool icFilterModule::icVisionCoreIsAvailable() {
	return yarp.exists(icVisionPortName.c_str());
}

bool icFilterModule::registerModuleWithCore() {
	yarp::os::Bottle cmd, response;
	
	cmd.addString("add");		// send to icVision::core
	cmd.addString(getName());		// send to icVision::core	
	icVisionPort.write(cmd, response);	
	
	if( response.get(0).asString() == "OK" ) {
		// TODO add to do something with rest of bottle
		moduleID = response.get(1).asInt();
		return true;
	}
	
	return false;
}

bool icFilterModule::deregisterModuleWithCore() {
	std::cout << "DEregisterWithCore... " <<std::endl;

	yarp::os::Bottle cmd, response;
	
	cmd.addString("del");			// send to icVision::core
	cmd.addInt(moduleID);
	cmd.addString(getName());		// send to icVision::core	
	icVisionPort.write(cmd, response);	
	
	if( response.get(0).asString() == "OK" ) {
		// TODO add to do something with rest of bottle
		return true;
	}
	
	return false;
}


/*
 * Message handler. RPC and from the terminal
 */
bool icFilterModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
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



// TODO create world interface
// to kail's world model
bool icFilterModule::setWorldPositionOfObject(double x, double y, double z, const char *objName) {
	yarp::os::Bottle cmd, response;
	
	if(std::isnan(x) || std::isnan(y) || std::isnan(z))
		return false;
	
	// get information about the object from rpc
	cmd.clear();
	cmd.addString("set");
	cmd.addString(objName);
	
	cmd.addDouble(x);
	cmd.addDouble(y);
	cmd.addDouble(z);
	
    std::cout << "bottle: " << cmd.toString() << std::endl;
    
	bool r = vSkinPort.write(cmd, response);
	std::cout << "response: " << response.toString() << std::endl;	
	// return r;
	return true;
}	