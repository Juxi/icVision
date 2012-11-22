#ifndef __SPRINTF_H__
#define __SPRINTF_H__

#include <string>
#include <sstream>

template <typename T1>
inline std::string Sprintf(T1 input1) {
  std::ostringstream oss;
  oss << input1 << " " << endl;
  return oss.str();
}

template <typename T1, typename T2>
  inline std::string Sprintf(T1 input1, T2 input2) {
  std::ostringstream oss;
  oss << input1 << " " << input2 << " " << endl;
  return oss.str();
}


template <typename T1, typename T2, typename T3>
  inline std::string Sprintf(T1 input1, T2 input2, T3 input3) {
  std::ostringstream oss;
  oss << input1 << " " << input2 << " " << input3 << " " << endl;
  return oss.str();
}

#endif
