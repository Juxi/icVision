#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <vector>
#include <string>
#include <QString>
#include <algorithm>
#include <numeric>

class Constraint {

public:
	Constraint()
	 {}

	virtual double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) = 0;

	static double pos_error(std::vector<double> const &values, std::vector<double> const &goal_pos) {
		double distance(0.0);
		for (size_t i(0); i < values.size(); ++i)
			distance += pow(values[i] - goal_pos[i], 2.0);
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

	static std::vector<double> vector3(double a1, double a2, double a3) {
		std::vector<double> a_vector(3);
		a_vector[0] = a1;
		a_vector[1] = a2;
		a_vector[2] = a3;
		return a_vector;
	}
};

class HomePoseConstraint : public Constraint {
	std::vector<double> d_home_pose;
public:
	HomePoseConstraint(std::vector<double> home_pose) : d_home_pose(home_pose){}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		return Constraint::pos_error(d_home_pose, motor_values) / d_home_pose.size();
	}

};

class CollisionConstraint : public Constraint {
public:
	CollisionConstraint(){}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		return collisions;
	}
};

class PositionConstraint : public Constraint {
public:
	std::string d_marker_name;
	std::vector<double> d_goal_position;

public:
	PositionConstraint(std::string marker_name, std::vector<double> goal_position) : d_marker_name(marker_name), d_goal_position(goal_position) {
	}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		std::vector<double> position = observation.markerPosition(QString(d_marker_name.c_str()));
		return Constraint::pos_error(position, d_goal_position);
	}
};

class PlaneConstraint : public Constraint {
	std::string d_marker_name;
	size_t d_axis;
	double d_value;

public:
	PlaneConstraint(std::string marker_name, size_t axis, double value) :
		d_marker_name(marker_name),
		d_axis(axis),
		d_value(value)
	{}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		std::vector<double> position = observation.markerPosition(QString(d_marker_name.c_str()));
		return fabs(position[d_axis] - d_value);
	}
};

class OrientationConstraint : public Constraint {
public:
	std::string d_marker_name;
	std::vector<double> d_goal_orientation;
	std::vector<double> d_mask_orientation;
	int d_axis;
public:
	OrientationConstraint(std::string marker_name, size_t axis, std::vector<double> goal_orientation): d_marker_name(marker_name), d_goal_orientation(9), d_mask_orientation(9), d_axis(axis) {
		assert(goal_orientation.size() == 3 && d_axis < 3);
		copy(goal_orientation.begin(), goal_orientation.end(), d_goal_orientation.begin() + d_axis * 3);
		fill(d_mask_orientation.begin() + axis * 3, d_mask_orientation.begin() + d_axis * 3 + 3, 1.0);
	}

	double &element(int index) {
		return d_goal_orientation[d_axis * 3 + index];
	}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		return observation.orientationMeasure(QString(d_marker_name.c_str()), d_goal_orientation, d_mask_orientation);
	}
};

class PointingConstraint : public Constraint {
	std::string d_marker;
	std::vector<double> d_goal_position;
	double d_distance;
	size_t d_axis;

public:
	PointingConstraint(std::string marker, std::vector<double> goal_position, double distance, size_t axis)
	: d_marker(marker),
	  d_goal_position(goal_position),
	  d_distance(distance),
	  d_axis(axis)
	{

	}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		std::vector<double> goal_orientation(9);
		std::vector<double> mask_orientation(9);

		std::vector<double> position_marker = observation.markerPosition(QString(d_marker.c_str()));
		std::vector<double> marker_diff(diff(d_goal_position, position_marker));
		double marker_distance = pos_error(d_goal_position, position_marker);
		double marker_distance_measure = fabs(marker_distance - d_distance);

		copy(marker_diff.begin(), marker_diff.end(), goal_orientation.begin() + 3 * d_axis);
		fill(mask_orientation.begin() + 3 * d_axis, mask_orientation.begin() + 3 * d_axis + 3, 1.);
		double marker_to_object = observation.orientationMeasure(QString(d_marker.c_str()), goal_orientation, mask_orientation);
		return 10. * marker_distance_measure + marker_to_object;
	}
};

class OppositeConstraint : public Constraint {
	std::string d_marker1, d_marker2;
	std::vector<double> d_goal_position;
public:
	OppositeConstraint(std::string marker1, std::string marker2, std::vector<double> goal_position) :
	d_marker1(marker1),
	d_marker2(marker2),
	d_goal_position(goal_position)
	{}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		std::vector<double> position_marker1 = observation.markerPosition(QString(d_marker1.c_str()));
		std::vector<double> position_marker2 = observation.markerPosition(QString(d_marker2.c_str()));

		std::vector<double> marker1_diff(diff(d_goal_position, position_marker1));
		std::vector<double> marker2_diff(diff(d_goal_position, position_marker2));

		return (norm_in_product(marker1_diff, marker2_diff) + 1.0) / 2.0;
	}
};

class GraspConstraint : public Constraint {
	std::string d_marker_1, d_marker_2;

	PointingConstraint d_point_constraint1, d_point_constraint2;
	OppositeConstraint d_opposite_constraint;

public:
	GraspConstraint(std::string marker_1, std::string marker_2, size_t axis1, size_t axis2, double distance, std::vector<double> goal_position) :
	d_marker_1(marker_1), d_marker_2(marker_2),
	d_point_constraint1(marker_1, goal_position, distance, axis1),
	d_point_constraint2(marker_2, goal_position, distance, axis2),
	d_opposite_constraint(marker_1, marker_2, goal_position)
	{}


	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		return d_point_constraint1.evaluate(motor_values, observation, collisions) +
			d_point_constraint2.evaluate(motor_values, observation, collisions) +
			d_opposite_constraint.evaluate(motor_values, observation, collisions);
	}

};

class MapBuildConstraint : public Constraint {
	std::string d_marker;
	std::vector<std::vector<double> > d_points;
	size_t d_nNN;
	double d_alpha;

public:
	MapBuildConstraint(std::string marker, size_t nNN, double alpha) :
		d_marker(marker),
		d_nNN(nNN),
		d_alpha(alpha)
	{}

	std::vector<std::vector<double> > &points() {
		return d_points;
	}

	double close_measure(std::vector<double> &values, double alpha) const {
		std::vector<double> distances;
		for (size_t i(0); i < d_points.size(); ++i) {
			distances.push_back(pos_error(values, d_points[i]));
		}

		sort(distances.begin(), distances.end());
		int n_distances = distances.size() >= d_nNN ? d_nNN : distances.size();

		double total_dist = std::accumulate(distances.begin(), distances.end(), 0.0);
		double measure(0.0);
		for (size_t i(0); i < n_distances; ++i)  {
			measure += fabs(distances[i] - alpha);// * (distances[i] - alpha); //or use abs?
		}

		return measure;// + exp(-total_dist);
	}

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {
		std::vector<double> position = observation.markerPosition(QString(d_marker.c_str()));

		return close_measure(position, d_alpha);
	}

};


#endif
