#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <vector>
#include <iostream>
#include "model.h"

class Simulator {
	KinematicModel::Model &d_model;
	KinematicModel::Robot &d_robot;

public:
	int d_total_motors;
	std::vector<double> d_home_pos, d_last_position;

public:
	Simulator(KinematicModel::Model& model,  KinematicModel::Robot& robot) : d_model(model), d_robot(robot) {
		int total_motors(0);
		int n_parts = d_robot.numBodyParts();
		std::vector<double> home_pos;
		for (size_t i(0); i < n_parts; ++i) {
			int n_motors = d_robot.getNumMotors(i);
			total_motors += n_motors;
			for (size_t m(0); m < n_motors; ++m) {
				home_pos.push_back(d_robot.getPart(i)->at(m)->normHomePos());
			}
		}
		d_total_motors = total_motors;
		d_home_pos = home_pos;
		d_last_position = d_home_pos;
	}

	void add_ball(float x, float y, float z) const {
		  KinematicModel::CompositeObject *composite = new KinematicModel::CompositeObject( d_model.OBSTACLE() );
		  KinematicModel::Sphere *sphere = new KinematicModel::Sphere( .02 );
		  QColor color( 0, 0, 255, 255 );
		  QVector3D pos(x, y, z);
		  sphere->translate(pos);
		  sphere->setFreeColor(color);
		  sphere->setCollidingColor(color);
		  composite->append(sphere);
		  d_model.appendObject(composite);
	}

	void add_point(float x, float y, float z) const {
		  KinematicModel::CompositeObject *composite = new KinematicModel::CompositeObject( d_model.TARGET() );
		  KinematicModel::Sphere *sphere = new KinematicModel::Sphere( .004 );
		  QColor color( 0, 0, 255, 255 );
		  QVector3D pos(x, y, z);
		  sphere->translate(pos);
		  sphere->setFreeColor(color);
		  sphere->setCollidingColor(color);
		  composite->append(sphere);
		  d_model.appendObject(composite);
	}

	int computePose() {
		return d_model.computePose();
	}

	void set_motors(std::vector<double> &motor_values) {
		assert(motor_values.size() == d_total_motors);
		size_t i(0);
		int n_parts = d_robot.numBodyParts();
		for (size_t p(0); p < n_parts; ++p) {
			int n_motors = d_robot.getNumMotors(p);
			for (size_t m(0); m < n_motors; ++m, ++i)
				d_robot.setNormalPosition(p, m, motor_values[i]);
		}
	}

	std::map<std::string, std::vector<double> > to_motor_named_map(std::vector<double> &vec) {
		std::map<std::string, std::vector<double> > motor_map;

		int n_parts = d_robot.numBodyParts();
		std::vector<double>::iterator it(vec.begin());

		for (size_t p(0); p < n_parts; ++p) {
			int n_motors = d_robot.getNumMotors(p);
			motor_map[d_robot.getPart(p)->name().toStdString()] = std::vector<double>(it, it + n_motors);
			it += n_motors;
		}

		return motor_map;
	}

	std::vector<double> normal_to_real_motors(std::vector<double> normal_values) {
		std::vector<double> real_values(normal_values.size());
		size_t i(0);
		int n_parts = d_robot.numBodyParts();
		for (size_t p(0); p < n_parts; ++p) {
			int n_motors = d_robot.getNumMotors(p);
			for (size_t m(0); m < n_motors; ++m, ++i)
				real_values[i] = d_robot.NormalToEncoderPosition(p, m, normal_values[i]);
		}
		return real_values;
	}

	std::vector<double> real_to_normal_motors(std::vector<double> real_values) {
		std::vector<double> normal_values(real_values.size());
		size_t i(0);
		int n_parts = d_robot.numBodyParts();
		for (size_t p(0); p < n_parts; ++p) {
			int n_motors = d_robot.getNumMotors(p);
			for (size_t m(0); m < n_motors; ++m, ++i)
				normal_values[i] = d_robot.EncoderToNormalPosition(p, m, real_values[i]);
		}
		return normal_values;
	}

	KinematicModel::Model &model() {
		return d_model;
	}

	KinematicModel::Robot &robot() {
		return d_robot;
	}

	std::vector<double> home_pos() {
		return d_home_pos;
	}

	int total_motors() {
		return d_total_motors;
	}

	int total_parts() {
		return d_robot.numBodyParts();
	}


	std::string part_name(size_t i) {
		return d_robot.getPart(i)->name().toStdString();
	}
};

#endif
