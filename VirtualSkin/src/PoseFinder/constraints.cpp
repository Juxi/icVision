#include "constraints.h"
#include <iostream>
#include "yarp_misc.h"
#include <QString>

#include <algorithm>
#include <numeric>
#include <iterator>
#include <math.h>
#include "util.h"
#include "model.h"

#include "general.h"


using namespace std;
using namespace yarp::os;

//using namespace yarp::dev;

pair<Constraint*, double> ConstraintFactory::constraint_from_bottle(yarp::os::Bottle &bottle, yarp::os::Property &main) {
	Value &type_b = bottle.find("type");
	string type = type_b.asString().c_str();
	cout << "type: [" << type << "]" << endl;
	pair<Constraint*, double> constraint_weight;

	if (type == "homepose") {
		//double weight = bottle.find("weight").asDouble();
		double weight = find_and_check<double>(bottle, "weight");
		string pose_name = find_and_check<string>(bottle, "pose");
		string mask_name = find_and_check<string>(bottle, "mask");
		vector<double> home_pos = group_to_vector(main.findGroup(pose_name.c_str()).findGroup("pose"));
		vector<double> mask = group_to_vector(main.findGroup(mask_name.c_str()).findGroup("pose"));

		cout << "home pose: [";
		for (size_t i(0); i < home_pos.size(); ++i)
			cout << home_pos[i] << " ";
		cout << "] = " << home_pos.size() << endl;

		cout << "mask: [";
		for (size_t i(0); i < mask.size(); ++i)
			cout << mask[i] << " ";
		cout << "] = " << mask.size() << endl;
		constraint_weight.first = new HomePoseConstraint(home_pos, mask);
		constraint_weight.second = weight;
		cout << weight << endl;
	} else if (type == "collision") {
		double weight = find_and_check<double>(bottle, "weight");
		//double weight = bottle.find("weight").asDouble();

		constraint_weight.first = new CollisionConstraint();
		constraint_weight.second = weight;
		cout << weight << endl;
	} else if (type == "position") {
		double weight = find_and_check<double>(bottle, "weight");
		//double weight = bottle.find("weight").asDouble();
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		string marker = find_and_check<string>(bottle, "marker");

		constraint_weight.first = new PositionConstraint(marker, pos);
		constraint_weight.second = weight;

		cout << weight << " " << marker << endl;
	} else if (type == "averageposition") {
		double weight = find_and_check<double>(bottle, "weight");
		//double weight = bottle.find("weight").asDouble();
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		string marker1 = find_and_check<string>(bottle, "marker1");
		string marker2 = find_and_check<string>(bottle, "marker2");

		constraint_weight.first = new AveragePositionConstraint(marker1, marker2, pos);
		constraint_weight.second = weight;

		cout << weight << " " << marker1 << " " << marker2 << endl;
	} else if (type == "plane") {
		double weight = find_and_check<double>(bottle, "weight");
		//double weight = bottle.find("weight").asDouble();
		string marker = find_and_check<string>(bottle, "marker");

		double val = find_and_check<double>(bottle, "pos");
		int axis = find_and_check<int>(bottle, "axis");

		constraint_weight.first = new PlaneConstraint(marker, axis, val);
		constraint_weight.second = weight;

		cout << weight << " " << marker << " " << val << " " << axis << endl;
	} else if (type == "orientation") {
		double weight = find_and_check<double>(bottle, "weight");
		//double weight = bottle.find("weight").asDouble();
		vector<double> orientation = group_to_vector(bottle.findGroup("orientation"));
		string marker = find_and_check<string>(bottle, "marker");
		int axis = find_and_check<int>(bottle, "axis");

		constraint_weight.first = new OrientationConstraint(marker, axis, orientation);
		constraint_weight.second = weight;

		cout << weight << " " << marker << " " << axis << endl;
	} else if (type == "pointing") {
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
	} else if (type == "pointingmarker") {
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
	} else if (type == "opposite") {
		double weight = find_and_check<double>(bottle, "weight");
		//double weight = bottle.find("weight").asDouble();
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));

		string marker1 = find_and_check<string>(bottle, "marker1");
		string marker2 = find_and_check<string>(bottle, "marker2");

		constraint_weight.first = new OppositeConstraint(marker1, marker2, pos);
		constraint_weight.second = weight;

		cout << weight << " " << marker1 << " " << marker2 << endl;
	} else if (type == "grasp") {
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


static double pos_error(std::vector<double> const &values, std::vector<double> const &goal_pos) {
	double distance(0.0);
	for (size_t i(0); i < values.size(); ++i)
		distance += pow(values[i] - goal_pos[i], 2.0);
	return sqrt(distance);
}

static double pos_error(std::vector<double> const &values, std::vector<double> const &goal_pos, std::vector<double> const &mask) {
	double distance(0.0);
	for (size_t i(0); i < values.size(); ++i)
		distance += pow(values[i] - goal_pos[i], 2.0) * mask[i];
	return sqrt(distance);
}

static std::vector<double> diff(std::vector<double> const &left, std::vector<double> const &right) {
	std::vector<double> diff(left);
	for (size_t i(0); i < diff.size(); ++i)
		diff[i] -= right[i];
	return diff;
}

static double norm_in_product(std::vector<double> const &left, std::vector<double> const &right) {
	double total(0), total_left(0), total_right(0);
	for (size_t i(0); i < left.size(); ++i) {
		total += left[i] * right[i];
		total_left += left[i] * left[i];
		total_right += right[i] * right[i];
	}
	return total / sqrt(total_left) / sqrt(total_right);
}


HomePoseConstraint::HomePoseConstraint(std::vector<double> home_pose) : 
Constraint("HomePoseConstraint"), 
	d_home_pose(home_pose), 
	d_home_pose_mask(d_home_pose.size()) 
{ 
	std::fill(d_home_pose_mask.begin(), d_home_pose_mask.end(), 1.0);
}


HomePoseConstraint::HomePoseConstraint(std::vector<double> home_pose, std::vector<double> home_pose_mask) : 
Constraint("HomePoseConstraint"), 
	d_home_pose(home_pose), 
	d_home_pose_mask(home_pose_mask) 
{}

double HomePoseConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return pos_error(d_home_pose, motor_values, d_home_pose_mask) / d_home_pose.size();
}

void HomePoseConstraint::post_hook(Simulator &simulator) {
	d_home_pose = simulator.real_to_normal_motors(d_home_pose);
}

CollisionConstraint::CollisionConstraint() : Constraint("CollisionConstraint") {}

double CollisionConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return collisions;
}


PositionConstraint::PositionConstraint(std::string marker_name, std::vector<double> goal_position) : 
Constraint("PositionConstraint"), 
	d_marker_name(marker_name), 
	d_goal_position(goal_position) 
{}

double PositionConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	std::vector<double> position = observation.markerPosition(QString(d_marker_name.c_str()));
	return pos_error(position, d_goal_position);
}


AveragePositionConstraint::AveragePositionConstraint(std::string marker_name, std::string marker_name2, std::vector<double> goal_position) : 
Constraint("AveragePositionConstraint"),
	d_marker_name(marker_name), 
	d_marker_name2(marker_name2), 
	d_goal_position(goal_position) 
{}

double AveragePositionConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	std::vector<double> position = observation.markerPosition(QString(d_marker_name.c_str()));
	std::vector<double> position2 = observation.markerPosition(QString(d_marker_name2.c_str()));
	for (size_t i(0); i < position.size(); ++i)
		position[i] = (position[i] + position2[i]) / 2.0;
	return pos_error(position, d_goal_position);
}


PlaneConstraint::PlaneConstraint(std::string marker_name, size_t axis, double value) :
Constraint("PlaneConstraint"),
	d_marker_name(marker_name),
	d_axis(axis),
	d_value(value)
{}

double PlaneConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	std::vector<double> position = observation.markerPosition(QString(d_marker_name.c_str()));
	return fabs(position[d_axis] - d_value);
}


OrientationConstraint::OrientationConstraint(std::string marker_name, size_t axis, std::vector<double> goal_orientation) :
Constraint("OrientationConstraint"),
	d_marker_name(marker_name), 
	d_goal_orientation(9), 
	d_mask_orientation(9), 
	d_axis(axis) 
{
	assert(goal_orientation.size() == 3 && d_axis < 3);
	copy(goal_orientation.begin(), goal_orientation.end(), d_goal_orientation.begin() + d_axis * 3);
	fill(d_mask_orientation.begin() + axis * 3, d_mask_orientation.begin() + d_axis * 3 + 3, 1.0);
}

double OrientationConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return observation.orientationMeasure(QString(d_marker_name.c_str()), d_goal_orientation, d_mask_orientation);
}

PointingConstraint::PointingConstraint(std::string marker, std::vector<double> goal_position, double distance, size_t axis, size_t dir)	: 
Constraint("PointingConstraint"),
	d_marker(marker),
	d_goal_position(goal_position),
	d_distance(distance),
	d_axis(axis),
	d_dir(dir)
{}

double PointingConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	std::vector<double> goal_orientation(9);
	std::vector<double> mask_orientation(9);

	std::vector<double> position_marker = observation.markerPosition(QString(d_marker.c_str()));
	std::vector<double> marker_diff(diff(d_goal_position, position_marker));
	double marker_distance = pos_error(d_goal_position, position_marker);
	double marker_distance_measure = fabs(marker_distance - d_distance);

	copy(marker_diff.begin(), marker_diff.end(), goal_orientation.begin() + 3 * d_axis);
	fill(mask_orientation.begin() + 3 * d_axis, mask_orientation.begin() + 3 * d_axis + 3, 1.);
	double marker_to_object = observation.orientationMeasure(QString(d_marker.c_str()), goal_orientation, mask_orientation);
	if (d_distance)
		return 10. * marker_distance_measure + marker_to_object;
	else
		return marker_to_object;
}

PointingMarkerConstraint::PointingMarkerConstraint(std::string marker, std::string target, double distance, size_t axis, size_t dir) : 
Constraint("PointingMarkerConstraint"),
	d_marker(marker),
	d_target(target),
	d_distance(distance),
	d_axis(axis),
	d_dir(dir)
{}

double PointingMarkerConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	std::vector<double> goal_orientation(9);
	std::vector<double> mask_orientation(9);

	std::vector<double> position_marker = observation.markerPosition(QString(d_marker.c_str()));
	std::vector<double> target_pos = observation.markerPosition(QString(d_target.c_str()));
	std::vector<double> marker_diff(diff(target_pos, position_marker));
	double marker_distance = pos_error(target_pos, position_marker);
	double marker_distance_measure = fabs(marker_distance - d_distance);

	copy(marker_diff.begin(), marker_diff.end(), goal_orientation.begin() + 3 * d_axis);
	for (size_t i(0); i < goal_orientation.size(); ++i)
		goal_orientation[i] *= d_dir;
	fill(mask_orientation.begin() + 3 * d_axis, mask_orientation.begin() + 3 * d_axis + 3, 1.);
	double marker_to_object = observation.orientationMeasure(QString(d_marker.c_str()), goal_orientation, mask_orientation);
	if (d_distance)
		return 10. * marker_distance_measure + marker_to_object;
	else
		return marker_to_object;
}


OppositeConstraint::OppositeConstraint(std::string marker1, std::string marker2, std::vector<double> goal_position) :
Constraint("OppositeConstraint"),
	d_marker1(marker1),
	d_marker2(marker2),
	d_goal_position(goal_position)
{}

double OppositeConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	std::vector<double> position_marker1 = observation.markerPosition(QString(d_marker1.c_str()));
	std::vector<double> position_marker2 = observation.markerPosition(QString(d_marker2.c_str()));

	std::vector<double> marker1_diff(diff(d_goal_position, position_marker1));
	std::vector<double> marker2_diff(diff(d_goal_position, position_marker2));

	return (norm_in_product(marker1_diff, marker2_diff) + 1.0) / 2.0;
}


GraspConstraint::GraspConstraint(std::string marker_1, std::string marker_2, size_t axis1, size_t axis2, double distance, std::vector<double> goal_position, double factor) :
Constraint("GraspConstraint"),
	d_marker_1(marker_1), d_marker_2(marker_2),
	d_point_constraint1(marker_1, goal_position, distance, axis1),
	d_point_constraint2(marker_2, goal_position, distance, axis2),
	d_opposite_constraint(marker_1, marker_2, goal_position),
	d_factor(factor)
{}


double GraspConstraint::evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return d_point_constraint1.evaluate(motor_values, observation, collisions) +
		d_point_constraint2.evaluate(motor_values, observation, collisions) +
		d_factor * d_opposite_constraint.evaluate(motor_values, observation, collisions);
}

MapBuildConstraint::MapBuildConstraint(std::string marker, size_t nNN, double alpha, double config_measure, double exploration_force) :
Constraint("MapBuildConstraint"),
	d_marker(marker),
	d_nNN(nNN),
	d_alpha(alpha),
	d_config_measure(config_measure),
	d_exploration_force(exploration_force)
{}


void MapBuildConstraint::add_point(std::vector<double> &x, std::vector<double> &q) {
	d_points.push_back(x);
	d_configurations.push_back(q);
}

void MapBuildConstraint::add_points(std::vector<std::vector<double> > xs, std::vector<std::vector<double> > qs) {
	std::cout << "=> " << xs.size();
	std::cout << " " <<
		qs.size() << " ";
	std::cout << d_points.size() << " ";

	std::cout << d_configurations.size() << std::endl;
	std::copy(xs.begin(), xs.end(), back_inserter(d_points));
	std::copy(qs.begin(), qs.end(), back_inserter(d_configurations));
}

double MapBuildConstraint::close_measure(std::vector<double> &values, double alpha, std::vector<size_t> &indexes) const {
	double total_dist(0);
	std::vector<double> distances(d_points.size());
	for (size_t i(0); i < d_points.size(); ++i) {
		total_dist += distances[i] = pos_error(values, d_points[i]);
	}

	std::vector<double*> distances_ptrs(distances.size());
	std::vector<double>::iterator it(distances.begin()), it_end(distances.end());
	std::vector<double*>::iterator it2(distances_ptrs.begin());
	for (; it != it_end; ++it, ++it2)
		*it2 = &(*it);

	sort(distances_ptrs.begin(), distances_ptrs.end(), LessDereference());
	int n_distances = distances.size() >= d_nNN ? d_nNN : distances.size();

	//double total_dist = std::accumulate(distances.begin(), distances.end(), 0.0);
	indexes.resize(n_distances);
	double measure(0.0);
	for (size_t i(0); i < n_distances; ++i)  {
		measure += fabs(*distances_ptrs[i] - alpha);// * (distances[i] - alpha); //or use abs?
		indexes[i] = distances_ptrs[i] - &distances[0];
	}

	//std::cout << measure << " " << exp(-total_dist) << " " << total_dist << std::endl;
	if (d_exploration_force)
		return measure + exp(-total_dist * d_exploration_force);
	else
		return measure;
}


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

