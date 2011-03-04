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
		if ( config.find("type").asString() == "velocity" ) { type = 0; }
		else if ( config.find("type").asString() == "position" ) { type = 1; }
		else
		{
			printf("The only motion types currently supported are 'position' and 'velocity'.\n");
			return 1;
		}
	}
	else { type = 0; }
	
	qreal period;
	if ( config.check("period") ) { period = config.find("period").asDouble(); }
	else { period = 1.0; }
	period *= 1000000;
	
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
		case 0 : babbler.crackBaby( period, speed, hands );
		case 1 : babbler.doTheRobot( period, speed, hands );
	}
	
	return 0;
}
