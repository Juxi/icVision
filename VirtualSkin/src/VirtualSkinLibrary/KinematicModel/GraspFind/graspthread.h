#ifndef __GRASPTHREAD_H__
#define __GRASPTHREAD_H__

#include <QThread>

#include "graspfinder.h"

class GraspThread : public QThread {
	bool keepRunning;
	bool verbose;

	GraspFinder d_grasp_finder;

public:
	GraspThread(KinematicModel::Model& model,  KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true), d_grasp_finder(model, robot)
	{}


	void stop()
	{
		printf("GraspFinder thread is shutting down ");
		keepRunning = false;
		while ( isRunning() )
		{
			printf(".");
			msleep(1000);
		}
		printf("\n");
	}

	void run()
	{
		QTime time = QTime::currentTime();
		qsrand((uint)time.msec());
	//	while (true) {
			d_grasp_finder.find_pose();
	//	}
	}

};

#endif
