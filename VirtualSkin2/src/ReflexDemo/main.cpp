#include <QApplication>
#include <yarp/os/Property.h>

//#include "model.h"
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
		QString robotFile = config.find("robot").asString().c_str();
	
		// optional... path to the xml file that contains the world
		QString worldFile = config.find("world").asString().c_str();
		
		// whether or not to run the visualization
		bool visualize = false;
		if ( config.check("visualize") ) visualize = true;
		
		// print the configuration to the console
		printf("Launching Virtual Skin... \n");
		if ( visualize ) { printf("  ...with visualization\n"); }
		else { printf("  ...without visualization\n"); }
	/***********************************************************************/
	
	// Create the QApplication
	QApplication app( argc, argv, visualize );	// create the QT application
	
	KinematicModel::Model* model = NULL;
	KinematicModel::Robot* robot = NULL;
	ReflexFilter* filter = NULL;
	
	try
	{
		model = new VirtualSkin::YarpModel( visualize );
		model->start();						/*	if we want display lists to be created automatically,
												the model must be started prior to appending objects by
												calling loadWorld(), loadRobot(), or appendObject()		*/
	
		if ( worldFile != "" )
		{
			printf( "loading world model from: %s\n", worldFile.toStdString().c_str() );
			model->loadWorld( worldFile, false );
		}
		
		if ( robotFile != "" )
		{
			printf( "loading robot model from: %s\n", robotFile.toStdString().c_str() );
			robot = model->loadRobot( robotFile, false );
			
			printf( "  ...opening robot filter for '%s'\n", robot->getName().toStdString().c_str() );
			filter = new ReflexFilter( robot, visualize );
			filter->open< VirtualSkin::StateObserver,
						  VirtualSkin::CallObserver,
						  VirtualSkin::ResponseObserver >(); 
		}
	}
	
	catch ( std::exception& e )
	{ 
		printf("%s\n",e.what());
		return 1;
	}
	
	// run the Qt application
	int result = app.exec();

	printf("SHUTTING DOWN!!!\n");
	printf("Result = %d\n", result);

	if ( filter ) { filter->close(); }
	if ( model ) { model->stop(); }
	
	delete filter;
	delete robot;
	delete model;

    return result;
}
