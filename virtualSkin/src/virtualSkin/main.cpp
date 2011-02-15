#include <QApplication>
#include <iostream>
#include <yarp/os/Property.h>

#include "window.h"
#include "robot.h"
#include "collisionDetector.h"
#include "RobotFilter.h"

using namespace std;

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
			cout << "Please specify a robot model using '--file yourFileName.xml'." << endl;
			return 1;
		}
		else xmlFile = config.find("file").asString();
		
		// whether or not to run the visualization
		bool visualize = false;
		if ( config.check("visualize") ) visualize = true;
		
		// print the configuration to the console
		cout << "Launching... " << endl;
		if ( visualize ) cout << "  ...with visualization" << endl;
		else cout << "  ...without visualization" << endl;
		cout << "  ...using robot model file: " << xmlFile.toStdString() << endl;
	
	/***********************************************************************/
	
	bool result = 0;
	
	// prepare objects for the QApplication
	RobotModel::Robot* robot = new RobotModel::Robot();
	RobotModel::World* world = new RobotModel::World();
	RobotFilter* filter = new RobotFilter();
	CollisionDetector* detector = new CollisionDetector();
	Window* window;
	
	detector->setRobot(robot);
	detector->setWorld(world);
	filter->setRobot(robot);
	
	/* Collision detection thread triggers the port filter 
	   to cut the connection to the user and stop the robot */
	QObject::connect( filter, SIGNAL(dataReceived()),	detector,  SLOT(computePose()) );
	QObject::connect( detector,  SIGNAL(crash()),		filter, SLOT(takeControl()) );
	QObject::connect( filter, SIGNAL(haveControl()),	detector,  SLOT(disarmDetector()) );
	QObject::connect( filter, SIGNAL(returnControl()),	detector,  SLOT(armDetector()) );
	
	// create the QT application
    QApplication app( argc, argv, visualize );
	
	if ( visualize )
	{
		// create a window in which to render the robot
		window = new Window;

		// for adding and removing OpenGL display lists
		QObject::connect( robot, SIGNAL(appendedObject(DisplayList*)), window->glWidget, SLOT(addDisplayList(DisplayList*)) );
		QObject::connect( world, SIGNAL(appendedObject(DisplayList*)), window->glWidget, SLOT(addDisplayList(DisplayList*)) );
		QObject::connect( world, SIGNAL(outdatedDisplayList(int)),		window->glWidget, SLOT(removeDisplayList(int)) );
		QObject::connect( robot, SIGNAL(outdatedDisplayList(int)),		window->glWidget, SLOT(removeDisplayList(int)) );
		
		// to actually render the display lists
		QObject::connect( robot, SIGNAL(changedState()), window->glWidget, SLOT(update()) );	// redraw the scene whenever the collision detection thread finishes a check
		QObject::connect( world, SIGNAL(changedState()), window->glWidget, SLOT(update()) ); 
		QObject::connect( window->glWidget, SIGNAL(renderStuff()), robot, SLOT(callLists()) ); 
		QObject::connect( window->glWidget, SIGNAL(renderStuff()), world, SLOT(callLists()) ); 
		
		// display the window
		window->show();
	}

	// load the robot kinematics
	if ( !robot->configure( xmlFile ) ) { return 1; }
	
	// open a yarp port that streams the status of the command filter
	filter->setPortName( "/" + robot->getName() + "F/filterStatus" );
	if ( !filter->open() ) { return 1; }
	
	// open a yarp port that serves as an RPC interface to the world
	world->setPortName( "/" + robot->getName() + "F/world" );
	world->openRpcInterface();

	// open the collision reporting port
	detector->setPortName( "/" + robot->getName() + "F/collisions" );
	detector->openPort();

		// run the application
		result = app.exec();

	printf("SHUTTIN' 'ER DOWN!!!\n");

	detector->closePort();
	world->closeRpcInterface();
	filter->close();
	
	delete detector;
	delete filter;
	delete robot;
	delete world;
	delete window;
	
    return result;
}
