#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <vector>
#include <string>
#include <ostream>
#include "model.h"
#include "simulator.h"
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

class Constraint {
private:
	std::string d_name;
public:
	Constraint(std::string name) : d_name(name)
	{}

	virtual double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) = 0;
	virtual void post_hook(Simulator &simulator) {}; // called after initializing the simulator
	virtual void start_pose_hook(const std::vector<double>& pose) {}; // called before starting search for next map pose
	virtual void adjust_pose_hook(std::vector<double>& pose) {}; // called before evaluation of each pose
	virtual std::string toString() { return ""; };

	std::string name() {return d_name;}

	static std::vector<double> vector3(double a1, double a2, double a3) {
		std::vector<double> a_vector(3);
		a_vector[0] = a1;
		a_vector[1] = a2;
		a_vector[2] = a3;
		return a_vector;
	}
};
//std::string operator<<(std::string o, Constraint& c) { return c.toString(o); }; // causes linking problems

class StartPoseConstraint : public Constraint {
private:
	std::vector<double> d_pose;
	std::vector<double> d_mask;
public:
	StartPoseConstraint(std::vector<double> d_start_pose);
	StartPoseConstraint(std::vector<double> d_start_pose, std::vector<double> d_start_pose_mask);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	
	virtual void start_pose_hook(const std::vector<double>& pose);
	virtual void post_hook(Simulator &simulator);
	virtual std::string toString();
};


class HomePoseConstraint : public Constraint {
private:
	std::vector<double> d_pose;
	std::vector<double> d_mask;
public:
	HomePoseConstraint(std::vector<double> home_pose);
	HomePoseConstraint(std::vector<double> home_pose, std::vector<double> home_pose_mask);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual void post_hook(Simulator &simulator);
	virtual std::string toString();
};


class FixPoseConstraint : public Constraint {
private:
	std::vector<double> d_pose;
	std::vector<double> d_mask;
public:
	FixPoseConstraint(std::vector<double> pose);
	FixPoseConstraint(std::vector<double> pose, std::vector<double> mask);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {return 0.0; };
	virtual void post_hook(Simulator &simulator);
	virtual void adjust_pose_hook(std::vector<double>& pose);
	virtual std::string toString();
};


class LimitConstraint : public Constraint {
private:
	std::vector<double> d_min;
	std::vector<double> d_max;
	std::vector<double> d_mask;
public:
	LimitConstraint(std::vector<double> min, std::vector<double> max);
	LimitConstraint(std::vector<double> min, std::vector<double> max, std::vector<double> mask);

	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions) {return 0.0; };
	virtual void post_hook(Simulator &simulator);
	virtual void adjust_pose_hook(std::vector<double>& pose);
	virtual std::string toString();
};



class CollisionConstraint : public Constraint {
public:
	CollisionConstraint();
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class MinDistanceConstraint : public Constraint {
private:
	std::string d_marker2;
	std::string d_marker1;
	double d_min_distance;
public:
	MinDistanceConstraint(std::string marker1_name, std::string marker2_name, double d_min_distance);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class PositionConstraint : public Constraint {
private:
	std::string d_marker;
	std::vector<double> d_goal;
public:
	PositionConstraint(std::string marker_name, std::vector<double> goal);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	std::vector<double> &goal() {return d_goal; };
	virtual std::string toString();
};


class AveragePositionConstraint : public Constraint {
private:
	std::string d_marker1, d_marker2;
	std::vector<double> d_goal;
public:
	AveragePositionConstraint(std::string marker1, std::string marker2, std::vector<double> goal);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class PlaneConstraint : public Constraint {
private:
	std::string d_marker;
	size_t d_axis;
	double d_value;
public:
	PlaneConstraint(std::string marker_name, size_t axis, double value);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class OrientationConstraint : public Constraint {
private:
	std::string d_marker;
	std::vector<double> d_goal_orientation;
	std::vector<double> d_mask_orientation;
	int d_axis;
public:
	OrientationConstraint(std::string marker_name, size_t axis, std::vector<double> goal_orientation);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	double &element(int index) { return d_goal_orientation[d_axis * 3 + index]; }
	virtual std::string toString();
};


class PointingConstraint : public Constraint {
private:
	std::string d_marker;
	std::vector<double> d_goal;
	double d_distance;
	size_t d_axis, d_dir;
public:
	PointingConstraint(std::string marker, std::vector<double> goal, double distance, size_t axis, size_t dir = 1);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class PointingMarkerConstraint : public Constraint {
private:
	std::string d_marker, d_target;
	std::vector<double> d_goal;
	double d_distance;
	size_t d_axis;
	int d_dir;
public:
	PointingMarkerConstraint(std::string marker, std::string target, double distance, size_t axis, size_t dir = 1);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class OppositeConstraint : public Constraint {
private:
	std::string d_marker1, d_marker2;
	std::vector<double> d_goal, d_mask;
public:
	OppositeConstraint(std::string marker1, std::string marker2, std::vector<double> goal, std::vector<double> mask);
	OppositeConstraint(std::string marker1, std::string marker2, std::vector<double> goal);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class GraspConstraint : public Constraint {
private:
	std::string d_marker1, d_marker2;
	PointingConstraint d_point_constraint1, d_point_constraint2;
	OppositeConstraint d_opposite_constraint;
	double d_factor;
public:
	GraspConstraint(std::string marker_1, std::string marker_2, size_t axis1, size_t axis2, double distance, std::vector<double> goal, double factor = 1.);
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class MapBuildConstraint : public Constraint {
private:
	std::string d_marker;
	std::vector<std::vector<double> > d_points;
	std::vector<std::vector<double> > d_configurations;
	size_t d_nNN;
	double d_alpha, d_config_measure;
	double d_exploration_force;
public:
	MapBuildConstraint(std::string marker, size_t nNN, double alpha, double config_measure = 0.0, double exploration_force = 0.0);

	std::string marker() { return d_marker; }
	double alpha() { return d_alpha; };
	std::vector<std::vector<double> > &points() { return d_points; }
	std::vector<std::vector<double> > &configurations() {  return d_configurations; }

	void add_point(std::vector<double> &x, std::vector<double> &q);
	void add_points(std::vector<std::vector<double> > xs, std::vector<std::vector<double> > qs);

	double close_measure(std::vector<double> &values, double alpha, std::vector<size_t> &indexes) const;
	double config_measure(std::vector<double> &values, std::vector<size_t> &indexes) const;
	double evaluate(std::vector<double> motor_values, KinematicModel::RobotObservation observation, int collisions);
	virtual std::string toString();
};


class ConstraintFactory {
public:
	ConstraintFactory(){}
	std::pair<Constraint*, double> constraint_from_bottle(yarp::os::Bottle &bottle, yarp::os::Property &main);
};


#endif
