#include "posefinder.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>

#include "constraints.h"
#include "posefitnessfunction.h"
#include "exception.h"
#include "mones.h"

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
  switch (d_build_mode) {
  case XNES:
	find_pose_xnes(maxevals, fitness_threshold, variance_threshold, std, population_size);
	break;
  case MONES:
	find_pose_mones(maxevals, fitness_threshold, variance_threshold, std, population_size);
	break;
  }
}

void PoseFinder::find_pose_mones(unsigned int maxevals, double fitness_threshold, double variance_threshold, double std, int population_size) {
  int dim = d_simulator.total_motors();

  ObservationWorkspaceFunction test_function;
  MoNes mones(d_pose_fitness_function, test_function, dim, 50);

  mones.init(d_start_search_pos, std);

  for (size_t i(0); i < 10000; ++i) {
	mones.iterate();
	//mones.d_individuals[0].d_A.print();  }
  d_best_point = mones.bestPoint().get_data();
  }
}

void PoseFinder::find_pose_xnes(unsigned int maxevals, double fitness_threshold, double variance_threshold, double std, int population_size) {
  
  try
	{
	  NES nes(d_pose_fitness_function, false, false);
	  int dim = d_simulator.total_motors();
		Matrix sigma = Matrix::ones(dim);
		sigma *= std;

		int population = population_size;//leave 0 to use default
		
		nes.init(get_start_pos(dim), sigma, population);

		size_t n_evaluations(0);
		//d_pose_fitness_function.debug() = ;
		do
		  {
			try {
			  nes.iterate();
			  #ifdef WIN32
			    Sleep(3);
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
		while (nes.bestFitness() > fitness_threshold && nes.evaluations() < maxevals && nes.variance() > variance_threshold);

		d_best_point = (nes.bestPoint().get_data());
	}
	catch (const char* exception)
	{
		printf("\n\nEXCEPTION: %s\n\n", exception);
	}
}
