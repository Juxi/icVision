#include <QApplication>
#include <yarp/os/Property.h>

//#include "model.h"
#include "yarprobot.h"
#include "yarpModel.h"
#include "simSyncer.h"
#include "reflexFilter.h"
//#include "worldRpcInterface.h"
//#include "filterRpcInterface.h"

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
		QString robotFile2 = config.find("robot2").asString().c_str();
	
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
	
	// we use either these
	MoBeE::YarpModel* yarpModel = NULL;
	MoBeE::YarpRobot* yarpRobot = NULL;
	ReflexFilter*			filter	  = NULL;

	// or these
	KinematicModel::Model* model = NULL;
	KinematicModel::Robot* nonYarpRobot = NULL;
	
	// according to this
	bool useYarp = true;
	
	int result = 0;
	
	try
	{
		if ( useYarp ) 
		{
			yarpModel = new MoBeE::YarpModel( visualize );
			yarpModel->start();	
									/*	if we want display lists to be created automatically,
									the model must be started prior to appending objects by
									calling loadWorld(), loadRobot(), or appendObject()		*/
			
			// Load a robot model from file
			if ( robotFile != "" )
			{
				printf( "\nLOADING ROBOT MODEL FROM: %s\n", robotFile.toStdString().c_str() );
				yarpRobot = yarpModel->loadYarpRobot( robotFile, false );
				//printf("done loading robot\n");
				
				yarpRobot->openCollisionPort("/MoBeE/collisions");
				yarpRobot->openObservationPort("/MoBeE/observations");
				
			  #ifdef WIN32
			    Sleep(1);
			  #else
				usleep(1000);
			  #endif
				
				// Enable Virtual Skin for the robot model
				printf( "\n\nOPENING PORT FILTERS FOR ROBOT: %s\n", yarpRobot->getName().toStdString().c_str() );
				filter = new ReflexFilter( yarpRobot, visualize );
				filter->open<MoBeE::StateObserver,MoBeE::CallObserver,MoBeE::ResponseObserver>(); 
				 
				// Open the RPC interface to the filter
				printf("\n\nOPENING FILTER RPC PORT\n");
				filter->openFilterRpcPort("/MoBeE/filterRpc");
				
				// THIS CAUSES DEADLOCK PROBLEMS
				// Open the filter status port
				printf("opening filter status port\n");
				filter->openStatusPort("/MoBeE/filterStatus");
			}
			
			if ( robotFile2 != "" ) {
				//load another robot
				MoBeE::YarpRobot* yarpRobot2 = NULL;
				ReflexFilter* filter2 = NULL;
				yarpRobot2 = yarpModel->loadYarpRobot( robotFile2, false );
				yarpRobot2->openCollisionPort("/MoBeE/collisions2");
				yarpRobot2->openObservationPort("/MoBeE/observations2");
				
             #ifdef WIN32
			    Sleep(1);
			  #else
				usleep(1000);
			  #endif

				printf( " ...opening robot filter for '%s'\n", yarpRobot2->getName().toStdString().c_str() );
				filter2 = new ReflexFilter( yarpRobot2, false );
				filter2->open<MoBeE::StateObserver,MoBeE::CallObserver,MoBeE::ResponseObserver>(); 
				printf("opening filter RPC port\n");
				filter2->openFilterRpcPort("/MoBeE/filterRpc2");
			}
			
			// Load a world model from file
			if ( worldFile != "" )
			{
				printf( "\nLOADING WORLD MODEL FROM: %s\n", worldFile.toStdString().c_str() );
				yarpModel->loadWorld( worldFile, false );
			}
			
			// Open the RPC interface to the world model
			printf("\nOPENING WORLD RPC PORT\n");
			yarpModel->openWorldRpcPort("/MoBeE/world");

			// Start syncer instance
			yarpModel->openSimSyncer("/MoBeE/syncer", "/icubSim/world");
		} 
		else
		{
			model = new KinematicModel::Model( visualize );
			model->start();
			
			if ( robotFile != "" )
			{
				nonYarpRobot = model->loadRobot(robotFile, true);
			}
			
			if ( worldFile != "" )
			{
				printf( "LOADING WORLD MODEL FROM: %s\n", worldFile.toStdString().c_str() );
				model->loadWorld( worldFile, false );
			}
		}
	
		// run the Qt application
		result = app.exec();
		
		if ( filter )
		{ 
			//filter->closeFilterRpcPort();
			filter->close();
		}
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
