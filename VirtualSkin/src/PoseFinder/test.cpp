#include "mones.h"
#include <iostream>
#include <vector>

using namespace std;

class TestFunction : public Function {
  std::vector<double> d_goal;
  std::vector<double> d_workspace;

 public:
  TestFunction(std::vector<double> goal) : d_goal(goal){}
  virtual double eval(const Matrix& point) {
	double cost(0);
	for (size_t i(0); i < d_goal.size(); ++i)
	  cost += (point[i] - d_goal[i]) * (point[i] - d_goal[i]);
	d_workspace = std::vector<double>(1);
	d_workspace[0] = point[0];
	return cost;
  }

  std::vector<double> &workspace() {
	return d_workspace;
  }
};

struct TestWorkspaceFunction : public WorkspaceFunction {
  virtual std::vector<double> get_workspace(Function &fitness_funtion) {
	return dynamic_cast<TestFunction&>(fitness_funtion).workspace();
  }
};

inline void mones_test2() {
  std::vector<double> goal;
  goal.push_back(3);
  goal.push_back(2);
  goal.push_back(1);
  TestFunction test_function(goal);

  cout << "start" << endl;
  std::vector<double> start(3);
  
  TestWorkspaceFunction test_workspace_function;
  MoNes mones(test_function, test_workspace_function, 3, 10);
  cout << "start" << endl;
  mones.init(start, 1);

  cout << "iterations" << endl;
  for (size_t i(0); i < 100; ++i) {
	//cout << "iterate" << endl;
	mones.iterate();
  }
  
  mones.print_fitnesses();

  mones.d_individuals[0].print();
  cout << "dsfdsf" << endl;
  mones.d_individuals[1].print();
  
  mones.d_individuals[2].print();
  
}

inline void mones_test() {
  std::vector<double> goal;
  goal.push_back(3);
  goal.push_back(2);
  goal.push_back(1);
  TestFunction test_function(goal);
  Individual ind(Matrix::ones(3), 1);


  for (size_t i(0); i < 1000; ++i) {
	Individual offspring(ind.offspring());
	double fit_p = test_function.eval(ind.d_u);
	double fit_o = test_function.eval(offspring.d_u);
	if (fit_o < fit_p) {
	  offspring.update_win_offspring(.2, .2);
	  ind = offspring;
	  //ind.update_win(.2, .2);
	} else {
	  ind.update_lose(.05);
	}

	std::cout << fit_o << " " << fit_p << ":" << std::endl;
  }

  ind.print();
}

int main() {
  mones_test2();
}
