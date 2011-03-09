#include <QApplication>
#include <yarp/os/Property.h>

#include "robot.h"
#include "reflexFilter.h"
#include "worldRpcInterface.h"

int main(int argc, char *argv[])
{
	/******************************************************************************
	*** Create a configuration for the robot model from command line arguments	***
	***	currently supported arguments are:										***
	***		--visualize															***
	***		--file yourRobotModel.xml (required)								***
	******************************************************************************/
	
		yarp::os::Property config;
		config.fromCommand(argc,argv);
		
		// path to the XML file that defines the robot model
		QString xmlFile = "";
		if ( !config.check("file") ) {
			printf("Please specify a robot model using '--file yourFileName.xml'\n");
			return 1;
		}
		else xmlFile = config.find("file").asString();
		
		// whether or not to run the visualization
		bool visualize = false;
		if ( config.check("visualize") ) visualize = true;
		
		// print the configuration to the console
		printf("Launching... \n");
		if ( visualize ) 
		{
			printf("  ...with visualization\n");
		}
		else
		{
			printf("  ...without visualization\n");
			printf("  ...using robot model file: %s\n", xmlFile.toStdString().c_str());
		}
	/***********************************************************************/
	
	// First, create the QApplication
	QApplication app( argc, argv, visualize );	// create the QT application
	
	// The Robot Model
	RobotModel::Robot robot;
	
	// The World Model
	RobotModel::World world;
	//VirtualSkin::WorldRpcInterface worldRPC;
	//worldRPC.setWorld(&world);
	//worldRPC.setName( "/" + robot.getName() + "F/world" );
	//worldRPC.start();
	
	// Virtual Skin Command Filter
	ReflexFilter filter(visualize);
	filter.setRobot(&robot);
	filter.setWorld(&world);
	//filter.setStatusPortName( "/" + robot.getName() + "F/status" );
	// TODO: name and open the collision port
	//filter.setCollisionPortName( "/" + robot->getName() + "F/collisions" );
	
	if ( !robot.configure( xmlFile ) ) { return 1; }									// parse the XML file and create the robot model
	if ( !filter.open<VirtualSkin::StateObserver,
					  VirtualSkin::CallObserver,
					  VirtualSkin::ResponseObserver>() ) { return 1; }	// start the Virtual Skin command filter
	
	bool result = 0;
	result = app.exec();						// run the Qt application

	printf("SHUTTIN' 'ER DOWN!!!\n");
	//worldRPC.stop();
	filter.close();

    return result;
}
