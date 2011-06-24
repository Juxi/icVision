#include "model.h"

using namespace RobotModel;

Model::Model( bool visualize ) : robot(NULL), world(NULL), skinWindow(NULL), keepRunning(true), col_count(0), reflexTriggered(false)
{
	// initialize the SOLID datastructures for managing collision response
	scene = DT_CreateScene();
	robotTable =  DT_CreateRespTable();  // setup of this response table is handled by KinTreeNode
	worldTable =  DT_CreateRespTable();
	OBSTACLE = DT_GenResponseClass(worldTable);
	TARGET = DT_GenResponseClass(worldTable);
	BODY_PART = DT_GenResponseClass(worldTable);
	
	// FOR SELF_COLLISION
	DT_AddDefaultResponse( robotTable, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	
	// FOR INTERACTION WITH THE ENVIRONMENT
	// anything that hits an 'OBSTACLE' causes the reflexTrigger signal to be emitted
	DT_AddClassResponse( worldTable, OBSTACLE, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	
	// anything that hits a 'TARGET' does not (but still computes the witness point)
	DT_AddClassResponse( worldTable, TARGET, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );

	// collisions between objects in the robot's environment are ignored completely
	DT_RemovePairResponse( worldTable,	OBSTACLE,	OBSTACLE,	collisionHandler);
	DT_RemovePairResponse( worldTable,	TARGET,		TARGET,		collisionHandler);
	DT_RemovePairResponse( worldTable,	TARGET,		OBSTACLE,	collisionHandler);
	
	// the robot's self-collisions are also ignored, as they are handled in another response table
	DT_RemovePairResponse( worldTable,	BODY_PART,	BODY_PART,	collisionHandler);

	// initialize the data-structures that hold the geometries in the model
	robot = new Robot(this);
	world = new World(this);
	
	// set up the window for OpenGL
	if ( visualize )
	{ 
		skinWindow = new SkinWindow();
		
		QObject::connect( robot, SIGNAL(appendedObject(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( robot, SIGNAL(outdatedDisplayList(int)),					skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		
		QObject::connect( world, SIGNAL(appendedObject(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( world, SIGNAL(outdatedDisplayList(int)),					skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		
		QObject::connect( this, SIGNAL(collisions(int)),				skinWindow->glWidget, SLOT(update(int)) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	robot, SLOT(callLists()) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	world, SLOT(callLists()) );
		
		skinWindow->show();
	}
	
}

Model::~Model()
{
	if ( skinWindow ) { delete skinWindow; }
	DT_DestroyRespTable(worldTable);
	DT_DestroyRespTable(robotTable);
}

void Model::removeSelfCollisionPair( DT_ResponseClass objType1, DT_ResponseClass objType2 )
{
	//printf("Cleared a pair response %i, %i\n", responseClass, obj->getResponseClass() );
	//DT_RemovePairResponse( getWorldTable(), objType1, objType2, reflexTrigger);
	//DT_RemovePairResponse( getWorldTable(), objType1, objType2, collisionHandler);
	
	DT_RemovePairResponse( getRobotTable(), objType1, objType2, reflexTrigger);
	//DT_RemovePairResponse( getRobotTable(), objType1, objType2, collisionHandler);
}

int Model::computePose()
{
	if ( !robot->isOpen() )
	{
		printf("COLLISION DETECTOR ERROR: the robot has not been configured...  call robot.open(QString) before computePose()...\n *** DO YOU HAVE THE RIGHT ROBOT NAME?!? ***\n");
		return 0;
	}
	
	// Prepare to do collision detection
	col_count = 0;			// reset collision counter
	reflexTriggered = false;
	computePosePrefix();	// pure virtual function for extra pre-collision-detection computations
	
	robot->updatePose();		// do forward kinematics
	
	// do collision detection
	//if ( !DT_Test(scene,robotTable) )
	//{
	//	DT_Test(scene,worldTable);
	//}
	DT_Test(scene,robotTable);
	DT_Test(scene,worldTable);
	
	// send the results of DT_TEST as Qt Signals
	if ( reflexTriggered )
	{
		printf("EMITTING REFLEX SIGNAL!!! \n");
		emit reflexResponse();
	}
	emit collisions(col_count); // this number may not be correct as we stop DT_TEST early in the case of reflexive response
	
	//if ( col_count ) { printf("%i Collisions!\n", col_count); }
	//printf("called compute pose \n");
	
	computePoseSuffix();

	return col_count;
}

void Model::run()
{	
	while ( keepRunning )
	{
		computePose();
		msleep( period );
	}
}

void Model::stop()
{
	keepRunning = false;
	while ( isRunning() ) {}
}
