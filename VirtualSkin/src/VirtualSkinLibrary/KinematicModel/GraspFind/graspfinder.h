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
//
//	void clear() {
//		d_pose_fitness_function = PoseFitnessFunction(d_simulator);
//	}

	std::vector<double> get_best_point();

	Simulator &simulator() {
		return d_simulator;
	}

public:
	Simulator d_simulator;

	std::vector<std::vector<double> > d_points;
	std::vector<std::vector<double> > d_configuration_points;
	std::vector<double> d_best_point;

	PoseFitnessFunction d_pose_fitness_function;

private:
	Matrix get_start_pos(int dim);

};

#endif
/** @} */
