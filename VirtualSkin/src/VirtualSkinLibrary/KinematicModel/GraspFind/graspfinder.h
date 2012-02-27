#ifndef GRASPFINDER_H_
#define GRASPFINDER_H_

#include <cmath>
#include <iostream>
#include <QVector>
#include <cassert>

#include "model.h"
#include "sphere.h"

#include "posefitnessfunction.h"
#include "simulator.h"
#include "xnes.h"

class GraspFinder
{
public:
	GraspFinder( KinematicModel::Model& model,  KinematicModel::Robot& robot) :
		d_simulator(model, robot),
		d_pose_fitness_function(d_simulator)
	{}

	~GraspFinder() {}

	void find_pose(unsigned int maxevals = 100000, double fitness_threshold = 0., double std = .4, int population_size = 150);

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

public:
	Simulator d_simulator;

	std::vector<double> d_best_point;

	PoseFitnessFunction d_pose_fitness_function;

private:
	Matrix get_start_pos(int dim);

};

#endif
/** @} */
