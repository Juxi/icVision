#ifndef POSEFINDER_H_
#define POSEFINDER_H_

#include <vector>
#include "model.h"
#include "robot.h"
#include "simulator.h"
#include "posefitnessfunction.h"
#include "constraints.h"

class PoseFinder
{
public:
  enum BuildMode {
	XNES,
	MONES,
    MC,
	SIMPLEX
  };
  
PoseFinder( KinematicModel::Model& model, KinematicModel::Robot& robot, BuildMode build_mode = XNES) :
//PoseFinder( KinematicModel::Model& model,  KinematicModel::Robot& robot, BuildMode build_mode = SIMPLEX) :
		d_simulator(model, robot),
		  d_build_mode(build_mode),
		d_pose_fitness_function(d_simulator)
		  //d_nes(d_pose_fitness_function, false, false)
	{
	}

	~PoseFinder() {}
	void find_pose(std::vector<double> start_search_pos, double fitness_threshold = 0., double variance_threshold = 0.0, double std = .4, int population_size = 150);
	void find_pose_mones(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size);
	void find_pose_simplex(std::vector<double> start_search_pos, double fitness_threshold);
	void find_pose_xnes(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size);
    void find_pose_mc(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size);
  
	void add_constraint(Constraint *constraint, double weight = 1.0){ d_pose_fitness_function.add_constraint(constraint, weight); }
	void set_filter(EvaluationFilter *filter) { d_pose_fitness_function.set_filter(filter); }
	void clear() { d_pose_fitness_function.clear_constraints(); }

	std::vector<double> best_point();
	EvaluationFilter &get_filter() { return d_pose_fitness_function.get_filter(); }
	std::vector<double> get_normal_homepos() { return d_simulator.home_pos(); 
	//return d_simulator.real_to_normal_motors(d_simulator.home_pos());
	}
	Simulator &simulator() { return d_simulator; }
	PoseFitnessFunction &pose_fitness_function(){return d_pose_fitness_function;}
public:
	Simulator d_simulator;

	std::vector<double> d_best_point;
	
	BuildMode d_build_mode;
	PoseFitnessFunction d_pose_fitness_function;

	//NES d_nes;

private:

};

#endif