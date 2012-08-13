#include "mones.h"
#include <ctime>

using namespace std;

MoNes::MoNes(Function &fitness_function, WorkspaceFunction &workspace_function, size_t dim, size_t n)
  : d_fitness_function(fitness_function), 
	d_workspace_function(workspace_function),
	d_dim(dim), d_n(n) {
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


void MoNes::rank() {
	//sort individuals in bins
  typedef map<int, vector<Individual> > map_type;
	map_type bin_map;
	for (size_t i(0); i < d_individuals.size(); ++i) {
	  double val = d_individuals[i].d_workspace[1];
	  int index = 0;//val < -1 ? -1 : (val < .0 ? 0 : 1);

	  if (!bin_map.count(index))
		bin_map[index] = vector<Individual>();
	  bin_map[index].push_back(d_individuals[i]);
	}
	
	//sort bins
	map_type::iterator it(bin_map.begin());

	for (; it != bin_map.end(); ++it)
	  sort(it->second, &Individual::get_fitness);
	
	//assign ranks
	vector<vector<Individual>::iterator> iterators, ends;
	it = bin_map.begin();
	for (; it != bin_map.end(); ++it) {
	  iterators.push_back(it->second.begin());
	  ends.push_back(it->second.end());
	}

	map<size_t, size_t> id_rank_map;
	size_t rank(1);
	while (true) {
	  bool was_set(false);
	  for (size_t i(0); i < iterators.size(); ++i) {
		if (iterators[i] == ends[i]) continue;
		id_rank_map[iterators[i]->d_id] = rank++;
		++iterators[i];
		was_set = true;
	  }
	  if (!was_set) break;
	}

	/*
	sort(d_individuals, &Individual::get_fitness);
	
	for (size_t i(0); i < d_individuals.size(); ++i)
	  d_individuals[i].d_rank = i;

	 
	
	map<size_t, size_t> id_rank_map;
	for (size_t i(0); i < d_individuals.size(); ++i)
	  if (d_individuals[i].d_id)
	    id_rank_map[d_individuals[i].d_id] = d_individuals[i].d_rank;
	*/
	
	//set ids and parent ids
	for (size_t i(0); i < d_individuals.size(); ++i)
	  d_individuals[i].d_rank = id_rank_map[d_individuals[i].d_id];

	sort(d_individuals, &Individual::get_rank);

	for (size_t i(0); i < d_individuals.size(); ++i)
	  if (d_individuals[i].d_parent_id)
		d_individuals[i].d_parent_rank = id_rank_map[d_individuals[i].d_parent_id];
	
  }
  

void MoNes::init(std::vector<double> mu, double sigma) {
  
  Individual individual(mu, sigma);
  
  for (size_t i(0); i < d_n; ++i) 
	d_individuals.push_back(individual.offspring());
  
  //print_centers();
  //exit(1);
}
