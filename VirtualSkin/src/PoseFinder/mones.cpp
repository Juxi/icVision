#include "mones.h"

MoNes::MoNes(Function &fitness_function, size_t dim, size_t n) : d_fitness_function(fitness_function), d_dim(dim), d_n(n) {
  d_na = 1./4 * pow(static_cast<float>(dim), -1.5);
  d_nsigma_min = 1./5 * pow(static_cast<float>(dim), -1.5);
  d_nsigma_plus = pow(static_cast<float>(dim), -1.5);
  
  //d_na = 1./8.;
  //d_nsigma_min = 1./10.;
  //d_nsigma_plus = 1./2.;
}

void Individual::update_if_has_parent(double na, double nsigma_plus, double nsigma_min) {
  if (d_parent) {
	double f1 = get_fitness();
	double f2 = d_parent->get_fitness();
	double fraction = -f1 / -f2;
	double rand_val =  static_cast<double>(rand() % 1000000) / 1000000.;
	std::cout << f1 << " " << f2 << " " << (f2 < f1) << " " << fraction << " " << rand_val << std::endl;
	
	if ((f1 < f2)) {// || (rand_val < fraction)) {
	  //if (get_fitness() < d_parent->get_fitness()) {
	  update_win_offspring(na, nsigma_plus);
	  d_parent->update_win_parent(nsigma_plus);
	} else {
	  update_lose(nsigma_min);
	  d_parent->update_lose(nsigma_min);
	}
  }
}
