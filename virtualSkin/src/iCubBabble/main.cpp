#include "iCubBabbler.h"

int main(int argc, char *argv[])
{
	/******************************************
	 *** Get arguments off the command line ***
	 ******************************************/
	yarp::os::Property config;
	config.fromCommand(argc,argv);
	
	if ( !config.check("robot") )	// the name of the robot to connect to
	{
		printf("Please specify a robot name using '--robot yourRobotName'.\n");
		return 1;
	}
	
	int type;
	if ( config.check("type") )  // the type of motor babbling to do
	{
		if ( config.find("type").asString() == "velocity" ) { type = 0; }
		else if ( config.find("type").asString() == "position" ) { type = 1; }
		else
		{
			printf("The only motion types currently supported are 'position' and 'velocity'.\n");
			return 1;
		}
	}
	else { type = 0; }
	
	qreal period;	// the time between commands that will be sent
	if ( config.check("period") ) { period = config.find("period").asDouble(); }
	else { period = 1.0; }
	period *= 1000000;
	
	qreal speed;	// the speed to use for both position and velocity commands
	if ( config.check("speed") ) { speed = config.find("speed").asDouble(); }
	else { speed = 20.0; }
	
	bool hands;		// whether or not to include the hands in babbling
	if ( config.check("hands") ) { hands = true; }
	else { hands = false; }

	/**********************
	 *** START BABBLING ***
	 **********************/
	ICubBabbler babbler;
	if ( !babbler.configure(config.find("robot").asString().c_str()) )
	{
		printf("Failed to configure iCubBabbler. Check the robot name.\n");
		return 1;
	}
	
	switch ( type )
	{
		case 0 : babbler.crackBaby( period, speed, hands );
		case 1 : babbler.doTheRobot( period, speed, hands );
	}
	
	return 0;
}
