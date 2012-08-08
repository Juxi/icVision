#include "mones.h"
#include <ctime>

MoNes::MoNes(Function &fitness_function, size_t dim, size_t n) : d_fitness_function(fitness_function), d_dim(dim), d_n(n) {
  rngSeed(time(0));
  d_na = 1./4 * pow(static_cast<float>(dim), static_cast<float>(-1.5));
  d_nsigma_min = 1./5 * pow(static_cast<float>(dim), static_cast<float>(-1.5));
  d_nsigma_plus = pow(static_cast<float>(dim), static_cast<float>(-1.5));
  
  //d_na = 1./8.;
  //d_nsigma_min = 1./10.;
  //d_nsigma_plus = 1./2.;
}

/*
void Individual::update_if_has_parent(double na, double nsigma_plus, double nsigma_min) {
  if (d_parent_id) {
	if (d_rank < d_parent_rank) {
	  update_win_offspring(na, nsigma_plus);
	  d_parent->update_win_parent(nsigma_plus);
	} else {
	  update_lose(nsigma_min);
	  d_parent->update_lose(nsigma_min);
	}
  }
}
*/
