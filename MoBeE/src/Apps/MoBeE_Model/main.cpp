#include <QApplication>
#include <yarp/os/Property.h>

#include "yarprobot.h"
#include "yarpModel.h"
#include "controller.h"
#include "simSyncer.h"

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
		//QString robotFile2 = config.find("robot2").asString().c_str();
	
        // path to config dir for robot controllers
        QString confDir = config.find("conf").asString().c_str();
    
		// optional... path to the xml file that contains the world
		QString worldFile = config.find("world").asString().c_str();
		
		// whether or not to run the visualization
		bool visualize = false;
		if ( config.check("visualize") ) visualize = true;

		// print the configuration to the console
		printf("Launching MoBeE... \n");
		if ( visualize ) {	printf("  ...with visualization\n");		}
		else {				printf("  ...without visualization\n");		}
	
	/***********************************************************************/
	
	// Create the QApplication
	QApplication app( argc, argv, visualize );	// create the QT application
	
	MoBeE::YarpModel* yarpModel = NULL;
	MoBeE::YarpRobot* yarpRobot = NULL;
    QVector<Controller*> controllers;

	int result = 0;
	
	try
	{
        yarpModel = new MoBeE::YarpModel( visualize );
        yarpModel->start();	
                                /*	if we want display lists to be created automatically,
                                the model must be started prior to appending objects by
                                calling loadWorld(), loadRobot(), or appendObject()		*/
        
        // Open the RPC interface to the world model
        printf("\nOPENING WORLD RPC PORT\n");
        yarpModel->openWorldRpcPort("/MoBeE/world");
        
        // Load a robot model from file
        if ( robotFile != "" )
        {
            printf( "\nLOADING ROBOT MODEL FROM: %s\n", robotFile.toStdString().c_str() );
            yarpRobot = yarpModel->loadYarpRobot( robotFile, false );
            //printf("done loading robot\n");
            
            printf( "\nLOADING CONTROLLER CONFIG FILES FROM: %s\n", confDir.toStdString().c_str() );
            for ( int i=0; i<yarpRobot->numBodyParts(); i++ )
            {
                QString config_file;
                if (!confDir.isEmpty()) config_file = confDir + "/" + yarpRobot->getPart(i)->name() + ".ini";
                Controller* c = new Controller( yarpRobot, config_file, i, 20 );
                controllers.append(c);
                c->start();
            }
            
          /*#ifdef WIN32
            Sleep(1);
          #else
            usleep(1000);
          #endif*/
        }
        
        // Load a world config from file
        if ( worldFile != "" )
        {
            printf( "\nLOADING WORLD MODEL FROM: %s\n", worldFile.toStdString().c_str() );
            yarpModel->loadWorld( worldFile, false );
        }
	
		// run the Qt application
		result = app.exec();
        
        for ( size_t i=0; i < controllers.size(); i++ )
            controllers.at(i)->stop();
        
		if ( yarpModel ) {
            yarpModel->closeWorldRpcPort();
			yarpModel->stop(); 
			delete yarpModel;
		}
	}
	catch ( std::exception& e )
	{ 
		printf("%s\n",e.what());
		return 1;
	}

    printf("All finished\n");
    return result;
}
