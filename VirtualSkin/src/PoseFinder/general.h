#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include <iostream>


enum TreeMode {
	WORKSPACE = 0,
	CONFIGURATIONSPACE = 1,
	SCALEDCONFIGURATIONSPACE = 2,
	CONNECTIONMODE = 3,
	MAXCONFIGURATIONSPACE = 4
};


template <typename T>
inline std::vector<T> string_to_vector(std::string str) {
	std::vector<T> vec;
	std::istringstream iss(str.c_str());
	while (true) {
		T val;
		iss >> val;
		if (!iss)
			break;
		vec.push_back(val);
	}
	return vec;
}


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
struct CompareClass2 {
  typedef double (T::*value_func_t)()const;
  value_func_t d_value_func;
  value_func_t d_value_func2;

CompareClass2(value_func_t value_func, value_func_t value_func2) : d_value_func(value_func), d_value_func2(value_func2){}


  bool operator()(T const &e1, T const &e2) const {
	if ((e1.*d_value_func)() < (e2.*d_value_func)())
	  return true;
	if ((e1.*d_value_func)() > (e2.*d_value_func)())
	  return false;
	return ((e1.*d_value_func2)() < (e2.*d_value_func2)());
  }
};

template <typename T>
void sort(std::vector<T> &vec, typename CompareClass<T>::value_func_t value_func) {
  std::sort(vec.begin(), vec.end(), CompareClass<T>(value_func));
}

template <typename T>
void sort(std::vector<T> &vec, typename CompareClass2<T>::value_func_t value_func, typename CompareClass2<T>::value_func_t value_func2) {
  std::sort(vec.begin(), vec.end(), CompareClass2<T>(value_func, value_func2));
}

inline double calculate_distance( std::vector<double> const &v1, std::vector<double> const &v2) {
	assert(v1.size() == v2.size());
	double distance(0.0), diff;
	std::vector<double>::const_iterator it(v1.begin()), it_end(v1.end()), it2(v2.begin());
	
	for (; it != it_end; ++it, ++it2) {
	  //distance += std::pow(*it - *it2, 2.0); // pow is slow!
		diff = *it - *it2;
	    distance += diff * diff;
	}
	return std::sqrt(distance);
}

inline void write_vector(std::vector<double> v1, std::ostream &out = std::cout) {
  for (size_t i(0); i < v1.size(); ++i)
	out << v1[i] << " ";
}

#endif
