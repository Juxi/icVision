#include <QApplication>
#include <yarp/os/Property.h>

#include "robot.h"
#include "yarpModel.h"
#include "reflexFilter.h"
#include "worldRpcInterface.h"

int main(int argc, char *argv[])
{
	/******************************************************************************
	*** Create a configuration for the robot model from command line arguments	***
	***	currently supported arguments are:										***
	***		--file yourRobotModel.xml (required)								***
	***		--visualize															***
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
	
	// Create the QApplication
	QApplication app( argc, argv, visualize );	// create the QT application
	
	// Virtual Skin Command Filter
	ReflexFilter filter(visualize);
	
	// start the Virtual Skin command filter
	if ( !filter.open< VirtualSkin::StateObserver, VirtualSkin::CallObserver, VirtualSkin::ResponseObserver >( xmlFile ) ) { return 1; }
	
	// open a port to report collision events
	filter.model.openCollisionPort("/" + filter.model.robot.getName() + "F/collisions");
	
	// open a port to interact with the world model
	filter.model.openWorldRpcPort("/" + filter.model.robot.getName() + "F/world");
	
	// Filter Status Port (streams 1 or 0 indicating filter is open or closed respectively)
	filter.openStatusPort("/" + filter.model.robot.getName() + "F/status");
	filter.startStatusPort();
	
	bool result = 0;
	result = app.exec();						// run the Qt application

	printf("SHUTTIN' 'ER DOWN!!!\n");
	//worldRPC.stop();
	filter.close();

    return result;
}
