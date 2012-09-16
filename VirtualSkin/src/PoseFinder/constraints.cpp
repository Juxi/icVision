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


static void sprintvector(ostream& o, const string name, const vector<double>& vec, const vector<double>& mask) {
	o << name << ": (" << vec.size() << ") [";
	for (size_t i(0); i < vec.size(); ++i) {
		if (mask[i] == 0) {
			o << "NA ";
		} else {
			o << vec[i] << " ";
		}
	}
	o << "]" << endl;
}

static void sprintvector(ostream&o, const string name, const vector<double>& vec) {
	o << name << ": (" << vec.size() << ") [";
	for (size_t i(0); i < vec.size(); ++i) {
		o << vec[i] << " ";
	} 
	o << "]" << endl;
}

static double pos_error(vector<double> const &values, vector<double> const &goal_pos) {
	double distance(0.0), diff;
	for (size_t i(0); i < values.size(); ++i) {
		diff = values[i] - goal_pos[i];
		distance += diff * diff;
		//distance += pow(values[i] - goal_pos[i], 2.0);//pow=slow
	}
	
	return sqrt(distance);
}

static double pos_error(vector<double> const &values, vector<double> const &goal_pos, vector<double> const &mask) {
	double distance(0.0), diff;
	for (size_t i(0); i < values.size(); ++i) {
		diff = values[i] - goal_pos[i];
		distance += diff * diff * mask[i];
		//distance += pow(values[i] - goal_pos[i], 2.0) * mask[i]; //pow=slow
	}
	return sqrt(distance);
}

static vector<double> diff(vector<double> const &left, vector<double> const &right) {
	vector<double> diff(left);
	for (size_t i(0); i < diff.size(); ++i)
		diff[i] -= right[i];
	return diff;
}

static double norm_in_product(vector<double> const &left, vector<double> const &right) {
	double total(0), total_left(0), total_right(0);
	for (size_t i(0); i < left.size(); ++i) {
		total += left[i] * right[i];
		total_left += left[i] * left[i];
		total_right += right[i] * right[i];
	}
	return total / sqrt(total_left) / sqrt(total_right);
}


pair<Constraint*, double> ConstraintFactory::constraint_from_bottle(yarp::os::Bottle &bottle, yarp::os::Property &main) {
	Value &type_b = bottle.find("type");
	string type = type_b.asString().c_str();
	pair<Constraint*, double> constraint_weight;

	if (type == "homepose") {
		double weight = find_and_check<double>(bottle, "weight");
		string pose_name = find_and_check<string>(bottle, "pose");
		string mask_name = find_and_check<string>(bottle, "mask");
		vector<double> home_pose = group_to_vector(main.findGroup(pose_name.c_str()).findGroup("pose"));
		vector<double> mask = group_to_vector(main.findGroup(mask_name.c_str()).findGroup("mask"));

		constraint_weight.first = new HomePoseConstraint(home_pose, mask);
		constraint_weight.second = weight;
	} else if (type == "fixpose") {
		string pose_name = find_and_check<string>(bottle, "pose");
		string mask_name = find_and_check<string>(bottle, "mask");
		vector<double> fix_pose = group_to_vector(main.findGroup(pose_name.c_str()).findGroup("pose"));
		vector<double> mask = group_to_vector(main.findGroup(mask_name.c_str()).findGroup("mask"));

		constraint_weight.first = new FixPoseConstraint(fix_pose, mask);
		constraint_weight.second = 0.0;
	} else if (type == "limit") {
		string min_name = find_and_check<string>(bottle, "min");
		string max_name = find_and_check<string>(bottle, "max");
		string mask_name = find_and_check<string>(bottle, "mask");
		vector<double> min_pose = group_to_vector(main.findGroup(min_name.c_str()).findGroup("limit"));
		vector<double> max_pose = group_to_vector(main.findGroup(max_name.c_str()).findGroup("limit"));
		vector<double> mask = group_to_vector(main.findGroup(mask_name.c_str()).findGroup("mask"));

		constraint_weight.first = new LimitConstraint(min_pose, max_pose, mask);
		constraint_weight.second = 0.0;
	} else if (type == "startpose") {
		double weight = find_and_check<double>(bottle, "weight");
		string pose_name = find_and_check<string>(bottle, "pose");
		string mask_name = find_and_check<string>(bottle, "mask");
		vector<double> start_pos = group_to_vector(main.findGroup(pose_name.c_str()).findGroup("pose"));
		vector<double> mask = group_to_vector(main.findGroup(mask_name.c_str()).findGroup("mask"));

		constraint_weight.first = new StartPoseConstraint(start_pos, mask);
		constraint_weight.second = weight;
	} else if (type == "collision") {
		double weight = find_and_check<double>(bottle, "weight");
		
		constraint_weight.first = new CollisionConstraint();
		constraint_weight.second = weight;
	} else if (type == "position") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		string marker = find_and_check<string>(bottle, "marker");

		constraint_weight.first = new PositionConstraint(marker, pos);
		constraint_weight.second = weight;
	} else if (type == "mindistance") {
		double weight = find_and_check<double>(bottle, "weight");
		double min_dist = find_and_check<double>(bottle, "mindist");
		string marker1 = find_and_check<string>(bottle, "marker1");
		string marker2 = find_and_check<string>(bottle, "marker2");

		constraint_weight.first = new MinDistanceConstraint(marker1, marker2, min_dist);
		constraint_weight.second = weight;
	} else if (type == "averageposition") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		string marker1 = find_and_check<string>(bottle, "marker1");
		string marker2 = find_and_check<string>(bottle, "marker2");

		constraint_weight.first = new AveragePositionConstraint(marker1, marker2, pos);
		constraint_weight.second = weight;
	} else if (type == "plane") {
		double weight = find_and_check<double>(bottle, "weight");
		string marker = find_and_check<string>(bottle, "marker");
		double val = find_and_check<double>(bottle, "pos");
		int axis = find_and_check<int>(bottle, "axis");

		constraint_weight.first = new PlaneConstraint(marker, axis, val);
		constraint_weight.second = weight;
	} else if (type == "orientation") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> orientation = group_to_vector(bottle.findGroup("orientation"));
		string marker = find_and_check<string>(bottle, "marker");
		int axis = find_and_check<int>(bottle, "axis");

		constraint_weight.first = new OrientationConstraint(marker, axis, orientation);
		constraint_weight.second = weight;
	} else if (type == "pointing") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		string marker = find_and_check<string>(bottle, "marker");
		double distance = find_and_check<double>(bottle, "distance");
		int axis = find_and_check<int>(bottle, "axis");
		int dir = find_and_check<int>(bottle, "dir");

		constraint_weight.first = new PointingConstraint(marker, pos, distance, axis, dir);
		constraint_weight.second = weight;
	} else if (type == "pointingmarker") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		string marker = find_and_check<string>(bottle, "marker");
		string point_marker = find_and_check<string>(bottle, "pointmarker");

		double distance = find_and_check<double>(bottle, "distance");
		int axis = find_and_check<int>(bottle, "axis");
		int dir = find_and_check<int>(bottle, "dir");

		constraint_weight.first = new PointingMarkerConstraint(marker, point_marker, distance, axis, dir);
		constraint_weight.second = weight;
	} else if (type == "opposite") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));
		vector<double> mask = group_to_vector(bottle.findGroup("mask"));

		string marker1 = find_and_check<string>(bottle, "marker1");
		string marker2 = find_and_check<string>(bottle, "marker2");

		constraint_weight.first = new OppositeConstraint(marker1, marker2, pos, mask);
		constraint_weight.second = weight;
	} else if (type == "grasp") {
		double weight = find_and_check<double>(bottle, "weight");
		vector<double> pos = group_to_vector(bottle.findGroup("pos"));

		string marker1 = find_and_check<string>(bottle, "marker1");
		string marker2 = find_and_check<string>(bottle, "marker2");

		int index1 = find_and_check<int>(bottle, "index1");
		int index2 = find_and_check<int>(bottle, "index2");

		double distance = find_and_check<double>(bottle, "distance");

		constraint_weight.first = new GraspConstraint(marker1, marker2, index1, index2, distance, pos);
		constraint_weight.second = weight;
	} else
		throw StringException(type + "no such type of constraint ");
	
	constraint_weight.first->toString(cout);
	cout << "weight: " << constraint_weight.second << endl << endl;
	return constraint_weight;
}


FixPoseConstraint::FixPoseConstraint(vector<double> pose) : 
Constraint("FixPoseConstraint"), 
	d_pose(pose), 
	d_mask(d_pose.size(), 1.0) 
{}

FixPoseConstraint::FixPoseConstraint(vector<double> pose, vector<double> mask) : 
Constraint("FixPoseConstraint"), 
	d_pose(pose), 
	d_mask(mask) 
{}

void FixPoseConstraint::adjust_pose_hook(vector<double>& pose) {
	// overwrite values in POSE with values in fix_pose where fix_pose_mask = 1
	for (size_t j(0); j < pose.size(); j++) {
		if (d_mask[j] >= 1) {
			pose[j] = d_pose[j];
		}
	}
}

void FixPoseConstraint::post_hook(Simulator &simulator) {
	d_pose = simulator.real_to_normal_motors(d_pose);
}

ostream& FixPoseConstraint::toString(ostream& o) {
	o << "type: [fixpose]" << endl;
	sprintvector(o, "pose", d_pose, d_mask);
	return o;
}



LimitConstraint::LimitConstraint(vector<double> min, vector<double> max) : 
Constraint("LimitConstraint"), 
	d_min(min), 
	d_max(max),
	d_mask(d_min.size(), 1.0) 
{ }

LimitConstraint::LimitConstraint(vector<double> min, vector<double> max, vector<double> mask) : 
Constraint("LimitConstraint"), 
	d_min(min), 
	d_max(max),
	d_mask(mask) 
{}

void LimitConstraint::adjust_pose_hook(vector<double>& pose) {
	// set values in POSE between min and max mask == 1
	for (size_t j(0); j < pose.size(); j++) {
		if (d_mask[j] >= 1) {
			if (pose[j] < d_min[j]) { pose[j] = d_min[j]; }
			if (pose[j] > d_max[j]) { pose[j] = d_max[j]; }
		}
	}
}

void LimitConstraint::post_hook(Simulator &simulator) {
	d_min = simulator.real_to_normal_motors(d_min);
	d_max = simulator.real_to_normal_motors(d_max);
}

ostream& LimitConstraint::toString(ostream& o) {
	o << "type: [limit]" << endl;
	sprintvector(o, "maximum", d_min, d_mask);
	sprintvector(o, "minimum", d_max, d_mask);
	return o;
}


HomePoseConstraint::HomePoseConstraint(vector<double> pose) : 
Constraint("HomePoseConstraint"), 
	d_pose(pose), 
	d_mask(pose.size(), 1.0)
{}

HomePoseConstraint::HomePoseConstraint(vector<double> pose, vector<double> mask) : 
Constraint("HomePoseConstraint"), 
	d_pose(pose), 
	d_mask(mask) 
{}

double HomePoseConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return pos_error(d_pose, motor_values, d_mask) / d_pose.size();
}

void HomePoseConstraint::post_hook(Simulator &simulator) {
	d_pose = simulator.real_to_normal_motors(d_pose);
}

ostream& HomePoseConstraint::toString(ostream& o) {
	o << "type: [homepose]" << endl;
	sprintvector(o, "pose", d_pose, d_mask);
	sprintvector(o, "mask", d_mask);
	return o;
}	


StartPoseConstraint::StartPoseConstraint(vector<double> pose) : 
Constraint("StartPoseConstraint"), 
	d_pose(pose), 
	d_mask(d_pose.size(), 1.0) 
{ }

StartPoseConstraint::StartPoseConstraint(vector<double> pose, vector<double> mask) : 
Constraint("StartPoseConstraint"), 
	d_pose(pose), 
	d_mask(mask) 
{}

double StartPoseConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return pos_error(d_pose, motor_values, d_mask) / d_pose.size();
}

void StartPoseConstraint::post_hook(Simulator &simulator) {
	d_pose = simulator.real_to_normal_motors(d_pose);
}

void StartPoseConstraint::start_pose_hook(const vector<double>& pose) {
	d_pose = pose;
}

ostream& StartPoseConstraint::toString(ostream& o) {
	o << "type: [startpose]" << endl;
	sprintvector(o, "pose", d_pose, d_mask);
	sprintvector(o, "mask", d_mask);
	return o;
}	


CollisionConstraint::CollisionConstraint() : Constraint("CollisionConstraint") {}

double CollisionConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return collisions;
}

ostream& CollisionConstraint::toString(ostream& o) {
	o << "type: [collision]" << endl;
	return o;
}	



MinDistanceConstraint::MinDistanceConstraint(string marker1_name, string marker2_name, double min_distance) : 
Constraint("MinDistanceConstraint"), 
	d_marker1(marker1_name), 
	d_marker2(marker2_name), 
	d_min_distance(min_distance) 
{}

double MinDistanceConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> pos1 = observation.markerPosition(QString(d_marker1.c_str()));
	vector<double> pos2 = observation.markerPosition(QString(d_marker2.c_str()));
	double distance = pos_error(pos1, pos2);
	double evl = 0.0;

	if (distance < d_min_distance)
		evl = d_min_distance - distance;

	/*if (d_marker2 == "left_hip") {
		cout << d_marker1 << " " << pos1[0] << " " << pos1[1] << " " << pos1[2] << " dist: " << distance << " eval: " << evl << endl;
	}*/
	return evl;
}

ostream& MinDistanceConstraint::toString(ostream& o) {
	o << "type: [mindistance]" << endl;
    o << "distance (" << d_marker1 << ", " << d_marker2 <<  ") > " << d_min_distance << endl;
	return o;
}	


PositionConstraint::PositionConstraint(string marker, vector<double> goal) : 
Constraint("PositionConstraint"), 
	d_marker(marker), 
	d_goal(goal) 
{}

double PositionConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> position = observation.markerPosition(QString(d_marker.c_str()));
	return pos_error(position, d_goal);
}

ostream& PositionConstraint::toString(ostream& o) {
	o << "type: [position]" << endl;
	o << "marker: d_marker = ";
	sprintvector(o, "marker", d_goal);
	return o;
}	


AveragePositionConstraint::AveragePositionConstraint(string marker1, string marker2, vector<double> goal) : 
Constraint("AveragePositionConstraint"),
	d_marker1(marker1), 
	d_marker2(marker2), 
	d_goal(goal) 
{}

double AveragePositionConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> position1 = observation.markerPosition(QString(d_marker1.c_str()));
	vector<double> position2 = observation.markerPosition(QString(d_marker2.c_str()));
	for (size_t i(0); i < position1.size(); ++i)
		position1[i] = (position1[i] + position2[i]) / 2.0;
	return pos_error(position1, d_goal);
}

ostream& AveragePositionConstraint::toString(ostream& o) {
	o << "type: [averageposition]" << endl;
	o << "mean (" << d_marker1 << ", " << d_marker2 << ") = ";
	for (size_t i(0); i < d_goal.size(); i++)
		o << d_goal[i] << " ";
	o << endl;
	return o;
}	



PlaneConstraint::PlaneConstraint(string marker, size_t axis, double value) :
Constraint("PlaneConstraint"),
	d_marker(marker),
	d_axis(axis),
	d_value(value)
{}

double PlaneConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> position = observation.markerPosition(QString(d_marker.c_str()));
	return fabs(position[d_axis] - d_value);
}

ostream& PlaneConstraint::toString(ostream& o) {
	o << "type: [plane]" << endl;
	o << d_marker << ", axis: " << d_axis << " = " << d_value << endl;
	return o;
}	



OrientationConstraint::OrientationConstraint(string marker, size_t axis, vector<double> goal_orientation) :
Constraint("OrientationConstraint"),
	d_marker(marker), 
	d_goal_orientation(9), 
	d_mask_orientation(9), 
	d_axis(axis) 
{
	assert(goal_orientation.size() == 3 && d_axis < 3);
	copy(goal_orientation.begin(), goal_orientation.end(), d_goal_orientation.begin() + d_axis * 3);
	fill(d_mask_orientation.begin() + d_axis * 3, d_mask_orientation.begin() + d_axis * 3 + 3, 1.0);
}

double OrientationConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return observation.orientationMeasure(QString(d_marker.c_str()), d_goal_orientation, d_mask_orientation);
}

ostream& OrientationConstraint::toString(ostream& o) {
	o << "type: [orientation]" << endl;
	o << d_marker << ", axis: " << d_axis << endl;
	sprintvector(o, "goal", d_goal_orientation);
	return o;
}	


PointingConstraint::PointingConstraint(string marker, vector<double> goal, double distance, size_t axis, size_t dir)	: 
Constraint("PointingConstraint"),
	d_marker(marker),
	d_goal(goal),
	d_distance(distance),
	d_axis(axis),
	d_dir(dir)
{}

double PointingConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> goal_orientation(9);
	vector<double> mask_orientation(9);

	vector<double> position_marker = observation.markerPosition(QString(d_marker.c_str()));
	vector<double> marker_diff(diff(d_goal, position_marker));
	double marker_distance = pos_error(d_goal, position_marker);
	double marker_distance_measure = fabs(marker_distance - d_distance);

	copy(marker_diff.begin(), marker_diff.end(), goal_orientation.begin() + 3 * d_axis);
	fill(mask_orientation.begin() + 3 * d_axis, mask_orientation.begin() + 3 * d_axis + 3, 1.);
	double marker_to_object = observation.orientationMeasure(QString(d_marker.c_str()), goal_orientation, mask_orientation);
	if (d_distance)
		return 10. * marker_distance_measure + marker_to_object;
	else
		return marker_to_object;
}

ostream& PointingConstraint::toString(ostream& o) {
	o << "type: [pointing]" << endl;
	o << d_marker << ", distance: " << d_distance << ", axis: " << d_axis << ", direction: " << d_dir << endl;
	sprintvector(o, "goal", d_goal);
	return o;
}	


PointingMarkerConstraint::PointingMarkerConstraint(string marker, string target, double distance, size_t axis, size_t dir) : 
Constraint("PointingMarkerConstraint"),
	d_marker(marker),
	d_target(target),
	d_distance(distance),
	d_axis(axis),
	d_dir(dir)
{}

double PointingMarkerConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> goal_orientation(9);
	vector<double> mask_orientation(9);

	vector<double> position_marker = observation.markerPosition(QString(d_marker.c_str()));
	vector<double> target_pos = observation.markerPosition(QString(d_target.c_str()));
	vector<double> marker_diff(diff(target_pos, position_marker));
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

ostream& PointingMarkerConstraint::toString(ostream& o) {
	o << "type: [pointingmarker]" << endl;
	o << "marker: " << d_marker << ", to: " << d_target << ", distance: " << d_distance;
	o << ", axis " << d_axis << ", direction: " << d_dir << endl;
	return o;
}	


OppositeConstraint::OppositeConstraint(string marker1, string marker2, vector<double> goal) :
Constraint("OppositeConstraint"),
	d_marker1(marker1),
	d_marker2(marker2),
	d_goal(goal),
	d_mask(goal.size(), 1.0)
{}

OppositeConstraint::OppositeConstraint(string marker1, string marker2, vector<double> goal, vector<double> mask) :
Constraint("OppositeConstraint"),
	d_marker1(marker1),
	d_marker2(marker2),
	d_goal(goal),
	d_mask(mask)
{}

double OppositeConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	vector<double> position_marker1 = observation.markerPosition(QString(d_marker1.c_str()));
	vector<double> position_marker2 = observation.markerPosition(QString(d_marker2.c_str()));

	vector<double> marker1_diff(diff(d_goal, position_marker1));
	vector<double> marker2_diff(diff(d_goal, position_marker2));

	// scale difference with mask vector
	for (size_t i(0); i<marker1_diff.size(); i++) {
		marker1_diff[i] = marker1_diff[i]*d_mask[i];
		marker2_diff[i] = marker2_diff[i]*d_mask[i];
	}

	return (norm_in_product(marker1_diff, marker2_diff) + 1.0) / 2.0;
}

ostream& OppositeConstraint::toString(ostream& o) {
	o << "type: [opposite]" << endl;
	o << d_marker1 << " opposite to: " << d_marker2 << " ";
	sprintvector(o, "at", d_goal, d_mask);
	o << endl;
	return o;
}	



GraspConstraint::GraspConstraint(string marker1, string marker2, size_t axis1, size_t axis2, double distance, vector<double> goal, double factor) :
Constraint("GraspConstraint"),
	d_marker1(marker1), d_marker2(marker2),
	d_point_constraint1(marker1, goal, distance, axis1),
	d_point_constraint2(marker2, goal, distance, axis2),
	d_opposite_constraint(marker1, marker2, goal),
	d_factor(factor)
{}

double GraspConstraint::evaluate(vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
	return d_point_constraint1.evaluate(motor_values, observation, collisions) +
		d_point_constraint2.evaluate(motor_values, observation, collisions) +
		d_factor * d_opposite_constraint.evaluate(motor_values, observation, collisions);
}

ostream& GraspConstraint::toString(ostream& o) {
	o << "type: [grasp]" << endl;
	o << d_marker1 << " " << d_marker2 << endl;
	d_point_constraint1.toString(o);
	d_point_constraint2.toString(o);
	d_opposite_constraint.toString(o);
	o << endl;
	return o;
}	


MapBuildConstraint::MapBuildConstraint(string marker, size_t nNN, double alpha, double config_measure, double exploration_force) :
Constraint("MapBuildConstraint"),
	d_marker(marker),
	d_nNN(nNN),
	d_alpha(alpha),
	d_config_measure(config_measure),
	d_exploration_force(exploration_force)
{}

void MapBuildConstraint::add_point(vector<double> &x, vector<double> &q) {
	d_points.push_back(x);
	d_configurations.push_back(q);
}

void MapBuildConstraint::add_points(vector<vector<double> > xs, vector<vector<double> > qs) {
	cout << "=> " << xs.size();
	cout << " " <<
		qs.size() << " ";
	cout << d_points.size() << " ";

	cout << d_configurations.size() << endl;
	copy(xs.begin(), xs.end(), back_inserter(d_points));
	copy(qs.begin(), qs.end(), back_inserter(d_configurations));
}

double MapBuildConstraint::close_measure(vector<double> &values, double alpha, vector<size_t> &indexes) const {
	double total_dist(0);
	vector<double> distances(d_points.size());
	for (size_t i(0); i < d_points.size(); ++i) {
		total_dist += distances[i] = pos_error(values, d_points[i]);
	}

	vector<double*> distances_ptrs(distances.size());
	vector<double>::iterator it(distances.begin()), it_end(distances.end());
	vector<double*>::iterator it2(distances_ptrs.begin());
	for (; it != it_end; ++it, ++it2)
		*it2 = &(*it);

	sort(distances_ptrs.begin(), distances_ptrs.end(), LessDereference());
	int n_distances = distances.size() >= d_nNN ? d_nNN : distances.size();

	//double total_dist = accumulate(distances.begin(), distances.end(), 0.0);
	indexes.resize(n_distances);
	double measure(0.0);
	for (size_t i(0); i < n_distances; ++i)  {
		measure += fabs(*distances_ptrs[i] - alpha);// * (distances[i] - alpha); //or use abs?
		indexes[i] = distances_ptrs[i] - &distances[0];
	}

	//cout << measure << " " << exp(-total_dist) << " " << total_dist << endl;
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

double MapBuildConstraint::config_measure(vector<double> &values, vector<size_t> &indexes) const {
	double measure(0.0);

	for (size_t i(0); i < indexes.size(); ++i) 
		measure += pos_error(values, d_configurations[indexes[i]]);	  
	return measure;
}

ostream& MapBuildConstraint::toString(ostream& o) {
	o << "type: [mapbuild]" << endl;
	return o;
}	

