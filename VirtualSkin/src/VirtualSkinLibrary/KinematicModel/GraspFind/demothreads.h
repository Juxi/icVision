#ifndef __DEMOTHREADS_H__
#define __DEMOTHREADS_H__

#include <QThread>
#include <fstream>
#include <algorithm>

#include "sliderwindow.h"
#include "graspfinder.h"
#include "path_extractor.h"
#include "poses_reader.h"

inline bool exists(std::string filename) {
	std::ifstream in_file(filename.c_str());
	return in_file;
}

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
		d_map_build_constraint = new MapBuildConstraint("left_hand", 2, .025);
		d_points = &(d_map_build_constraint->points());

		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 1.);

		d_grasp_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));

		d_grasp_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.add_constraint(new PlaneConstraint(d_marker, 2, .0), 10.);

		d_grasp_finder.add_constraint(new CollisionConstraint());

		d_grasp_finder.add_constraint(d_map_build_constraint);

		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., 0., 1.)));
	}

	void add_best_pose() {
		std::vector<double> best_point = d_grasp_finder.best_point();
		KinematicModel::RobotObservation observation = d_grasp_finder.simulator().robot().observe();
		std::vector<double> position = observation.markerPosition(QString(d_marker.c_str()));

		d_configuration_points.push_back(best_point);
		d_points->push_back(position);

		d_grasp_finder.simulator().add_point(position[0], position[1], position[2]);
	}

	void store_points(std::string filename) {
		poses_map_t store_map;
		std::vector<std::vector<double> > real_poses = convert_to_real(d_configuration_points);
		for (size_t i(0); i < real_poses.size(); ++i) {
			std::map<std::string, std::vector<double> > a_motor_map(d_grasp_finder.simulator().to_motor_named_map(real_poses[i]));
			std::string name("CFGSPACE_");
			std::map<std::string, std::vector<double> >::iterator it(a_motor_map.begin());
			for (; it != a_motor_map.end(); ++it) {
				std::string sub_name(name + it->first);
				std::transform(sub_name.begin(), sub_name.end(), sub_name.begin(), toupper);
				if (!store_map.count(sub_name))
					store_map[sub_name] = std::vector<std::vector<double> >();
				store_map[sub_name].push_back(it->second);
			}
		}
		store_map["WORKSPACE"] = *d_points;
		write_poses(store_map, filename);
	}

	void load_points(std::string filename) {
		poses_map_t poses_map = read_poses(filename);

		*d_points = poses_map["WORKSPACE"];
		size_t n_poses = poses_map["WORKSPACE"].size();

		d_configuration_points = std::vector<std::vector<double> >(n_poses);
		std::cout << "test" << std::endl;
		for (size_t i(0); i < d_grasp_finder.simulator().total_parts(); ++i) {
			std::string name("CFGSPACE_");
			name += d_grasp_finder.simulator().part_name(i);
			std::transform(name.begin(), name.end(), name.begin(), toupper);
			std::vector<std::vector<double> > &part_configuration(poses_map[name]);
			for (size_t n(0); n < part_configuration.size(); ++n)
				std::copy(part_configuration[n].begin(), part_configuration[n].end(), back_inserter(d_configuration_points[n]));
		}
		std::cout << "test" << std::endl;

		for (size_t i(0); i < d_configuration_points[0].size(); ++i)
			std::cout << d_configuration_points[0][i] << " ";
		std::cout << std::endl;
		d_configuration_points = convert_to_normal(d_configuration_points);
		for (size_t i(0); i < d_configuration_points[0].size(); ++i)
			std::cout << d_configuration_points[0][i] << " ";
		for (size_t i(0); i < d_points->size(); ++i)
			d_grasp_finder.simulator().add_point((*d_points)[i][0], (*d_points)[i][1], (*d_points)[i][2]);

		std::cout << "N Points: " << d_points->size() << std::endl;
		std::cout << "Dim Points: " << d_points->at(0).size() << std::endl;

	}

	std::vector<std::vector<double> > convert_to_real(std::vector<std::vector<double> > &in) {
		std::vector<std::vector<double> >  out;
		for (size_t i(0); i < in.size(); ++i)
			out.push_back(d_grasp_finder.simulator().normal_to_real_motors(in[i]));
		return out;
	}

	std::vector<std::vector<double> > convert_to_normal(std::vector<std::vector<double> > &in) {
		std::vector<std::vector<double> >  out;
		for (size_t i(0); i < in.size(); ++i) {

			out.push_back(d_grasp_finder.simulator().real_to_normal_motors(in[i]));
		}
		return out;
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
		std::cout << "test3" << std::endl;
		if (exists("table_map_better.save"))
			load_points("table_map_better.save");

		std::cout << "test4" << std::endl;
//		store_points("table_map.save");

		QTime time = QTime::currentTime();
		qsrand((uint)time.msec());

		bool test(false);

		if (test)
			while (true) {
				size_t n(rand() % d_configuration_points.size());

				d_grasp_finder.simulator().set_motors(d_configuration_points[n]);
				double n_collisions = d_grasp_finder.simulator().computePose();
				std::cout << n_collisions << std::endl;
				sleep(1);
			}
		else
			while (true) {
				d_grasp_finder.find_pose(2000000, 0., 1.0e-6, .10, 200);
				add_best_pose();
				store_points("table_map.save");
			}
	}

};

class OnlineThread : public QThread {
	bool keepRunning;
	bool verbose;

	GraspFinder d_grasp_finder;
	SliderWindow slider_window;

public:
	OnlineThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true), d_grasp_finder(model, robot)
	{
		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 1.);

		d_grasp_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));

		d_grasp_finder.add_constraint(new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.add_constraint(new PlaneConstraint("left_hand", 2, .0), 10.);

		d_grasp_finder.add_constraint(new CollisionConstraint());

		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., 0., 1.)));

		slider_window.show();
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
		bool changed(false);

		while (true) {
			if (changed)
				d_grasp_finder.find_pose(2000000, 0., 1.0e-6, .10, 200);
			else
				usleep(10000);
		}
	}

};

class StoreFilter : public EvaluationFilter{
public:
	std::vector<std::vector<double> > d_configurations;
	std::vector<double> d_fitnesses;
	std::vector<int> d_collisions;
	std::ofstream d_outfile;
	StoreFilter(std::string outfile) : d_outfile(outfile.c_str()) {

	}

	virtual void operator()(std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation) {
//		if (n_collisions)
//			return;
		d_configurations.push_back(values);
		d_fitnesses.push_back(fitness);
		d_collisions.push_back(n_collisions);
		std::vector<double> position = observation.markerPosition(QString("right_hand"));
		for (size_t i(0); i < position.size(); ++i)
			d_outfile << position[i] << ", ";
		d_outfile << std::endl;
	}

	std::vector<std::vector<double> > &configurations() {
		return d_configurations;
	}

	std::vector<double> &fitnesses() {
		return d_fitnesses;
	}

	std::vector<int> &collisions() {
		return d_collisions;
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
		float x(-0.2076), y(0.2942), z(0.13900);

		d_grasp_finder.add_constraint(new HomePoseConstraint(d_grasp_finder.simulator().d_home_pos), 10.);

		d_grasp_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

		d_grasp_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_grasp_finder.simulator().add_point(x, y, z);

		d_grasp_finder.add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .04, Constraint::vector3(x, y, z)));

		d_grasp_finder.add_constraint(new CollisionConstraint(), 2.);
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
		d_grasp_finder.set_filter(new StoreFilter("position"));
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
		d_grasp_finder.find_pose(10000, 0.,.1, 100);
//		d_grasp_finder.find_pose(10000, 0., .04, 50);

		StoreFilter &store_filter = reinterpret_cast<StoreFilter&>(d_grasp_finder.get_filter());
		std::cout << "n_points: " << store_filter.d_configurations.size() << std::endl;

		std::vector<std::vector<double> > path;
		try {
			path = extract_path(store_filter.configurations(), store_filter.fitnesses(), store_filter.collisions(), d_grasp_finder.simulator().home_pos(), d_grasp_finder.best_point());
		}
		catch (char const *message) {
			std::cout << message << std::endl;
		}

		while (true) {
			for (size_t i(0); i < path.size(); ++i) {
				std::cout << "step " << i << "/" << path.size() << std::endl;
				if (i)
					std::cout << "distance: " << Constraint::pos_error(path[i-1], path[i]) << std::endl;
				d_grasp_finder.simulator().set_motors(path[i]);
				d_grasp_finder.simulator().computePose();

				std::vector<double> position = d_grasp_finder.simulator().robot().observe().markerPosition(QString("right_hand"));
				std::cout << "pos" << std::endl;
				for (size_t i(0); i < position.size(); ++i)
				  std::cout << position[i] << " ";
				std::cout << std::endl;
				msleep(300);
			}
		}
	}

};

#endif
