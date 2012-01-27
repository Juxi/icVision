#include "graspfinder.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

void GraspFinder::stop()
{
	printf("ObjectSoup thread is shutting down ");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(1000);
	}
	printf("\n");
}

void GraspFinder::run()
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	
	while ( keepRunning )
	{
		theModel->
		int collisions = theModel->computePose();
		msleep(1);
	}
	

	//printf("ObjectSoup::run() returned\n");
}
