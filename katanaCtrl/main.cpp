#include <cstdio>
#include <iostream>
#include "jointController.h"

//#define VOCAB_PREGRASP VOCAB3('p','r','e')	
#define VOCAB_NEXT VOCAB4('n','e','x', 't')	

int main(int argc, char *argv[])
{
	std::string name = "/katanaCtrl";
	/******************************************
	 *** Get arguments off the command line ***
	 ******************************************/
	yarp::os::Property config;
	config.fromCommand(argc, argv);
	
	if ( !config.check("robot") )	// the name of the robot to connect to
	{
		printf("Please specify a robot name using '--robot yourRobotNameF'.\n");
		return 1;
	}

	if ( !config.check("file") )	// the name of the robot to connect to
	{
		printf("Please specify a csv file with positions using '--file filename.csv'.\n");
		return 1;
	}
	
	try
	{
		JointController controller;

		controller.loadPositions( config.find("file").asString().c_str() );		
		controller.start( config.find("robot").asString().c_str(), "arm" );

		
		yarp::os::Network yarp;
		yarp::os::RpcServer port;
		printf("Opening port...");
		port.open(name.c_str());
		printf(" . DONE!");
		
		while (true) {
			printf("Waiting for a message...\n");
			yarp::os::Bottle cmd;
			yarp::os::Bottle response;
			port.read(cmd,true);
			
			int command  = cmd.get(0).asVocab();
			printf("gOT MESSAGE...%s\n", cmd.toString().c_str());

			switch ( command ) {
				case VOCAB_NEXT:
					response.addInt(controller.nextTarget);
					controller.goToNext();
					//response.addList() = *(controller.getEncodersAsBottle());
					response.addString("OK");
					break;
//				case VOCAB_GRASP:
//					speed = cmd.get(1).asInt();
//					if ( speed == 0 ) { speed = 50; }
//					controller.grasp( speed );
//					response.addString("OK");
//					break;
//				case VOCAB_UNGRASP:
//					speed = cmd.get(1).asInt();
//					if ( speed == 0 ) { speed = 50; }
//					controller.unGrasp( speed );
//					response.addString("OK");
//					break;
				default:
					response.addString("Unknown Command.  Use 'next'");
					break;
			}
			printf("Got message: %s\n", cmd.toString().c_str());
			printf("Sending reply: %s\n", response.toString().c_str());
			port.reply(response);
		}
		
	}
	catch ( std::string e )
	{
		printf("%s", e.c_str());
	}
	catch (...) {
		printf("All shits fucked up, mate!");
	}
	return 0;
}
