#include "handController.h"

#define VOCAB_PREGRASP VOCAB3('p','r','e')	// goto pregrasp pose (position control)
#define VOCAB_GRASP VOCAB3('c','l','s')		// close the hand	(velocity control)
#define VOCAB_UNGRASP VOCAB3('o','p','n')	// open the hand	(velocity control)

int main(int argc, char *argv[])
{
	/******************************************
	 *** Get arguments off the command line ***
	 ******************************************/
	yarp::os::Property config;
	config.fromCommand(argc, argv);
	
	if ( !config.check("robot") )	// the name of the robot to connect to
	{
		printf("Please specify a robot name using '--robot yourRobotName'.\n");
		return 1;
	}
	
	std::string part = "left_arm";	// whether or not to use the left hand (default is right)
	if ( config.check("part") )  { part = config.find("part").asString().c_str(); }
	
	std::string name = "/graspController";	// the name of the RPC server
	if ( config.check("name") )  { name = config.find("name").asString().c_str(); } 
	
	qreal errTolerance = 0.8;	// 0-1, where smaller numbers favor precision
	if ( config.check("tolerance") )  { errTolerance = config.find("tolerance").asDouble(); }
	
	qreal attFactor = 4.0;	// 0-inf larger numbers cause the controller to approach target points more slowly
	if ( config.check("attenuation") )  { attFactor = config.find("attenuation").asDouble(); }
	
	qreal accel = 10.0;	// reference acceleration for the yarp remote control board interface
	if ( config.check("acceleration") )  { accel = config.find("acceleration").asDouble(); }
	
	qreal vel = 20.0;	// reference velocity for the yarp remote control board interface
	if ( config.check("velocity") )  { vel = config.find("velocity").asDouble(); }

	try
	{
		HandController controller( errTolerance, attFactor );
		controller.start( config.find("robot").asString().c_str(), part.c_str(), accel, vel);
		
		yarp::os::Network yarp;
		yarp::os::RpcServer port;
		printf("Opening port...");
		port.open(name.c_str());
		printf(" . DONE!");
		
		int speed;
		
		while (true) {
			printf("Waiting for a message...\n");
			yarp::os::Bottle cmd;
			yarp::os::Bottle response;
			port.read(cmd,true);
			
			int command  = cmd.get(0).asVocab();
			printf("gOT MESSAGE...%s\n", cmd.toString().c_str());

			switch ( command ) {
				case VOCAB_PREGRASP:
					controller.preGrasp();
					response.addString("OK");
					break;
				case VOCAB_GRASP:
					speed = cmd.get(1).asInt();
					if ( speed == 0 ) { speed = 50; }
					controller.grasp( speed );
					response.addString("OK");
					break;
				case VOCAB_UNGRASP:
					speed = cmd.get(1).asInt();
					if ( speed == 0 ) { speed = 50; }
					controller.unGrasp( speed );
					response.addString("OK");
					break;
				default:
					response.addString("Unknown Command.  Use 'pre', 'cls (int)', or 'opn (int)'. The ints should be a speed 0-100");
					break;
			}
			printf("Got message: %s\n", cmd.toString().c_str());
			printf("Sending reply: %s\n", response.toString().c_str());
			port.reply(response);
		}
		
	}
	catch ( QString e )
	{
		printf("%s", e.toStdString().c_str());
	}
	catch (...) {
		printf("All shits fucked up, mate!");
	}
	return 0;
}
