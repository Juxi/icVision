#include "mones.h"
#include <iostream>
#include <vector>

using namespace std;

class TestFunction : public Function {
  std::vector<double> d_goal;
 public:
  TestFunction(std::vector<double> goal) : d_goal(goal){}
  virtual double eval(const Matrix& point) {
	double cost(0);
	for (size_t i(0); i < d_goal.size(); ++i)
	  cost += (point[i] - d_goal[i]) * (point[i] - d_goal[i]);
	return cost;
  }
};

inline void mones_test() {
  rngSeed(time(0));

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
  mones_test();
}
