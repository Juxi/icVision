#ifndef __DEMOTHREADS_H__
#define __DEMOTHREADS_H__

#include <QThread>

#include "graspfinder.h"
#include "path_extractor.h"

class GraspThread : public QThread {
	bool keepRunning;
	bool verbose;
	bool d_fast;
	GraspFinder d_grasp_finder;

public:
	GraspThread(KinematicModel::Model& model,  KinematicModel::Robot& robot, bool with_things, bool fast = false)
	: verbose(false), keepRunning(true), d_grasp_finder(model, robot), d_fast(fast)
	{
		if (with_things) {
			for (float z(-.2); z < .2; z += .03)
				d_grasp_finder.simulator().add_ball(-.2, .15, z);
			for (float z(-.2); z < .2; z += .03)
				d_grasp_finder.simulator().add_ball(-.2, .3, z);
		}
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
		if (d_fast)
			d_grasp_finder.find_pose(100000, 0., .04, 50);
		else
			d_grasp_finder.find_pose(100000, 0., .4, 150);
	}
};

class MapThread : public QThread {
	bool keepRunning;
	bool verbose;

	GraspFinder d_grasp_finder;
	MapBuildConstraint *d_map_build_constraint;
	std::vector<std::vector<double> > *d_points;
	std::vector<std::vector<double> > d_configuration_points;

	std::string d_marker;

public:
	MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot, std::string marker, size_t nn, double alpha)
	: verbose(false), keepRunning(true), d_grasp_finder(model, robot), d_marker(marker)
	{
		d_map_build_constraint = new MapBuildConstraint("right_hand", 2, .05);
		d_points = &(d_map_build_constraint->points());

		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 1.);

		d_grasp_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.add_constraint(new PlaneConstraint(d_marker, 2, .05), 10.);

		d_grasp_finder.add_constraint(new CollisionConstraint());

		d_grasp_finder.add_constraint(d_map_build_constraint);

		d_grasp_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., 0., -1.)));


	}

	void add_best_pose() {
		std::vector<double> best_point = d_grasp_finder.best_point();
		KinematicModel::RobotObservation observation = d_grasp_finder.simulator().robot().observe();
		std::vector<double> position = observation.markerPosition(QString(d_marker.c_str()));

		d_configuration_points.push_back(best_point);
		d_points->push_back(position);

		d_grasp_finder.simulator().add_point(position[0], position[1], position[2]);
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
		while (true) {
			d_grasp_finder.find_pose(20000, 0., .10, 50);
			add_best_pose();
		}
	}

};

class StoreFilter : public EvaluationFilter{
public:
	std::vector<std::vector<double> > d_configurations;
	std::vector<double> d_fitnesses;

	virtual void operator()(std::vector<double> &values, double fitness) {
		d_configurations.push_back(values);
		d_fitnesses.push_back(fitness);
	}

	std::vector<std::vector<double> > &configurations() {
		return d_configurations;
	}

	std::vector<double> &fitnesses() {
		return d_fitnesses;
	}

};

class PathThread : public QThread {
	bool keepRunning;
	bool verbose;

	GraspFinder d_grasp_finder;
	MapBuildConstraint *d_map_build_constraint;
	std::vector<std::vector<double> > *d_points;
	std::vector<std::vector<double> > d_configuration_points;

	std::string d_marker;

public:
	PathThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true),
	  d_grasp_finder(model, robot)
	{
		if (true) {
			for (float z(-.2); z < .2; z += .03)
				d_grasp_finder.simulator().add_ball(-.2, .25, z);
//			for (float z(-.2); z < .2; z += .03)
//				d_grasp_finder.simulator().add_ball(-.2, .3, z);
		}
		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 10.);

		d_grasp_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.simulator().add_point(-0.237605, 0.234241,  0.1390077);

		d_grasp_finder.add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .04, Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.add_constraint(new CollisionConstraint());
//
//		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 1.);
//
//		d_grasp_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));
//
//		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));
//
//	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));
//
//		d_grasp_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));
//
//		d_grasp_finder.add_constraint(new CollisionConstraint());
//
		d_grasp_finder.set_filter(new StoreFilter());
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
		d_grasp_finder.find_pose(30000, 0., .1, 350);
//		d_grasp_finder.find_pose(10000, 0., .04, 50);

		StoreFilter &store_filter = reinterpret_cast<StoreFilter&>(d_grasp_finder.get_filter());
		std::cout << "n_points: " << store_filter.d_configurations.size() << std::endl;

		std::vector<std::vector<double> > path;
		try {
			path = extract_path(store_filter.configurations(), store_filter.fitnesses(), d_grasp_finder.simulator().home_pos(), d_grasp_finder.best_point());
		}
		catch (char const *message) {
			std::cout << message << std::endl;
		}

		while (true) {
			for (size_t i(0); i < path.size(); ++i) {
				std::cout << "step " << i << "/" << path.size() << std::endl;
				d_grasp_finder.simulator().set_motors(path[i]);
				d_grasp_finder.simulator().computePose();
				msleep(300);
			}
		}
	}

};

#endif
