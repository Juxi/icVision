#include "mones.h"

MoNes::MoNes(Function &fitness_function, size_t dim, size_t n) : d_fitness_function(fitness_function), d_dim(dim), d_n(n) {
  d_na = 1./4 * pow(static_cast<float>(dim), -1.5);
  d_nsigma_min = 1./5 * pow(static_cast<float>(dim), -1.5);
  d_nsigma_plus = pow(static_cast<float>(dim), -1.5);
  
  //d_na = 1./8.;
  //d_nsigma_min = 1./10.;
  //d_nsigma_plus = 1./2.;
}
