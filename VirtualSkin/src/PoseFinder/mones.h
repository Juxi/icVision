#ifndef __MONES_H__
#define __MONES_H__

#include "matrix.h"
#include "function.h"
#include "general.h"

#include <vector>
#include <iostream>

struct Individual {
  size_t d_dim;
  Matrix d_u, d_A;
  Matrix d_z;
  double d_sigma;
  double d_fitness;

  Individual *d_parent;
  std::vector<double> d_workspace;

Individual(Matrix u, double sigma): d_dim(u.size()), d_u(u), d_z(Matrix::zeros(d_dim)), d_sigma(sigma), d_fitness(0), d_parent(0) {d_A = Matrix::eye(d_dim);}
Individual(Matrix u, Individual &parent): d_dim(u.size()), d_u(u), d_A(parent.d_A), d_z(parent.d_z), d_sigma(parent.d_sigma), d_fitness(0), d_parent(&parent) {}
  

  Individual offspring() {
	d_parent = 0; //no more parent for you
	d_z = rngGauss(d_dim);
	Matrix x = d_u + d_sigma * d_A * d_z;
	Individual individual(x, *this);
	return individual;
  }

  void set_fitness(double fitness) {d_fitness = fitness;}
  double get_fitness() const {return d_fitness;}

  void set_workspace(std::vector<double> workspace) {d_workspace = workspace;}
  
  void update_win_parent(double nsigma_plus) {
	d_sigma *= exp(nsigma_plus);
  }

  void update_win_offspring(double na, double nsigma_plus) {
	Matrix Ga = d_z * d_z.T() - Matrix::eye(d_z.size());
	//std::cout << d_u.size() << " " << d_A.size() << " " << d_z.size() << std::endl;
	d_u = d_u + d_A * d_z;
	d_A = d_A * (na * Ga).exp();
	d_sigma *= exp(nsigma_plus);
  }

  void update_lose(double nsigma_min) {
	d_sigma /= exp(nsigma_min);
  }

  void update_if_has_parent(double na, double nsigma_plus, double nsigma_min);

  void print() {d_u.print();}
};

struct MoNes {
  Function &d_fitness_function;
  //WorkspaceFunction &d_workspace_function;


  size_t d_dim, d_n;
  std::vector<Individual> d_individuals;
  
  double d_na, d_nsigma_plus, d_nsigma_min;

  MoNes(Function &fitness_function, size_t dim, size_t);

  void init(std::vector<double> mu, double sigma) {
	
	Individual individual(mu, sigma);
	for (size_t i(0); i < d_n; ++i)
	  d_individuals.push_back(individual.offspring());
  }

  Matrix bestPoint() {return d_individuals[0].d_u;}
  double bestFitness() {return d_individuals[0].get_fitness();}

  void evaluate() {
	for (size_t i(0); i < d_individuals.size(); ++i)
	  d_individuals[i].set_fitness(d_fitness_function.eval(d_individuals[i].d_u));
  }

  void rank() {
	sort(d_individuals, &Individual::get_fitness);
  }
  
  void reproduce() {
	for (size_t i(0); i < d_n; ++i)
	  d_individuals.push_back(d_individuals[i].offspring());
  }

  void select() {
	std::cout << d_individuals.size() << " " << d_n << std::endl;
	d_individuals.erase(d_individuals.begin() + d_n, d_individuals.end());
	std::cout << d_individuals.size() << std::endl;
  }

  void update() {
	for (size_t i(0); i < d_individuals.size(); ++i)
	  d_individuals[i].update_if_has_parent(d_na, d_nsigma_plus, d_nsigma_min);
  }

  void iterate() {
	std::cout << "best fitness: " << bestFitness() << std::endl;

	reproduce();
	evaluate();
	rank();
	update();
	select();
  }
  
};

#endif
