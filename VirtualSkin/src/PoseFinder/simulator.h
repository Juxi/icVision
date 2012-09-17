#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <vector>
#include <map>
#include <string>
#include "model.h"
#include "robot.h"
#include "exception.h"
#include <QColor>

class Simulator {
private:
	KinematicModel::Model &d_model;
	KinematicModel::Robot &d_robot;

	int d_total_motors;
	QStringList d_color_list;
	std::vector<double> d_home_pos, d_last_position;

public:
	Simulator(KinematicModel::Model& model,  KinematicModel::Robot& robot);

	void add_ball(float x, float y, float z) const;
	void add_point(float x, float y, float z, size_t mapid) const;

	void set_motors(const std::vector<double> &motor_values);
	std::map<std::string, std::vector<double> > to_motor_named_map(std::vector<double> &vec);
	std::vector<double> normal_to_real_motors(std::vector<double> normal_values);
	std::vector<double> real_to_normal_motors(std::vector<double> real_values);

	KinematicModel::Model &model() { return d_model; }
	KinematicModel::Robot &robot() { return d_robot; }
	std::vector<double> home_pos() { return d_home_pos; }

	int computePose() { return d_model.computePose(); }
	int total_motors() { return d_total_motors; }
	int total_parts() { return d_robot.numBodyParts(); }
	std::string part_name(size_t i) { return d_robot.getPart(i)->name().toStdString(); }
};

#endif