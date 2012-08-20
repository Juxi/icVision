#include "posefitnessfunction.h"

double PoseFitnessFunction::eval(const Matrix& point)
{
  std::vector<double> motor_values(point.get_data());
  if (boost::math::isnan(motor_values[0])) {
	throw NanException();
  }
  //print_vector(motor_values);

  d_simulator.set_motors(motor_values);
  double n_collisions = d_simulator.computePose();
  d_colliding = n_collisions;
  
  double fitness(0.0);
  KinematicModel::RobotObservation observation(d_simulator.robot().observe());
  for (size_t i(0); i < d_constraints.size(); ++i)
	fitness += d_weights[i] * d_constraints[i]->evaluate(motor_values, observation, n_collisions);

  if (d_debug) {
	for (size_t i(0); i < d_constraints.size(); ++i)
	  std::cerr << d_constraints[i]->name() << " = " << d_weights[i] << "*" << d_constraints[i]->evaluate(motor_values, observation, n_collisions) << "  ";
	std::cerr << std::endl;
  }
  
  (*d_filter)(motor_values, fitness, n_collisions, observation);
  return fitness;
}
