#include "posefitnessfunction.h"
#include <boost/math/special_functions/fpclassify.hpp> // for isnan
#include "exception.h"
#include <iostream>
#include <QString>
#include <vector>

PoseFitnessFunction::PoseFitnessFunction(Simulator &simulator) :
d_simulator(simulator),
	d_filter(new DummyFilter()),
	d_debug(false),
	d_colliding(false)
{} 

PoseFitnessFunction::~PoseFitnessFunction() {
	for (size_t i(0); i < d_constraints.size(); ++i)
		delete(d_constraints[i]);
	delete(d_filter);
}

double PoseFitnessFunction::eval(const Matrix& point) {
	std::vector<double> motor_values(point.get_data());
	return eval(motor_values);
}

double PoseFitnessFunction::eval(const std::vector<double>& motor_values) {
	
	if (boost::math::isnan(motor_values[0])) {
		throw NanException();
	}

	// put motors for which mask=0 to their home position values instead of the values defined by the optimization algorithm
	std::vector<double> masked_motor_values = motor_values; // make a copy
	//print_vector(masked_motor_values);
	for (size_t i(0); i < d_constraints.size(); ++i) {
		if (d_constraints[i]->name() == "HomePoseConstraint") {
			HomePoseConstraint *constraint = dynamic_cast<HomePoseConstraint *>(d_constraints[i]);
			std::vector<double> &pose = constraint->home_pose();
			std::vector<double> &mask = constraint->home_pose_mask();
			for (size_t j(0); j < pose.size(); j++) {
				if (mask[j] <= 0) {
					masked_motor_values[j] = pose[j];
					//std::cout << "setting motor value at index " << j << " to its home position: " << pose[j] << std::endl;
				}
			}
		}
	}


	d_simulator.set_motors(masked_motor_values);
	double n_collisions = d_simulator.computePose();
	d_colliding = n_collisions;

	double fitness(0.0);
	KinematicModel::RobotObservation observation(d_simulator.robot().observe());
	for (size_t i(0); i < d_constraints.size(); ++i)
		fitness += d_weights[i] * d_constraints[i]->evaluate(masked_motor_values, observation, n_collisions);

	if (d_debug) {
		for (size_t i(0); i < d_constraints.size(); ++i)
			std::cerr << d_constraints[i]->name() << " = " << d_weights[i] << "*" << d_constraints[i]->evaluate(masked_motor_values, observation, n_collisions) << "  ";
		std::cerr << std::endl;
	}

	(*d_filter)(masked_motor_values, fitness, n_collisions, observation);
	return fitness;
}

void PoseFitnessFunction::add_constraint(Constraint *constraint, double weight) {
	d_constraints.push_back(constraint);
	d_weights.push_back(weight);
}


void PoseFitnessFunction::startpose_hook(std::vector<double>& start_pose){
	for (size_t i(0); i < d_constraints.size(); ++i)
		d_constraints[i]->startpose_hook(start_pose);
}

void PoseFitnessFunction::clear_constraints() {
	for (size_t i(0); i < d_constraints.size(); ++i)
		delete(d_constraints[i]);
	d_constraints.clear();
	d_weights.clear();
}

void PoseFitnessFunction::set_filter(EvaluationFilter *filter) {
	delete(d_filter);
	d_filter = filter;
}



std::vector<double> ObservationWorkspaceFunction::get_workspace(Function &fitness_function) {
	PoseFitnessFunction &pose_fitness_function = dynamic_cast<PoseFitnessFunction &>(fitness_function);
	return pose_fitness_function.simulator().robot().observe().markerPosition(QString("right_hand"));
}

