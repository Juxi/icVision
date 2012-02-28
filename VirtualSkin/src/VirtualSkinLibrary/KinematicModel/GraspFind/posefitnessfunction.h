#ifndef __POSE_FITNESS_FUNCTION_H__
#define __POSE_FITNESS_FUNCTION_H__

#include <vector>

#include "simulator.h"
#include "xnes.h"
#include "constraints.h"

class EvaluationFilter {
public:
	virtual void operator()(std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation) = 0;
};

class DummyFilter : public EvaluationFilter {
public:
	void operator()(std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation){};
};

class PoseFitnessFunction : public Function
{
public:

	Simulator &d_simulator;
	std::vector<Constraint*> d_constraints;
	std::vector<double> d_weights;
	EvaluationFilter *d_filter;

	public:

	PoseFitnessFunction(Simulator &simulator) :
		d_simulator(simulator),
		d_filter(new DummyFilter())
	{
	}

	void add_constraint(Constraint *constraint, double weight = 1.0) {
		d_constraints.push_back(constraint);
		d_weights.push_back(weight);
	}

	void clear_constraints() {
		for (size_t i(0); i < d_constraints.size(); ++i)
			delete(d_constraints[i]);
		d_constraints.clear();
		d_weights.clear();
	}

	void set_filter(EvaluationFilter *filter) {
		delete(d_filter);
		d_filter = filter;
	}

	EvaluationFilter &get_filter() {
		return *d_filter;
	}

	double eval(const Matrix& point) const
	{
		std::vector<double> motor_values(point.get_data());
		d_simulator.set_motors(motor_values);
		double n_collisions = d_simulator.computePose();

		double fitness(0.0);
		KinematicModel::RobotObservation observation(d_simulator.robot().observe());
		for (size_t i(0); i < d_constraints.size(); ++i)
			fitness += d_weights[i] * d_constraints[i]->evaluate(motor_values, observation, n_collisions);

		(*d_filter)(motor_values, fitness, n_collisions, observation);
		return fitness;
	}

	~PoseFitnessFunction() {
		for (size_t i(0); i < d_constraints.size(); ++i)
			delete(d_constraints[i]);
		delete(d_filter);
	}
};


#endif