#ifndef __DEMOTHREADS_H__
#define __DEMOTHREADS_H__

#include <QThread>
#include <fstream>
#include <algorithm>

#include "sliderwindow.h"
#include "posefinder.h"
//#include "path_extractor.h"
#include "poses_reader.h"

inline bool exists(std::string filename) {
	std::ifstream in_file(filename.c_str());
	return in_file;
}

class MapThread : public QThread {
	bool keepRunning;
	bool verbose;

	PoseFinder d_pose_finder;
	MapBuildConstraint *d_map_build_constraint;
	std::vector<std::vector<double> > *d_points;
	std::vector<std::vector<double> > d_configuration_points;

	std::string d_marker;

	size_t d_population_size;
	double d_start_std;
	std::vector<double> d_simulator_home_pose, d_simulator_wide_pose;

public:
	MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot);

	void nullspace_function();

	void move_box_function();

	void hold_something_function();

	void hand_left_function();

	void hand_right_function();

	void hand_right_look_varun_function();

	void hand_right_mark_function();

	void grasp_function();

	void around_object_function();

    void read_constraints(std::string filename);

	void add_best_pose() {
		std::vector<double> best_point = d_pose_finder.best_point();

		//assumed last pose was best pose?
		d_pose_finder.d_simulator.set_motors(best_point);
		double n_collisions = d_pose_finder.d_simulator.computePose();

		//
		KinematicModel::RobotObservation observation = d_pose_finder.simulator().robot().observe();
		std::vector<double> position = observation.markerPosition(QString(d_map_build_constraint->marker().c_str()));
		if (n_collisions)
		  return;

		d_configuration_points.push_back(best_point);
		d_map_build_constraint->add_point(position, best_point);
		for (size_t i(0); i < position.size(); ++i)
		  std::cout << position[i] << " ";
		std::cout << std::endl;
		d_pose_finder.simulator().add_point(position[0], position[1], position[2]);
	}

	std::vector<double> random_pose() {
	  return d_configuration_points[rand() % d_configuration_points.size()];
	}

	void store_points(std::string filename) {
		poses_map_t store_map;

		std::vector<std::vector<double> > real_poses = convert_all_to_real(d_configuration_points);

		for (size_t i(0); i < real_poses.size(); ++i) {
			std::map<std::string, std::vector<double> > a_motor_map(d_pose_finder.simulator().to_motor_named_map(real_poses[i]));
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
	
	void load_points(std::string filename);
	
	std::vector<std::vector<double> > convert_all_to_real(std::vector<std::vector<double> > &in) {
		std::vector<std::vector<double> >  out;
		for (size_t i(0); i < in.size(); ++i)
			out.push_back(d_pose_finder.simulator().normal_to_real_motors(in[i]));
		return out;
	}

	std::vector<std::vector<double> > convert_all_to_normal(std::vector<std::vector<double> > &in) {
		std::vector<std::vector<double> >  out;
		for (size_t i(0); i < in.size(); ++i) {
			out.push_back(d_pose_finder.simulator().real_to_normal_motors(in[i]));
		}
		return out;
	}

	void stop()
	{
		printf("PoseFinder thread is shutting down ");
		keepRunning = false;
		while ( isRunning() )
		{
			printf(".");
			msleep(1000);
		}
		printf("\n");
	}

	void filter_collisions(Simulator &simulator) {
		std::vector<std::vector<double> > filtered_configurations;
		std::vector<std::vector<double> > filtered_points;

		for (size_t i(0); i < d_configuration_points.size(); ++i) {
			std::vector<double> pose = d_configuration_points[i];//d_pose_finder.simulator().home_pos();
			simulator.set_motors(pose);
			double n_collisions = simulator.computePose();
			if (n_collisions) {
				std::cout << "FILTARR" << std::endl;
				continue;
			}
			filtered_configurations.push_back(pose);
			filtered_points.push_back((*d_points)[i]);
		}

		d_configuration_points = filtered_configurations;
		*d_points = filtered_points;
	}

	void init_standard_poses();
	void run();
};

class OnlineThread : public QThread {
	Q_OBJECT

	bool keepRunning;
	bool verbose;

	PoseFinder d_pose_finder;
	SliderWindow slider_window;

	double d_test;
public:
	OnlineThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true), d_pose_finder(model, robot)
	{
		PositionConstraint *position_constraint = new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900));

		d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 1.);

		d_pose_finder.add_constraint(position_constraint);

		OrientationConstraint *orientation_constraint = new OrientationConstraint("right_hand", 1, Constraint::vector3(0., -1., 0.));
		d_pose_finder.add_constraint(orientation_constraint);

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_pose_finder.add_constraint(new PlaneConstraint("left_hand", 2, .0), 10.);

		d_pose_finder.add_constraint(new CollisionConstraint());

		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., 0., 1.)));

		slider_window.show();

		slider_window.add_slider("Front", &(position_constraint->d_goal_position[0]), -.3, .3);
		slider_window.add_slider("Right", &(position_constraint->d_goal_position[1]), -.3, .3);
		slider_window.add_slider("Up", &(position_constraint->d_goal_position[2]), -.3, .3);

		slider_window.add_slider("Pointing Front", &(orientation_constraint->element(0)), -1, 1);
		slider_window.add_slider("Pointing Right", &(orientation_constraint->element(1)), -1, 1);
		slider_window.add_slider("Pointing Up", &(orientation_constraint->element(2)), -1, 1);


		connect(&slider_window, SIGNAL(something_changed()), this, SLOT(reset_variance()));
	}


	void stop()
	{
		printf("PoseFinder thread is shutting down ");
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
			if (slider_window.changed()) {
			  d_pose_finder.find_pose(d_pose_finder.get_normal_homepos(), 0., 1.0e-6, .10, 50);
				slider_window.changed(false);
				std::cout << "Found pose" << std::endl;
			}
			else
				sleep(1);
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
/*
class PathThread : public QThread {
	bool keepRunning;
	bool verbose;

	PoseFinder d_pose_finder;
	MapBuildConstraint *d_map_build_constraint;
	std::vector<std::vector<double> > *d_points;
	std::vector<std::vector<double> > d_configuration_points;

	std::string d_marker;

public:
	PathThread(KinematicModel::Model& model, KinematicModel::Robot& robot)
	: verbose(false), keepRunning(true),
	  d_pose_finder(model, robot)
	{
		if (false) {
			for (float z(-.2); z < .2; z += .03)
				d_pose_finder.simulator().add_ball(-.2, .25, z);
//			for (float z(-.2); z < .2; z += .03)
//				d_pose_finder.simulator().add_ball(-.2, .3, z);
		}
		float x(-0.2076), y(0.2942), z(0.13900);

		d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 10.);

		d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_pose_finder.simulator().add_point(x, y, z);

		d_pose_finder.add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .04, Constraint::vector3(x, y, z)));

		d_pose_finder.add_constraint(new CollisionConstraint(), 2.);
//
//		d_pose_finder.add_constraint(new HomePoseConstraint(d_pose_finder.simulator().d_home_pos), 1.);
//
//		d_pose_finder.add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));
//
//		d_pose_finder.add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));
//
//	//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));
//
//		d_pose_finder.add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.2376, 0.2342, 0.13900)));
//
//		d_pose_finder.add_constraint(new CollisionConstraint());
//
		d_pose_finder.set_filter(new StoreFilter("position"));
	}


	void stop()
	{
		printf("PoseFinder thread is shutting down ");
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
		d_pose_finder.find_pose(10000, 0.,.1, 100);
//		d_pose_finder.find_pose(10000, 0., .04, 50);

		StoreFilter &store_filter = reinterpret_cast<StoreFilter&>(d_pose_finder.get_filter());
		std::cout << "n_points: " << store_filter.d_configurations.size() << std::endl;

		std::vector<std::vector<double> > path;
		try {
		  path = extract_path(store_filter.configurations(), store_filter.fitnesses(), store_filter.collisions(), d_pose_finder.simulator().home_pos(), d_pose_finder.best_point());
		}
		catch (char const *message) {
			std::cout << message << std::endl;
		}

		while (true) {
			for (size_t i(0); i < path.size(); ++i) {
				std::cout << "step " << i << "/" << path.size() << std::endl;
				if (i)
					std::cout << "distance: " << Constraint::pos_error(path[i-1], path[i]) << std::endl;
				d_pose_finder.simulator().set_motors(path[i]);
				d_pose_finder.simulator().computePose();

				std::vector<double> position = d_pose_finder.simulator().robot().observe().markerPosition(QString("right_hand"));
				std::cout << "pos" << std::endl;
				for (size_t i(0); i < position.size(); ++i)
				  std::cout << position[i] << " ";
				std::cout << std::endl;
				msleep(300);
			}
		}
	}

};
*/

#endif
