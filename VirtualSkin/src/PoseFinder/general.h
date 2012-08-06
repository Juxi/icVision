#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>

template <typename T>
struct CompareClass {
  typedef double (T::*value_func_t)()const;
  value_func_t d_value_func;

CompareClass(value_func_t value_func) : d_value_func(value_func){}


  bool operator()(T const &e1, T const &e2) const {
	return (e1.*d_value_func)() < (e2.*d_value_func)();
  }
};

template <typename T>
void sort(std::vector<T> &vec, typename CompareClass<T>::value_func_t value_func) {
  std::sort(vec.begin(), vec.end(), CompareClass<T>(value_func));
}

inline double calculate_distance( std::vector<double> const &v1, std::vector<double> const &v2) {
	assert(v1.size() == v2.size());
	double distance(0.0);
	std::vector<double>::const_iterator it(v1.begin()), it_end(v1.end()), it2(v2.begin());
	
	for (; it != it_end; ++it, ++it2)
	  distance += std::pow(*it - *it2, 2.0);
	return std::sqrt(distance);
}

#endif
