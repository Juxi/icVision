// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "core_module.h"

using namespace yarp::os;
using namespace yarp::sig;

CoreModuleQT::CoreModuleQT() : CoreModule() {
}

CoreModuleQT::~CoreModuleQT() { 
}

/*
* Message handler. RPC and from the terminal
*/
bool CoreModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	printDebug("Message Received: (echo is on)");
	
	// debug
	printf("GOT MESSAGE: %s\n", command.toString().c_str());
	
	
	// QUIT
	if( command.get(0).asString() == "quit" ||
	    command.get(0).asString() == "exit" || 
	    command.get(0).asString() == "stop" )
	{
		isRunning = false;
		return false;
	}
	
	reply.clear();
	reply.addString("The command is not valid! Try: quit|list|add|del");	

	// nothing there
	if( command.size() < 1 ) return true;
	if( command.size() == 1 && command.get(0).asString() == "") return true;


	// LIST
	if( command.get(0).asString() == "list" || command.get(0).asString() == "ls" ){
		
		reply.clear();
		reply.addString("list");
		
		std::vector<ModuleInfo>::iterator itr;
		for ( itr = listOfModules.begin(); itr != listOfModules.end(); ++itr ) {
			std::cout << (*itr).toStdString() << std::endl;

			Bottle b;
			(*itr).toBottle(b);
			
			reply.addList() = b;
		}
		
	}
	
	// ADDING A MODULE
	if( command.get(0).asString() == "add" || command.get(0).asString() == "launch" ){
		reply.clear();		
		
		if( command.size() < 2 ) {
			reply.addString("ERROR: The syntax should be:");
			reply.addString("add <name>");	
			return true;
		} 
		
		int thisModuleID = nextModuleID++;
		
		ModuleInfo i;
		i.set(thisModuleID, command);
		listOfModules.push_back(i);
		
		reply.addString("OK");
		reply.addInt(thisModuleID);
	}

	// DELETING A MODULE
	if(command.get(0).asString() == "del" ||
	   command.get(0).asString() == "rm"  ||
	   command.get(0).asString() == "delete") {
		reply.clear();		
		
		if( command.size() < 2 ) {
			reply.addString("ERROR: The syntax should be:");
			reply.addString("del <moduleID>");	
			return true;
		} 
		
	    if( command.get(1).isInt() ) {
		   int thisModuleID = command.get(1).asInt();
		   reply.addString("OK");
		   // delete from vector
		   std::vector<ModuleInfo>::iterator itr;
		   for ( itr = listOfModules.begin(); itr != listOfModules.end(); ++itr ) {
			   if(thisModuleID == (*itr).ID) {
				   listOfModules.erase(itr);
				   break;
			   }
		   }
	    } else {
			reply.addString("ERROR: Could not parse integer! the syntax should be: del <moduleID as integer>");
		}
		
	}
	
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

	return true;
}

///*
//* Interrupt function.
//*/
//bool CoreModule::interruptModule()
//{
//	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
////	leftInPort.interrupt();
////	rightInPort.interrupt();	
//	outputPort_ImagePosition.interrupt();
//	moduleOutputPort.interrupt();
//	return true;
//}
//
//
/*
* Close function, to perform cleanup.
*/
bool CoreModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;

	moduleOutputPort.close();
//	leftInPort.close();
//	rightInPort.close();	
	outputPort_ImagePosition.close();
	
	return true;
}

