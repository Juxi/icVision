#include "model.h"

using namespace RobotModel;

Model::Model( bool visualize ) : robot(NULL), world(NULL), skinWindow(NULL), keepRunning(true), col_count(0)
{
	scene = DT_CreateScene();
	responseTable =  DT_CreateRespTable();
	responseClass = DT_GenResponseClass( responseTable );
	
	DT_AddDefaultResponse(responseTable,
						  collisionHandler,
						  DT_WITNESSED_RESPONSE,
						  (void*) this);
	
	robot = new Robot(this);
	world = new World(this);
	
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
	DT_DestroyRespTable(responseTable);
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
	computePosePrefix();	// pure virtual function for extra pre-collision-detection computations
	
	robot->updatePose();		// do forward kinematics
				
	DT_Test(scene,responseTable);	// do collision detection
	
	//semit newStateReady();
	emit collisions(col_count);
	
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
