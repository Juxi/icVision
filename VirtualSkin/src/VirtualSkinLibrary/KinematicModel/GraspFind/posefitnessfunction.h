#ifndef __POSE_FITNESS_FUNCTION_H__
#define __POSE_FITNESS_FUNCTION_H__

#include <vector>

#include "simulator.h"
#include "xnes.h"
#include "constraints.h"

class PoseFitnessFunction : public Function
{
public:

	Simulator &d_simulator;
	std::vector<Constraint*> d_constraints;
	std::vector<double> d_weights;

	public:

	PoseFitnessFunction(Simulator &simulator) : d_simulator(simulator)
	{
	}

	void add_constraint(Constraint *constraint, double weight = 1.0) {
		d_constraints.push_back(constraint);
		d_weights.push_back(weight);
	}

	double eval(const Matrix& point) const
	{
		std::vector<double> motor_values(point.get_data());
		d_simulator.set_motors(motor_values);
		double n_collisions = d_simulator.model().computePose();

		double fitness(0.0);
		for (size_t i(0); i < d_constraints.size(); ++i)
			fitness += d_weights[i] * d_constraints[i]->evaluate(motor_values, d_simulator.robot().observe(), n_collisions);
		return fitness;
	}

	~PoseFitnessFunction() {
		for (size_t i(0); i < d_constraints.size(); ++i)
			delete(d_constraints[i]);
	}
};


#endif
