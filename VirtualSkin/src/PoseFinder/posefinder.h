#ifndef POSEFINDER_H_
#define POSEFINDER_H_

#include <cmath>
#include <iostream>
#include <QVector>
#include <cassert>

#include "yarprobot.h"
#include "yarpModel.h"

#include "sphere.h"

#include "posefitnessfunction.h"
#include "simulator.h"
#include "xnes.h"

class PoseFinder
{
public:
	PoseFinder( KinematicModel::Model& model,  KinematicModel::Robot& robot) :
		d_simulator(model, robot),
		d_pose_fitness_function(d_simulator),
		d_nes(d_pose_fitness_function, false, false)
	{
	  d_start_search_pos = d_simulator.home_pos();
	}

	~PoseFinder() {}

	void set_start_search_pos(std::vector<double> start_search_pos) {
	  d_start_search_pos = d_simulator.real_to_normal_motors(start_search_pos);
	}
	
	void find_pose(unsigned int maxevals = 100000, double fitness_threshold = 0., double variance_threshold = 0.0, double std = .4, int population_size = 150);

	void add_constraint(Constraint *constraint, double weight = 1.0){
		d_pose_fitness_function.add_constraint(constraint, weight);
	}

	void set_filter(EvaluationFilter *filter) {
		d_pose_fitness_function.set_filter(filter);
	}

	EvaluationFilter &get_filter() {
		return d_pose_fitness_function.get_filter();
	}

	void clear() {
		d_pose_fitness_function.clear_constraints();
	}

	std::vector<double> best_point();

	Simulator &simulator() {
		return d_simulator;
	}

	void set_variance(double std);

	PoseFitnessFunction &pose_fitness_function(){return d_pose_fitness_function;}
public:
	Simulator d_simulator;

	std::vector<double> d_best_point;
	std::vector<double> d_start_search_pos;

	PoseFitnessFunction d_pose_fitness_function;

	NES d_nes;

private:
	Matrix get_start_pos(int dim);

};

#endif

