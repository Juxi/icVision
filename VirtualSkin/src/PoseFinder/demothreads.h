#ifndef __DEMOTHREADS_H__
#define __DEMOTHREADS_H__

#include <QThread>
#include "sliderwindow.h"
#include "posefinder.h"
//#include "path_extractor.h"
#include "poses_reader.h"


class MapThread : public QThread {

public:
	MapThread(KinematicModel::Model& model, KinematicModel::Robot& robot);
	void run();
	void stop();

private:
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
	void add_best_pose(double minfitness);

	std::vector<double> random_pose();
	std::vector<double> outside_pose();
	void store_points(std::string filename);
	void load_points(std::string filename);
	std::vector<std::vector<double> > convert_all_to_real(std::vector<std::vector<double> > &in);
	std::vector<std::vector<double> > convert_all_to_normal(std::vector<std::vector<double> > &in);
	void filter_collisions(Simulator &simulator);
	void init_standard_poses();
};

class OnlineThread : public QThread {
	Q_OBJECT
public:
	OnlineThread(KinematicModel::Model& model, KinematicModel::Robot& robot);
	void run();
	void stop();
private:
	bool keepRunning;
	bool verbose;

	PoseFinder d_pose_finder;
	SliderWindow slider_window;

	double d_test;

};

class StoreFilter : public EvaluationFilter{

private:
	std::vector<std::vector<double> > d_configurations;
	std::vector<double> d_fitnesses;
	std::vector<int> d_collisions;
	std::ofstream d_outfile;

public:
	StoreFilter(std::string outfile) : d_outfile(outfile.c_str()) {}

	virtual void operator()(const std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation);

	std::vector<std::vector<double> > &configurations() { return d_configurations; }
	std::vector<double> &fitnesses() { return d_fitnesses; }
	std::vector<int> &collisions() { return d_collisions; }
};

/*
class PathThread : public QThread {

public:
	PathThread(KinematicModel::Model& model, KinematicModel::Robot& robot);
	void run();
	void stop();
private:
	bool keepRunning;
	bool verbose;

	PoseFinder d_pose_finder;
	MapBuildConstraint *d_map_build_constraint;
	std::vector<std::vector<double> > *d_points;
	std::vector<std::vector<double> > d_configuration_points;

	std::string d_marker;

};*/


#endif
