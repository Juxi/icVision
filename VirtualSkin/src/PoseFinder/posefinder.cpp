#include "posefinder.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>

#include "constraints.h"
#include "posefitnessfunction.h"
#include "exception.h"

using namespace std;

vector<double> PoseFinder::best_point() {
	return d_best_point;
}

Matrix PoseFinder::get_start_pos(int dim) {
	Matrix pt = Matrix::zeros(dim);
	for (size_t i(0); i < d_start_search_pos.size(); ++i)
	  pt[i] = d_start_search_pos[i];
	return pt;
}

void PoseFinder::find_pose(unsigned int maxevals, double fitness_threshold, double variance_threshold, double std, int population_size) {
	try
	{
		int dim = d_simulator.total_motors();
		Matrix sigma = Matrix::ones(dim);

		sigma *= std;

		int population = population_size;//leave 0 to use default
		
		d_nes.init(get_start_pos(dim), sigma, population);

		size_t n_evaluations(0);
		d_pose_fitness_function.debug() = false;
		do
		  {
			try {
			  d_nes.iterate();
			  usleep(3000);
			} catch (NanException &e) {
				std::cout << "breaking isnan" << std::endl;
				break;
			}
			n_evaluations += population_size;
			//			std::cout << "n_evaluations: " << n_evaluations << std::endl;
		}
		while (d_nes.bestFitness() > fitness_threshold && d_nes.evaluations() < maxevals && d_nes.variance() > variance_threshold);

		d_best_point = (d_nes.bestPoint().get_data());
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}

void PoseFinder::set_variance(double std) {
	Matrix sigma = Matrix::ones(d_nes.dim());
	sigma *= std;

	d_nes.set_variance(sigma);
}
