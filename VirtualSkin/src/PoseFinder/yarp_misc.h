#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <vector>
#include <iostream>
#include <string>

#include "exception.h"


inline std::vector<double> group_to_vector(yarp::os::Bottle &val) {
	std::vector<double> values;
    std::cout << val.toString().c_str() << std::endl;
    //if (!val.isList()) { throw StringException("Value not a list"); }
    ///yarp::os::Bottle* val_pointer = val.asList();

    for (size_t i(1); i < val.size(); ++i) {
      values.push_back(val.get(i).asDouble());
    }
    return values;
}


template <typename T>
inline T find_and_check(yarp::os::Bottle &bottle, std::string name){}

template <>
inline std::string find_and_check<std::string>(yarp::os::Bottle &bottle, std::string name) {
  yarp::os::Value &val = bottle.find(name.c_str());
  if (val.isNull())
    throw StringException(name + " not found");
  if (!val.isString())
    throw StringException(name + " is not a string");
  return val.asString().c_str();
}


template <>
inline int find_and_check<int>(yarp::os::Bottle &bottle, std::string name) {
  yarp::os::Value &val = bottle.find(name.c_str());
  if (val.isNull())
    throw StringException(name + " not found");
  if (!val.isInt())
    throw StringException(name + " is not an int");
  return val.asInt();
}



template <>
inline double find_and_check<double>(yarp::os::Bottle &bottle, std::string name) {
  yarp::os::Value &val = bottle.find(name.c_str());
  if (val.isNull())
    throw StringException(name + " not found");
  if (!(val.isDouble() || val.isInt()))
    throw StringException(name + " is not a double");
  return val.asDouble();
}

