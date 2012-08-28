#include "constraints.h"
#include <iostream>
#include "yarp_misc.h"

using namespace std;
using namespace yarp::os;

//using namespace yarp::dev;

double MapBuildConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
  vector<double> position = observation.markerPosition(QString(d_marker.c_str()));
  vector<size_t> indexes;
  float measure(0.0);
  
  float map_build_measure = close_measure(position, d_alpha, indexes);
  measure += map_build_measure;
  if (d_config_measure > 0.0) {
    measure += d_config_measure * config_measure(motor_values, indexes);
    
    float configuration_measure = config_measure(motor_values, indexes);
    //cout << map_build_measure << " " << configuration_measure << endl;
  }
  return measure;
}

double MapBuildConstraint::config_measure(std::vector<double> &values, std::vector<size_t> &indexes) const {
  double measure(0.0);

  for (size_t i(0); i < indexes.size(); ++i) 
    measure += pos_error(values, d_configurations[indexes[i]]);	  
  return measure;
}



pair<Constraint*, double> ConstraintFactory::constraint_from_bottle(yarp::os::Bottle &bottle, yarp::os::Property &main) {
  Value &type_b = bottle.find("type");
  string type = type_b.asString().c_str();
  cout << "type: [" << type << "]" << endl;
  pair<Constraint*, double> constraint_weight;
  
  if (type == "homepose") {
    //double weight = bottle.find("weight").asDouble();
    double weight = find_and_check<double>(bottle, "weight");
    string pose_name = find_and_check<string>(bottle, "pose");
    vector<double> home_pos = group_to_vector(main.findGroup(pose_name.c_str()).findGroup("pose"));
    cout << "home pose: [";
    for (size_t i(0); i < home_pos.size(); ++i)
      cout << home_pos[i] << " ";
    cout << "]" << endl;
    constraint_weight.first = new HomePoseConstraint(home_pos);
    constraint_weight.second = weight;
    cout << weight << endl;
  } else

  if (type == "collision") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    
    constraint_weight.first = new CollisionConstraint();
    constraint_weight.second = weight;
    cout << weight << endl;
  } else

  if (type == "position") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> pos = group_to_vector(bottle.findGroup("pos"));
    string marker = find_and_check<string>(bottle, "marker");

    constraint_weight.first = new PositionConstraint(marker, pos);
    constraint_weight.second = weight;

    cout << weight << " " << marker << endl;
  } else

  if (type == "averageposition") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> pos = group_to_vector(bottle.findGroup("pos"));
    string marker1 = find_and_check<string>(bottle, "marker1");
    string marker2 = find_and_check<string>(bottle, "marker2");

    constraint_weight.first = new AveragePositionConstraint(marker1, marker2, pos);
    constraint_weight.second = weight;

    cout << weight << " " << marker1 << " " << marker2 << endl;
  } else

  if (type == "plane") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    string marker = find_and_check<string>(bottle, "marker");

    double val = find_and_check<double>(bottle, "pos");
    int axis = find_and_check<int>(bottle, "axis");

    constraint_weight.first = new PlaneConstraint(marker, axis, val);
    constraint_weight.second = weight;

    cout << weight << " " << marker << " " << val << " " << axis << endl;
  } else

  if (type == "orientation") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> orientation = group_to_vector(bottle.findGroup("orientation"));
    string marker = find_and_check<string>(bottle, "marker");
    int axis = find_and_check<int>(bottle, "axis");

    constraint_weight.first = new OrientationConstraint(marker, axis, orientation);
    constraint_weight.second = weight;

    cout << weight << " " << marker << " " << axis << endl;
  } else

  if (type == "pointing") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> pos = group_to_vector(bottle.findGroup("pos"));
    string marker = find_and_check<string>(bottle, "marker");
    double distance = find_and_check<double>(bottle, "distance");
    int axis = find_and_check<int>(bottle, "axis");
    int dir = find_and_check<int>(bottle, "dir");

    constraint_weight.first = new PointingConstraint(marker, pos, distance, axis, dir);
    constraint_weight.second = weight;

    cout << weight << " " << marker << " " << distance << " " << axis << " " << dir << endl;
  } else

  if (type == "pointingmarker") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> pos = group_to_vector(bottle.findGroup("pos"));
    string marker = find_and_check<string>(bottle, "marker");
    string point_marker = find_and_check<string>(bottle, "pointmarker");

    double distance = find_and_check<double>(bottle, "distance");
    int axis = find_and_check<int>(bottle, "axis");
    int dir = find_and_check<int>(bottle, "dir");

    constraint_weight.first = new PointingMarkerConstraint(marker, point_marker, distance, axis, dir);
    constraint_weight.second = weight;

    cout << weight << " " << marker << " " << point_marker << " " << distance << " " << axis << " " << dir << endl;
  } else

  if (type == "opposite") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> pos = group_to_vector(bottle.findGroup("pos"));

    string marker1 = find_and_check<string>(bottle, "marker1");
    string marker2 = find_and_check<string>(bottle, "marker2");

    constraint_weight.first = new OppositeConstraint(marker1, marker2, pos);
    constraint_weight.second = weight;

    cout << weight << " " << marker1 << " " << marker2 << endl;
  } else

  if (type == "grasp") {
    double weight = find_and_check<double>(bottle, "weight");
    //double weight = bottle.find("weight").asDouble();
    vector<double> pos = group_to_vector(bottle.findGroup("pos"));

    string marker1 = find_and_check<string>(bottle, "marker1");
    string marker2 = find_and_check<string>(bottle, "marker2");

    int index1 = find_and_check<int>(bottle, "index1");
    int index2 = find_and_check<int>(bottle, "index2");

    double distance = find_and_check<double>(bottle, "distance");

    constraint_weight.first = new GraspConstraint(marker1, marker2, index1, index2, distance, pos);
    constraint_weight.second = weight;

    cout << weight << " " << marker2 << " " << index1 << " " << index2 << " " << distance << endl;
  } else
    throw StringException(type + "no such type of constraint ");

  return constraint_weight;
}
