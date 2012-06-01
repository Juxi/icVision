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
	
	
	posPortName  = portPrefix;
	posPortName += getName();
	posPortName += "/position:i";
	
	if(! posPort.open( posPortName.c_str() )){
		std::cout << "ERROR: could not connect port to " << posPortName.c_str() << "!" << std::endl;
		exit(1);
	}	
		
	// start RPC thread
	rpcThread = new ModuleRPCThread(this);
	rpcThread->start();	
	
	// TODO remove this we don't need it anymore ... right?
//	processor = new DataProcessor(this);
//	posPort.setReader(*processor);
	
	
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

/*
 * This is our main function. The bottle callback calls this one.
 */
Bottle& ThreeDModule::calculatePosition(const Bottle &in, Stamp &stamp) {
	std::cout << "Calculate Position!\ngot " << in.toString() << endl;
	Bottle *threeDpos = new Bottle();;	
	
	double x, y, z;
	x = y = z = 27.0;
	
	Bottle *p1 = in.get(0).asList();
	Bottle *p2 = in.get(1).asList();	
	
	// calculate
	calcuatePositionUsingSimonsMethod(&x, &y, &z,
									  p1->get(0).asInt(), p1->get(1).asInt(),
									  p2->get(0).asInt(), p2->get(1).asInt(),
									  stamp );
	
	// create threeDpos Bottle
	threeDpos->addDouble(x);
	threeDpos->addDouble(y);
	threeDpos->addDouble(z);
	
	return *threeDpos;
}



bool ThreeDModule::respond(const yarp::os::Bottle& in, yarp::os::Bottle& out, yarp::os::Stamp stamp) {
//?	Stamp stamp;
	std::cout << "responding: " << in.toString() << std::endl;

	//TODO sanity check
	//...
	

	out.clear();	
	// process data "in", prepare "out"
	out.append(in);
	out.addList() = calculatePosition(in, stamp);
	
	
	// reply
	return true;
	
	
}

bool DataProcessor::read(ConnectionReader& connection) {
	Bottle in, out;
	bool ok = in.read(connection);
	std::cout << "DataProcessor read bottle "  << std::endl;	
	Stamp stamp;
//	stamp.read(connection);
//	std::cout << "DataProcessor read stamp!"  << std::endl;		
	if (!ok) return false;

	// process data "in", prepare "out"
	out.append(in);
	out.addList() = module->calculatePosition(in, stamp);

	std::cout << "DataProcessor: " << in.toString() << std::endl;

	// reply
	ConnectionWriter *returnToSender = connection.getWriter();
	if (returnToSender!=NULL) {
	  out.write(*returnToSender);
	}
	return true;
}


void ThreeDModule::calcuatePositionUsingSimonsMethod(double *retX, double *retY, double *retZ, int frame1X, int frame1Y, int frame2X, int frame2Y, Stamp stamp) {
	// HACKING !! todo rewrite, ...
	// calculate world
	
	double headjnt_pos[6], torsojnt_pos[3];
	getEncoderPositions(headjnt_pos, torsojnt_pos, stamp);
	
	double x[13] = { 
		(double) frame1X,
		(double) frame1Y,
		(double) frame2X,
		(double) frame2Y,
		headjnt_pos[0], headjnt_pos[1],headjnt_pos[2], headjnt_pos[3], headjnt_pos[4], headjnt_pos[5],
		torsojnt_pos[0], torsojnt_pos[1], torsojnt_pos[2] };
	

	double estimatedZ = -.075;	//15cm is the table right roughtly
	
	
	//	frontybacky=
	//	2.3311224 + 0.012280603*x[0] + 0.075872004*x[10] + 0.00019401088*x[0]*x[8] + cos(5.1875334 - 0.075872004*x[10] - 0.00019401088*x[0]*x[8] - 0.013261461*x[0])
	//	double estimatedX = 16.336582 + 0.1394611*x[7] + 0.15662868*x[4] - 0.11288279*x[12] - 0.018227309*x[1];
	//	double estimatedY = 2.3311224 + 0.012280603*x[0] + 0.075872004*x[10] + 0.00019401088*x[0]*x[8] + cos(5.1875334 - 0.075872004*x[10] - 0.00019401088*x[0]*x[8] - 0.013261461*x[0]);
	//1.4924586 + 43.674198/x[0] + 1.5535291*pow((28.112892/x[0]),0.11274087)*abs(13.024383 + 0.11767572*x[7] + 0.13460229*x[4] - 0.1140458*x[12] - 0.015047867*x[1]);	
//	estimatedX = 16.776382 + 0.13043526*x[7] + 0.14312536*x[4] - 0.10902537*x[12] - 0.018313933*x[1];
//	estimatedY = 1.9018469 + 0.014656176*x[0] + 0.12437823*x[10] + 0.11133261*x[8] + (0.11133261*x[8] + 0.057853397*x[0] - 15.825777)/x[12];	
//	
	
	double estimatedX = 17.453409 + 0.13380532*x[7] + 0.14905137*x[4] - 0.11798947*x[12] - 0.018469423*x[1];
	double estimatedY = (0.031274121*x[0] + 0.2344905*x[10] + 0.21543403*x[8])/pow(x[12], 0.18780835) + log(x[12]) - 2.1461744;
	
	// lala
	std::cout <<  "Prediction: " << round(estimatedX)<< ", "<< (char)(round(estimatedY)+'A') << std::endl;
	
	double CellSize = 6;
	estimatedY *= CellSize;
	estimatedX *= CellSize;
	//estimatedZ *= CellSize;
	
	double leftOffset = CellSize * 6 + 2.5;
	double forwardOffset = 17.5;

	estimatedX += forwardOffset;
	estimatedY -= leftOffset;
	
//	std::cout <<  "Prediction x/y/z(cm): "<< estimatedX << ", "<< estimatedY << ", " << estimatedZ << std::endl;	
	
//	estimatedX += 8.5;
//	estimatedY -= 6.5;
	
	estimatedX = round(estimatedX*10)/10.0;
	estimatedY = round(estimatedY*10)/10.0;	
	
	*retX = -estimatedX/100.0;
	*retY = estimatedY/100.0;
	*retZ = estimatedZ;
}

void ThreeDModule::getEncoderPositions(double *headjnt_pos, double *torsojnt_pos, Stamp stamp) {	
	mutex.wait();
		
	if(stamp.getTime() == 0) {
		std::cout << "WARNING: No timestamp found in this bottle!! check your implementation!! " << std::endl;
		stamp.update();
	}
	
	// store the current (start) indices in the circular buffer
	int tIdx = torsoIdx;
	int hIdx = headIdx;
	
	// helpers
	int idxH = -1, idxT = -1;
	int hH, hT;
	double smallestDiffH = 10000;
	double smallestDiffT = 10000;	
	double tDiff;

	// the bottles to be copied out of the circ buffer
	Bottle head, torso;
	
	// going through the circular buffer
	for(int k = 0; k < LIST_LENGTH; k++) {
		hT = (LIST_LENGTH + tIdx - k) % LIST_LENGTH;	
		hH = (LIST_LENGTH + hIdx - k) % LIST_LENGTH;
		
		tDiff = fabs(stamp.getTime() - torsoStamp[hT].getTime());
//		std::cout << stamp.getTime() << " idx: " << hT <<  " getTime: " << torsoStamp[hT].getTime() << ", diffT: "<< tDiff << std::endl;		
		if( tDiff < smallestDiffT ) {
			idxT = hT;				
			smallestDiffT = tDiff;
		}

		tDiff = fabs(stamp.getTime() - torsoStamp[hH].getTime());
		if( tDiff < smallestDiffH ) { 
			idxH = hH;		
			smallestDiffH = tDiff;
		}
	}
	
	if( idxH != -1 ) head = headState[idxH];	
	if( idxT != -1 ) torso = torsoState[idxT];
	std::cout <<  "idxT: " << idxT << "/" << idxT-tIdx <<  " val: " << torso.toString() <<  std::endl;			
	std::cout <<  "idxH: " << idxH << "/" << idxH-hIdx << " val: " << head.toString() <<  std::endl;				
	
	for(int i = 0; i < 6; i++) {
		headjnt_pos[i] = head.get(i).asDouble();
		if(i < 3) torsojnt_pos[i] = torso.get(i).asDouble();
	}
	
	mutex.post();
}

