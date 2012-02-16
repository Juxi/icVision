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
		add_constraint(new HomePoseConstraint(d_simulator.d_home_pos), 10.);

		add_constraint(new PositionConstraint("left_hand", Constraint::vector3(-0.2376, -0.2342, 0.13900)));

		add_constraint(new OrientationConstraint("left_hand", 1, Constraint::vector3(0., -1., 0.)));

//		add_constraint(new PositionConstraint("right_hand", Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		d_simulator.add_point(-0.237605, 0.234241,  0.1390077);

		add_constraint(new GraspConstraint("right_thumb", "right_index", 2, 1, .04, Constraint::vector3(-0.237605, 0.234241,  0.1390077)));

		add_constraint(new CollisionConstraint());
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
