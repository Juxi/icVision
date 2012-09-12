#ifndef __POSE_FITNESS_FUNCTION_H__
#define __POSE_FITNESS_FUNCTION_H__

#include <vector>
#include "matrix.h"
#include "model.h"
#include "function.h"
#include "simulator.h"
#include "constraints.h"

class EvaluationFilter {
public:
	virtual void operator()(const std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation) = 0;
};

class DummyFilter : public EvaluationFilter {
public:
	void operator()(const std::vector<double> &values, double fitness, int n_collisions, KinematicModel::RobotObservation &observation){};
};

class PoseFitnessFunction : public Function {
private:
	Simulator &d_simulator;
	std::vector<Constraint*> d_constraints;
	std::vector<double> d_weights;
	EvaluationFilter *d_filter;

	bool d_debug;
	bool d_colliding; //variable will stay on if latest pose collided

public:
	PoseFitnessFunction(Simulator &simulator);
	~PoseFitnessFunction();
	double eval(const Matrix& point);
	double eval(const std::vector<double>& point);
	void add_constraint(Constraint *constraint, double weight = 1.0);
	void clear_constraints();
	void set_filter(EvaluationFilter *filter);
	Simulator &simulator() { return d_simulator; };
	bool colliding(){ return d_colliding; }
	EvaluationFilter &get_filter() { return *d_filter; }
	bool &debug() { return d_debug; }
};


struct ObservationWorkspaceFunction : public WorkspaceFunction {
	std::vector<double> get_workspace(Function &fitness_function);
};

#endif
