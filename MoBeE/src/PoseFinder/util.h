#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <vector>

template <typename T>
inline void print_vector(std::vector<T> vec) {
  for (size_t i(0); i < vec.size(); ++i)
    std::cout << vec[i] << " ";
  std::cout << std::endl;
}

struct LessDereference {
  template <class T>
  bool operator()(const T * lhs, const T * rhs) const {
    return *lhs < *rhs;
  }
};

#endif
