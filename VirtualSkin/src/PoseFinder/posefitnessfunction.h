#ifndef __POSE_FITNESS_FUNCTION_H__
#define __POSE_FITNESS_FUNCTION_H__

#include <vector>

#include "simulator.h"
#include "xnes.h"
#include "constraints.h"
#include "exception.h"

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

	bool d_debug;
	bool d_colliding; //variable will stay on if latest pose collided

	public:


	PoseFitnessFunction(Simulator &simulator) :
		d_simulator(simulator),
		  d_filter(new DummyFilter()),
		  d_debug(false),
		  d_colliding(false)
	{
	}

	void add_constraint(Constraint *constraint, double weight = 1.0) {
		d_constraints.push_back(constraint);
		d_weights.push_back(weight);
	}

	bool colliding(){return d_colliding;}
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

	bool &debug() {return d_debug;}

	double eval(const Matrix& point)
	{
		std::vector<double> motor_values(point.get_data());
		if (isnan(motor_values[0])) {
			throw NanException();
		}
		d_simulator.set_motors(motor_values);
		double n_collisions = d_simulator.computePose();
		d_colliding = n_collisions;

		double fitness(0.0);
		KinematicModel::RobotObservation observation(d_simulator.robot().observe());
		for (size_t i(0); i < d_constraints.size(); ++i) {
			fitness += d_weights[i] * d_constraints[i]->evaluate(motor_values, observation, n_collisions);
		}
		if (d_debug) {
		  for (size_t i(0); i < d_constraints.size(); ++i)
		    std::cerr << d_constraints[i]->name() << " = " << d_weights[i] << "*" << d_constraints[i]->evaluate(motor_values, observation, n_collisions) << "  ";
		  std::cerr << std::endl;
		}
				
		(*d_filter)(motor_values, fitness, n_collisions, observation);
		usleep(5000);
		return fitness;
	}

	~PoseFitnessFunction() {
		for (size_t i(0); i < d_constraints.size(); ++i)
			delete(d_constraints[i]);
		delete(d_filter);
	}
};


#endif
