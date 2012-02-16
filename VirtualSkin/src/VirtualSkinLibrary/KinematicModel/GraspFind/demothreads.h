#ifndef __DEMOTHREADS_H__
#define __DEMOTHREADS_H__

#include <QThread>

#include "graspfinder.h"

class GraspThread : public QThread {
	bool keepRunning;
	bool verbose;

	GraspFinder d_grasp_finder;

public:
	GraspThread(KinematicModel::Model& model,  KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true), d_grasp_finder(model, robot)
	{
		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 10.);

		d_grasp_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.simulator().add_point(-0.237605, 0.234241,  0.1390077);

		d_grasp_finder.add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .04, Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.add_constraint(new CollisionConstraint());
	}


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
