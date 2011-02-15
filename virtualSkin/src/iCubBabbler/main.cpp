#include "iCubBabbler.h"

int main(int argc, char *argv[])
{
	yarp::os::Property config;
	config.fromCommand(argc,argv);
	
	// path to the XML file that defines the robot model
	QString xmlFile = "";
	if ( !config.check("robot") )
	{
		printf("Please specify a robot name using '--robot yourRobotName'.\n");
		return 1;
	}
	
	int type;
	if ( config.check("type") )
	{
		if ( config.find("type").asString() == "crackBaby" ) { type = 0; }
		else if ( config.find("type").asString() == "doTheRobot" ) { type = 1; }
		else
		{
			printf("The only motion types currently supported are 'crackBaby' and 'doTheRobot'.\n");
			return 1;
		}
	}
	else
	{
		printf("Please specify the type of motion with '--type' followed by 'crackBaby' or 'doTheRobot'.\n");
		return 1;
	}
	
	qreal freq;
	if ( config.check("freq") ) { freq = config.find("freq").asDouble(); }
	else { freq = 1.0; }
	
	qreal speed;
	if ( config.check("speed") ) { speed = config.find("speed").asDouble(); }
	else { speed = 20.0; }
	
	bool hands;
	if ( config.check("hands") ) { hands = true; }
	else { hands = false; }

	ICubBabbler babbler;
	if ( !babbler.configure(config.find("robot").asString().c_str()) )
	{
		printf("Failed to configure iCubBabbler. Check the robot name.\n");
		return 1;
	}
	
	switch ( type )
	{
		case 0 : babbler.crackBaby( 1/freq, speed, hands );
		case 1 : babbler.doTheRobot( 1/freq, speed, hands );
	}
	
	return 0;
}
