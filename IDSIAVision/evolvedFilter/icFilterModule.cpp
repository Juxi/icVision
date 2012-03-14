// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
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
}


double icFilterModule::getPeriod()
{
	return 0.05;//0.02;	// we need something higher than 0.0 else it is too fast?!
	// todo check this!!
	//module periodicity (seconds)
}


bool icFilterModule::open(yarp::os::Searchable& config)
{
	if( ! configure(config) ) return false;
	
	std::cout << "Starting up the icVision Filter Module" << std::endl;
	
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
	if( ! yarp::os::Network::checkNetwork() ) {
		std::cout << "No YARP server found!" << std::endl;
		return false;
	}
	
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
	
	
	std::string portPrefix = "/";
	
	// attaching a port of the same name as the module (prefixed with a /)
	// to the module so that messages received from the port are redirected to the respond method
	handlerPortName = portPrefix;
	handlerPortName += getName();
	
	if (! handlerPort.open(handlerPortName.c_str())) {
		std::cout << getName() << ": Unable to open port " << handlerPortName << std::endl;
		return false;
	}
	
	attach(handlerPort); 
	
	std::string inputPortName, serverPortName;

	if(! isReadingFileFromHDD) {
		// open the input ports from the cameras
		
		inputPortName = portPrefix;	serverPortName = "/";
		inputPortName += getName(); serverPortName += robotName;
		inputPortName += "/left";	serverPortName += "/cam/left";
		
		// trying to connect to the left camera
		if(! leftInPort.open( inputPortName.c_str() )){
			std::cout << getName() << ": Unable to open port " << inputPortName << std::endl;
			return false;
		}
		printf("Trying to connect to %s\n", inputPortName.c_str());
		if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port " << serverPortName.c_str();
			std::cout << " with " << inputPortName.c_str() << std::endl;
			return false;
		}
		
		inputPortName = portPrefix;	serverPortName = "/";
		inputPortName += getName(); serverPortName += robotName;
		inputPortName += "/right";	serverPortName += "/cam/right";
		// trying to connect to the right camera
		if(! rightInPort.open( inputPortName.c_str() )){
			std::cout << getName() << ": Unable to open port " << inputPortName << std::endl;		
			return false;
		}
		
		printf("Trying to connect to %s\n", inputPortName.c_str());
		if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port " << serverPortName.c_str();
			std::cout << " with " << inputPortName.c_str() << std::endl;
			return false;
		}
	}
	
	outputPortName  = portPrefix;
	outputPortName += getName();
	outputPortName += "/output:o";
	if(! outputPort_Image.open( outputPortName.c_str() )){
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
	
	
	// TEMP Solution TODO change
	
	// connect to straems
	
	std::string portName = "/juxi/head/state:o";
	inputPortName = "/icub/head/state:o";	
	if(! head_port.open( portName.c_str() )){
		std::cout << getName() << ": Unable to open port " << portName << std::endl;		
		return false;
	}
	
	printf("Trying to connect to %s\n", portName.c_str());
	if(! yarp.connect(inputPortName.c_str(), portName.c_str()) ) {
		std::cout << getName() << ": Unable to connect to port " << inputPortName.c_str();
		std::cout << " with " << portName.c_str() << std::endl;
		return false;
	}
	
	
	portName = "/juxi/torso/state:o";
	inputPortName = "/icub/torso/state:o";	
	if(! torso_port.open( portName.c_str() )){
		std::cout << getName() << ": Unable to open port " << portName << std::endl;
		return false;
	}
	
	printf("Trying to connect to %s\n", portName.c_str());
	if(! yarp.connect(inputPortName.c_str(), portName.c_str()) ) {
		std::cout << getName() << ": Unable to connect to port " << inputPortName.c_str();
		std::cout << " with " << portName.c_str() << std::endl;
		return false;
	}
	
	
	
	if(addToWorldModel) {
	
		// connect to rpc	
		// check whether we have F or not!! TODO
		std::string clientPortName = "/evolvedfilter";
		clientPortName += "/world-client";
		if(! port.open( clientPortName.c_str() )){
			return false;
		}
	//	
		inputPortName = "/world";	
		//	inputPortName += robotName; 
		//	inputPortName += "F/world";
		
		// trying to connect to the rpc server (world interface)
		printf("Trying to connect to %s\n", inputPortName.c_str());
		if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port "; 
			std::cout << inputPortName.c_str() << std::endl;
			return false;
		}	

	//	inputPortName = "/stereoDisparity/rpc";	
	//	//	inputPortName += robotName; 
	//	//	inputPortName += "F/world";
	//	
	//	// trying to connect to the rpc server (world interface)
	//	printf("Trying to connect to %s\n", inputPortName.c_str());
	//	if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
	//		std::cout << getName() << ": Unable to connect to port "; 
	//		std::cout << inputPortName.c_str() << std::endl;
	//		return false;
	//	}	
		
//		portIKinIn = new BufferedPort<Vector>;
//		portIKinIn->open("/cubeDetector/iKinIn");
//		Network::connect("/cubeDetector/iKinIn", "/eyeTriangulation/x:i");
//		portIKinIn->setStrict(true);
//		
//		portIKinOut = new BufferedPort<Bottle>;
//		portIKinOut->open("/cubeDetector/iKinOut");
//		Network::connect("/eyeTriangulation/X:o", "/cubeDetector/iKinOut");
//		portIKinOut->setStrict(true);
	}
	
	
	if(sendToGazeCtrl) {
		
		// connect to rpc	
		// check whether we have F or not!! TODO
		std::string clientPortName = "/evolvedfilter";
		clientPortName += "/gaze-client";
		if(! gazeportRPC.open( clientPortName.c_str() )){
			return false;
		}
		//	
		inputPortName = "/iKinGazeCtrl/head/rpc";	
		//	inputPortName += robotName; 
		//	inputPortName += "F/world";
		
		// trying to connect to the rpc server (world interface)
		printf("Trying to connect to %s\n", inputPortName.c_str());
		if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port "; 
			std::cout << inputPortName.c_str() << std::endl;
			return false;
		}	
		
		
		
		// check whether we have F or not!! TODO
		clientPortName = "/evolvedfilter";
		clientPortName += "/gaze-client-3D";
		if(! gazeportPos.open( clientPortName.c_str() )){
			return false;
		}
		//	
		inputPortName = "/iKinGazeCtrl/head/xd:i";	
		//	inputPortName += robotName; 
		//	inputPortName += "F/world";
		
		// trying to connect to the rpc server (world interface)
		printf("Trying to connect to %s\n", inputPortName.c_str());
		if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
			std::cout << getName() << ": Unable to connect to port "; 
			std::cout << inputPortName.c_str() << std::endl;
			return false;
		}	

	}
		
	
	
	
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

// TODO create world interface
// to kail's world model
bool icFilterModule::setWorldPositionOfObject(double x, double y, double z, const char *objName) {
	if(! addToWorldModel) return false;
	if(std::isnan(x) || std::isnan(y) || std::isnan(z) ) {
		std::cout << "NOT setting cup1 ISNAN!! -- " << x <<"," << y <<"," << z << std::endl;		
		return false;
	}
	
	yarp::os::Bottle cmd, response;
	
	// get information about the object from rpc
	cmd.clear();
	cmd.addString("set");
	cmd.addString(objName);
	
	if(inDebugMode)
		std::cout << "setting cup1 to : " << x <<"," << y <<"," << z << std::endl;
	
	cmd.addDouble(x);
	cmd.addDouble(y);
	cmd.addDouble(z);
	
	bool r = port.write(cmd, response);
//	std::cout << "response: " << response.toString() << std::endl;	
	return r;
}	

bool icFilterModule::send3DPositionToGazeCtrl(double x, double y, double z) {
	yarp::os::Bottle cmd, response;
	
	if(! sendToGazeCtrl) return false;
	if(std::isnan(x) || std::isnan(y) || std::isnan(z) ) {
		std::cout << "NOT setting gaze ISNAN!! -- " << x <<"," << y <<"," << z << std::endl;		
		
		cmd.clear();
		cmd.addString("set");
		cmd.addString("track");		
		cmd.addInt(0);
		return false;
	}
	
	// get information about the object from rpc
	cmd.clear();
	cmd.addString("set");
	cmd.addString("track");
	cmd.addInt(1);	
	bool r = gazeportRPC.write(cmd, response);
	if(inDebugMode) {
		std::cout << "gaze response: " << response.toString() << std::endl;	
		std::cout << "setting cup1 to : " << x <<"," << y <<"," << z << std::endl;
	}

	cmd.clear();
	cmd.addDouble(x);
	cmd.addDouble(y);
	cmd.addDouble(z);
	
	bool b = gazeportPos.write(cmd);
	if(inDebugMode)
		std::cout << "gazePos response: " << response.toString() << std::endl;	
	
	return b && r;
}	

bool icFilterModule::sendPixelPosOfObject(double x1, double y1, double x2, double y2) {
	yarp::os::Bottle cmd, response;
	
	// get information about the object from rpc
	cmd.clear();
//	std::cout << "setting cup1 to : " << x <<"," << y <<"," << z << std::endl;
	// check which image first
	
	cmd.addDouble(x2);
	cmd.addDouble(y2);
	cmd.addDouble(x1);
	cmd.addDouble(y1);
	
	bool r = port.write(cmd, response);
	std::cout << "response from 3dstereo crap-a-thingie: " << response.toString() << std::endl;	
	return r;
}	