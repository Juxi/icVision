#include "graspfinder.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>

#include "constraints.h"
#include "posefitnessfunction.h"

using namespace std;

vector<double> GraspFinder::get_best_point() {
	return d_best_point;
}

Matrix GraspFinder::get_start_pos(int dim) {
	Matrix pt = Matrix::zeros(dim);
	for (size_t i(0); i < d_simulator.d_home_pos.size(); ++i)
	  pt[i] = d_simulator.d_home_pos[i];
	return pt;
}

void GraspFinder::find_pose(unsigned int maxevals, double fitness_threshold, double std, int population_size) {
	try
	{
		int dim = d_simulator.total_motors();
		bool useImportanceMixing = false;
		bool useBaseline = false;

		Matrix sigma = Matrix::ones(dim);
		sigma *= std;
		int population = population_size;//leave 0 to use default

		NES nes(d_pose_fitness_function, useImportanceMixing, useBaseline);
		nes.init(get_start_pos(dim), sigma, population);

		size_t n_evaluations(0);
		do
		{
			nes.iterate();
			n_evaluations += population_size;
			std::cout << "n_evaluations: " << n_evaluations << std::endl;
		}
		while (nes.bestFitness() > fitness_threshold && nes.evaluations() < maxevals);

		d_best_point = (nes.bestPoint().get_data());
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}

