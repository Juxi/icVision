#include "model.h"
#include "constants.h"

using namespace RobotModel;

Model::Model( bool visualize ) : keepRunning(true), col_count(0)
{
	dtSetDefaultResponse( collisionHandler, DT_WITNESSED_RESPONSE, (void*) this);
	
	if ( visualize )
	{ 
		skinWindow = new SkinWindow();
		
		QObject::connect( &robot, SIGNAL(appendedObject(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( &robot, SIGNAL(outdatedDisplayList(int)),					skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		
		QObject::connect( &world, SIGNAL(appendedObject(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( &world, SIGNAL(outdatedDisplayList(int)),					skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		
		QObject::connect( this, SIGNAL(newStateReady()),				skinWindow->glWidget, SLOT(update()) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	&robot, SLOT(callLists()) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	&world, SLOT(callLists()) );
		
		skinWindow->show();
	}
	
}

Model::~Model()
{
}

bool Model::computePose()
{
	if ( !robot.isOpen() )
	{
		printf("COLLISION DETECTOR ERROR: the robot has not been configured...  call robot.open(QString) before computePose()\n");
		return 0;
	}
	
	// Prepare to do collision detection
	col_count = 0;			// reset collision counter
	computePosePrefix();	// pure virtual function for extra pre-collision-detection computations
	
	robot.updatePose();		// do forward kinematics
	dtTest();				// do collision detection
	
	emit newStateReady();
	
	if ( col_count )
	{
		emit collision();
		printf("%i Collisions!\n", col_count);
	}
	
	computePoseSuffix();

	return col_count == 0;
}

void Model::run()
{
	while ( keepRunning )
	{
		computePose();
		usleep(YARP_PERIOD_us);
	}
}

void Model::stop()
{
	keepRunning = false;
	while ( isRunning() ) {}
}
