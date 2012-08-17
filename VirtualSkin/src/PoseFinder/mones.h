#ifndef __MONES_H__
#define __MONES_H__

#include "matrix.h"
#include "function.h"
#include "general.h"

#include <vector>
#include <iostream>
#include <map>

struct Individual {
  size_t d_dim;
  Matrix d_u, d_A;
  Matrix d_z;
  double d_sigma;
  double d_fitness;
  double d_domination_factor;
  //double d_parent_fitness;
  size_t d_id, d_parent_id;
  size_t d_rank, d_parent_rank;
  std::vector<double> d_workspace;

Individual(Matrix u, double sigma): d_dim(u.size()), d_u(u), d_z(Matrix::zeros(d_dim)), 
	d_sigma(sigma), d_fitness(0), d_domination_factor(0),
	d_id(0), d_parent_id(0),
	d_rank(0), d_parent_rank(0) 
  {d_A = Matrix::eye(d_dim);}

Individual(Matrix u, Individual &parent): d_dim(u.size()), d_u(u), d_A(parent.d_A), d_z(parent.d_z), 
	d_sigma(parent.d_sigma), d_fitness(0), d_domination_factor(0),
	d_id(0), d_parent_id(parent.d_id),
	d_rank(0), d_parent_rank(0)
  {}
  

  Individual offspring() {
	d_z = rngGauss(d_dim);
	
	Matrix x = d_u + d_sigma * d_A * d_z;
	Individual individual(x, *this);
	d_parent_id = 0;
	return individual;
  }

  void set_fitness(double fitness) {d_fitness = fitness;}
  double get_fitness() const {return d_fitness;}
  double get_rank() const {return d_rank;}
  double get_domination_factor() const {return d_domination_factor;}

  void set_workspace(std::vector<double> workspace) {d_workspace = workspace;}
  std::vector<double> &get_workspace() {return d_workspace;}
  
  void update_win_parent(double nsigma_plus) {
	//std::cout << "win parent" << std::endl;
	d_sigma *= exp(nsigma_plus);
  }

  double &domination_factor() {return d_domination_factor;}

  void update_win_offspring(double na, double nsigma_plus) {
	//std::cout << "win offspring" << std::endl;
	Matrix Ga = d_z * d_z.T() - Matrix::eye(d_z.size());
	//std::cout << d_u.size() << " " << d_A.size() << " " << d_z.size() << std::endl;

	d_u = d_u + d_sigma * d_A * d_z;
	//d_u = d_u + d_A * d_z;
	d_A = d_A * (na * Ga).exp();
	d_sigma *= exp(nsigma_plus);
  }

  void update_lose(double nsigma_min) {
	//std::cout << "lose" << std::endl;
	d_sigma /= exp(nsigma_min);
  }

  //void update_if_has_parent(double na, double nsigma_plus, double nsigma_min);

  void print() {d_u.print();}
};


struct MoNes {
  Function &d_fitness_function;
  WorkspaceFunction &d_workspace_function;
  //WorkspaceFunction &d_workspace_function;


  size_t d_dim, d_n;
  std::vector<Individual> d_individuals;
  
  double d_na, d_nsigma_plus, d_nsigma_min;

  MoNes(Function &fitness_function, WorkspaceFunction &workspace_function, size_t dim, size_t);

  void init(std::vector<double> mu, double sigma);

  Matrix bestPoint() {return d_individuals[0].d_u;}
  double bestFitness() {return d_individuals[0].get_fitness();}

  void evaluate() {
	for (size_t i(0); i < d_individuals.size(); ++i) {
	  d_individuals[i].set_fitness(d_fitness_function.eval(d_individuals[i].d_u));
	  d_individuals[i].d_workspace = d_workspace_function.get_workspace(d_fitness_function);
	}
  }

  void rank();

  bool dominates(Individual &i1, Individual &i2); //does i1 dominate i2?
  void calculate_domination() {
	for (size_t i(0); i < d_individuals.size(); ++i)
	  d_individuals[i].domination_factor() = 0;

	for (size_t i(0); i < d_individuals.size(); ++i)
	  for (size_t j(0); j < d_individuals.size(); ++j)
		d_individuals[j].domination_factor() += dominates(d_individuals[i], d_individuals[j]);	
  }

  void reproduce() {
	for (size_t i(0); i < d_n; ++i)
	  d_individuals.push_back(d_individuals[i].offspring());
  }

  void select() {
	d_individuals.erase(d_individuals.begin() + d_n, d_individuals.end());
  }

  void print_fitnesses() {
	std::cout << std::endl;
	for (size_t i(0); i < d_individuals.size(); ++i)
	  std::cout << "=" << d_individuals[i].get_fitness() << " "  << d_individuals[i].get_rank() << " " << d_individuals[i].d_id << std::endl;
  }

  void print_centers() {
	for (size_t i(0); i < d_individuals.size(); ++i) {
 	  std::cout << "=> ";
	  d_individuals[i].d_u.print();
	}
  }

  void print_domination_factors() {
	std::cout << "==>" << std::endl;
	for (size_t i(0); i < d_individuals.size(); ++i)
 	  std::cout << d_individuals[i].domination_factor() << std::endl;
  }

  void update() {
	std::map<size_t, size_t> id_index_map;
	for (size_t i(0); i < d_individuals.size(); ++i)
	  if (d_individuals[i].d_id)
		id_index_map[d_individuals[i].d_id] = i;
	for (size_t i(0); i < d_individuals.size(); ++i) {
	  //std::cout << d_individuals[i].d_parent_id << std::endl;
	  if (d_individuals[i].d_parent_id) {
		if (d_individuals[i].d_rank < d_individuals[i].d_parent_rank) {
		  d_individuals[i].update_win_offspring(d_na, d_nsigma_plus);
		  d_individuals[id_index_map[d_individuals[i].d_parent_id]].update_win_parent(d_nsigma_plus);
		} else {
		  d_individuals[i].update_lose(d_nsigma_min);
		  d_individuals[id_index_map[d_individuals[i].d_parent_id]].update_lose(d_nsigma_min);
		}
	  }
	}
  }

  void set_ids() {
	for (size_t i(0); i < d_individuals.size(); ++i) {
	  d_individuals[i].d_id = i + 1;
	}
  }

  void iterate() {
	set_ids();
	reproduce();
	set_ids();

	evaluate();
	calculate_domination();
	
	rank();
	print_domination_factors();

	//print_fitnesses();
	
	update();
	
	select();
  }
  
};

#endif
