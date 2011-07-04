// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "reaching_module.h"
#include "reaching.h"


ReachingModule::ReachingModule() : isStarted (false) {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("Reaching");
	reach = NULL;
}


ReachingModule::~ReachingModule() { 
	if(reach) delete reach;
}

double ReachingModule::getPeriod()
{
   return 2;
   //module periodicity (seconds)
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool ReachingModule::updateModule()
{
	if ( ! isStarted ) return true;
	
	// sending command and receiving response
	yarp::os::Bottle cmd;
	cmd.addString("ls");
	yarp::os::Bottle response;
	port.write(cmd, response);
	
	// does an object exist in the world
	if( response.size() > 0) {
		
		cout << "checking for new target objects .. " << response.size() << " object(s) found";
		
		// last object (by definition, should be target)
		// at least for now!!
		std::string objName = response.get(response.size() - 1).asString().c_str();
		
		// query if object is target !!!
		// TODO

		//	do it only once changes
		if( objName == lastTgtObjName ) {
			cout << ": last is the same" << endl;
			// todo check position
			return true;
		} else {
			cout << " new target found " << endl;
			reach->stopMotion();
		}
		lastTgtObjName = objName;
	

		// get information about the new object from rpc
		cmd.clear();
		cmd.addString("get");
		cmd.addString(objName.c_str());
		port.write(cmd, response);
		
		cout << objName << ": " << response.toString().c_str() << "" << endl;
				
		double x = response.get(13).asDouble();
		double y = response.get(14).asDouble();	
		double z = response.get(15).asDouble();
		
		// set the reach target!
		reach->setPosition(x, y, z);
		
		// reach->setPolicy ()... something like from top from left, which hand orientation?
		
		// reach for the target!
		if( isStarted ) reach->reachPosition();
		
	} else {
		// no object(s) found, stop active reaches ...
		lastTgtObjName = "";
		stop();
		
	}

   return true;
}


/*
* Message handler. Just echo all received messages.
*/
bool ReachingModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	std::cout << "Message Received: (echo is on)" << endl;
	if( command.get(0).asString() == "quit" || command.get(0).asString()=="exit" ) {
		close();
		return false;
	}
	
	reply.clear();

	if( command.get(0).asString() == "stop" || command.get(0).asString() == "wait" ){
		
		if( reach ) {
			isStarted = false;
			lastTgtObjName = "";
			stop();
			reply.addString("OK");
		}
		else
			reply.addString("ReachingWorker not ready yet!");
		
	} else if( command.get(0).asString() == "start" || command.get(0).asString() == "launch" ){

		if(reach) {
			isStarted = true;
			reply.addString("OK");
		}
		else
			reply.addString("ReachingWorker not ready yet!");

	} else
		reply.addString("The command is not valid");

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


void ReachingModule::stop()
{
	// stop motion if we were reaching before
	if( reach && reach->isActive() ) {
		reach->stopMotion();	// TODO // maybe go to home ?
		cout << "Stop sent to active position reaching!" << endl;		
	}
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

