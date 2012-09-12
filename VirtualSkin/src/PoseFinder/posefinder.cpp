#include "posefinder.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>

#include "constraints.h"
#include "posefitnessfunction.h"
#include "exception.h"
#include "mones.h"
#include "xnes.h"
#include "simplex.h"

using namespace std;

vector<double> PoseFinder::best_point() {
	return d_best_point;
}

void PoseFinder::find_pose(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size) {
	switch (d_build_mode) {
	case XNES:
		find_pose_xnes(start_search_pos, fitness_threshold, variance_threshold, std, population_size);
		break;
	case MONES:
		find_pose_mones(start_search_pos, fitness_threshold, variance_threshold, std, population_size);
		break;
	case MC:
		find_pose_mc(start_search_pos, fitness_threshold, variance_threshold, std, population_size);
		break;
	case SIMPLEX:
		find_pose_simplex(start_search_pos, fitness_threshold);
		break;
	}
}

void PoseFinder::find_pose_mc(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size) {
	rngSeed(time(0));  
	int dim = d_simulator.total_motors();

	ObservationWorkspaceFunction test_function;

	Individual ind(start_search_pos, std);

	double last_fitness = 9999; // inverse fitness
	Mean mean(100);

	for (size_t i(0); i < 10000000; ++i) {
		Individual offspring = ind.offspring();
		//adouble fitness = pow(1.0 / (d_pose_fitness_function.eval(offspring.d_u) + .00001), 2);
		double fitness = d_pose_fitness_function.eval(offspring.d_u);

		double p_accept = min(fitness / last_fitness, 1.0);
		double r = rand_double(1.0);
		///cout << last_fitness << " " << p_accept << " " << r << endl;
		//if (r < p_accept) {
		if (fitness < last_fitness) {
			mean.update(1);
			last_fitness = fitness;
			offspring.update_win_offspring(.001, .001);
			ind = offspring;
		} else {
			mean.update(0);
			ind.update_lose(.0001);
		}
		if (i % 1000 == 0)
			cout << "mean accept: " << mean.mean() << endl;
	}

	/*MoNes mones(d_pose_fitness_function, test_function, dim, 50);

	mones.init(start_search_pos, std);

	for (size_t i(0); i < 10000; ++i) {

	//mones.d_individuals[0].d_A.print();  }
	d_best_point = mones.bestPoint().get_data();
	}*/
}

void PoseFinder::find_pose_mones(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size) {
	int dim = d_simulator.total_motors();

	ObservationWorkspaceFunction test_function;
	MoNes mones(d_pose_fitness_function, test_function, dim, 50);

	mones.init(start_search_pos, std);

	for (size_t i(0); i < 10000; ++i) {
		mones.iterate();
		//mones.d_individuals[0].d_A.print();  }
		d_best_point = mones.bestPoint().get_data();
	}
}


void PoseFinder::find_pose_simplex(std::vector<double> start_search_pos, double fitness_threshold) {
	int max_iter = 1000;
	Simplex simplex(d_pose_fitness_function);
	d_best_point = simplex.run(start_search_pos, fitness_threshold, std::vector<std::vector<double> >(), max_iter);
}


void PoseFinder::find_pose_xnes(std::vector<double> start_search_pos, double fitness_threshold, double variance_threshold, double std, int population_size) {

	try
	{
		NES nes(d_pose_fitness_function, false, false);
		int dim = d_simulator.total_motors();
		Matrix sigma = Matrix::ones(dim);
		sigma *= std;

		int population = population_size;//leave 0 to use default
		nes.init(start_search_pos, sigma, population);

		size_t n_evaluations(0);
		//d_pose_fitness_function.debug() = ;
		do
		{
			try {
				nes.iterate();
				
#ifdef WIN32
				
#else
				usleep(3000);
#endif
			} catch (NanException &e) {
				std::cout << "breaking isnan" << std::endl;
				break;
			}
			n_evaluations += population_size;
			//			std::cout << "n_evaluations: " << n_evaluations << std::endl;
		}
		while (nes.bestFitness() > fitness_threshold && nes.variance() > variance_threshold);

		d_best_point = (nes.bestPoint().get_data());
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}
