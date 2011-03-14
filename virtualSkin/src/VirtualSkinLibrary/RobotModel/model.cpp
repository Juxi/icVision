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
	
	//crashed = false;
	col_count = 0;

	robot.updatePose();
	dtTest();				// do collision detection
	
	emit newStateReady();
	
	if ( col_count )
	{
		emit collision();
		printf("%i Collisions!\n", col_count);
		return false;
	}

	return true;
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

void Model::restart()
{
	keepRunning = true;
	start();
}
