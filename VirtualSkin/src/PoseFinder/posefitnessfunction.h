#ifndef __POSE_FITNESS_FUNCTION_H__
#define __POSE_FITNESS_FUNCTION_H__

#include <vector>
#include <boost/math/special_functions/fpclassify.hpp> // for isnan
#include "simulator.h"
#include "xnes.h"
#include "constraints.h"
#include "exception.h"

class EvaluationFilter {
public:
	virtual void operator()(const std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation) = 0;
};

class DummyFilter : public EvaluationFilter {
public:
	void operator()(const std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation){};
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

	double eval(const Matrix& point);
	double eval(const std::vector<double>& point);

	~PoseFitnessFunction() {
		for (size_t i(0); i < d_constraints.size(); ++i)
			delete(d_constraints[i]);
		delete(d_filter);
	}
};

struct ObservationWorkspaceFunction : public WorkspaceFunction {
  std::vector<double> get_workspace(Function &fitness_function) {
	PoseFitnessFunction &pose_fitness_function = dynamic_cast<PoseFitnessFunction &>(fitness_function);
	return pose_fitness_function.d_simulator.robot().observe().markerPosition(QString("right_hand"));
  }
};



#endif
