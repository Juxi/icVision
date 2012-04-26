#include "constraints.h"

using namespace std;

double MapBuildConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
  vector<double> position = observation.markerPosition(QString(d_marker.c_str()));
  vector<size_t> indexes;
  float measure(0.0);
  
  float map_build_measure = close_measure(position, d_alpha, indexes);
  measure += map_build_measure;
  if (d_config_measure > 0.0) {
    measure += d_config_measure * config_measure(motor_values, indexes);
    
    float configuration_measure = config_measure(motor_values, indexes);
    //cout << configuration_measure << endl;
  }
  return measure;
}

double MapBuildConstraint::config_measure(std::vector<double> &values, std::vector<size_t> &indexes) const {
  double measure(0.0);

  for (size_t i(0); i < indexes.size(); ++i) 
    measure += pos_error(values, d_configurations[indexes[i]]);	  
  return measure;
}
