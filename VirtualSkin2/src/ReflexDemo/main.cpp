#include <QApplication>
#include <yarp/os/Property.h>

//#include "model.h"
#include "yarprobot.h"
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
		QString robotFile = config.find("robot").asString().c_str();
	
		// optional... path to the xml file that contains the world
		QString worldFile = config.find("world").asString().c_str();
		
		// whether or not to run the visualization
		bool visualize = false;
		if ( config.check("visualize") ) visualize = true;
		
		// print the configuration to the console
		printf("Launching Virtual Skin... \n");
		if ( visualize ) {	printf("  ...with visualization\n");		}
		else {				printf("  ...without visualization\n");		}
	/***********************************************************************/
	
	// Create the QApplication
	QApplication app( argc, argv, visualize );	// create the QT application
	
	VirtualSkin::YarpModel* yarpModel = NULL;
	VirtualSkin::YarpRobot* yarpRobot = NULL;
	ReflexFilter*			filter	  = NULL;
	int result = 0;
	
	try
	{
		yarpModel = new VirtualSkin::YarpModel( visualize );
		yarpModel->start();	/*	if we want display lists to be created automatically,
								the model must be started prior to appending objects by
								calling loadWorld(), loadRobot(), or appendObject()		*/
		
		// Load a world model from file
		if ( worldFile != "" )
		{
			printf( "loading world model from: %s\n", worldFile.toStdString().c_str() );
			yarpModel->loadWorld( worldFile, false );
		}
		
		// Load a robot model from file
		if ( robotFile != "" )
		{
			printf( "loading robot model from: %s\n", robotFile.toStdString().c_str() );
			yarpRobot = yarpModel->loadYarpRobot( robotFile, false );
			yarpRobot->openCollisionPort("/collisions");
			yarpRobot->openObservationPort("/observations");
			
			sleep(1);
			
			// Enable Virtual Skin for the robot model
			printf( "  ...opening robot filter for '%s'\n", yarpRobot->getName().toStdString().c_str() );
			filter = new ReflexFilter( yarpRobot, visualize );
			filter->open<VirtualSkin::StateObserver,VirtualSkin::CallObserver,VirtualSkin::ResponseObserver>(); 
			 
		}
		
		// Open the RPC interface to the model
		printf("opening world RPC port\n");
		yarpModel->openWorldRpcPort("/world");
	
		// run the Qt application
		result = app.exec();
		
		if ( filter ) { filter->close(); }
		delete filter;
		
		if ( yarpModel )
		{
			//yarpModel->closeWorldRpcPort();
			yarpModel->stop(); 
			delete yarpModel;
		}
	}
	catch ( std::exception& e )
	{ 
		printf("%s\n",e.what());
		return 1;
	}

    return result;
}
