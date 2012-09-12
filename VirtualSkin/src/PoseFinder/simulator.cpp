#include "simulator.h"
#include <iostream>

Simulator::Simulator(KinematicModel::Model& model,  KinematicModel::Robot& robot) : d_model(model), d_robot(robot) {
	std::cout << "Initializing Simulator" << std::endl;
	int total_motors(0);
	int n_parts = d_robot.numBodyParts();
	std::vector<double> home_pos;
	std::cout << "N Parts: " << n_parts << std::endl;

	for (size_t i(0); i < n_parts; ++i) {
		std::cout << "part [" << i << "] = " << d_robot.getPart(i)->name().toStdString() << std::endl;
		int n_motors = d_robot.getNumMotors(i);
		total_motors += n_motors;
		for (size_t m(0); m < n_motors; ++m) {
			home_pos.push_back(d_robot.getPart(i)->at(m)->normHomePos());
		}
	}
	std::cout << "Total Motors: " << total_motors << std::endl;
	d_total_motors = total_motors;
	d_home_pos = home_pos;
	d_last_position = d_home_pos;
}

void Simulator::add_ball(float x, float y, float z) const {
	KinematicModel::CompositeObject *composite = new KinematicModel::CompositeObject( d_model.OBSTACLE(), d_model.FIELD() );
	KinematicModel::Sphere *sphere = new KinematicModel::Sphere( .02 );
	QColor color( 0, 0, 255, 255 );
	QColor red_color( 255, 0, 0, 255 );
	QVector3D pos(x, y, z);
	sphere->translate(pos);
	sphere->setFreeColor(color);
	sphere->setCollidingColor(red_color);
	composite->appendPrimitive(sphere);
	d_model.appendObject(composite);
}

void Simulator::add_point(float x, float y, float z) const {
	KinematicModel::CompositeObject *composite = new KinematicModel::CompositeObject( d_model.TARGET(), d_model.GHOST() );
	KinematicModel::Sphere *sphere = new KinematicModel::Sphere( .004 );
	QColor color( 0, 0, 255, 255 );
	QVector3D pos(x, y, z);
	sphere->translate(pos);
	sphere->setFreeColor(color);
	sphere->setCollidingColor(color);
	composite->appendPrimitive(sphere);
	d_model.appendObject(composite);
}

void Simulator::set_motors(const std::vector<double> &motor_values) {
	assert(motor_values.size() == d_total_motors);
	size_t i(0);
	int n_parts = d_robot.numBodyParts();
	for (size_t p(0); p < n_parts; ++p) {
		int n_motors = d_robot.getNumMotors(p);
		for (size_t m(0); m < n_motors; ++m, ++i)
			d_robot.setNormalPosition(p, m, motor_values[i]);
	}
}

std::map<std::string, std::vector<double> > Simulator::to_motor_named_map(std::vector<double> &vec) {
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


std::vector<double> Simulator::normal_to_real_motors(std::vector<double> normal_values) {
	std::vector<double> real_values(normal_values.size());
	size_t i(0);
	int n_parts = d_robot.numBodyParts();
	for (size_t p(0); p < n_parts; ++p) {
		int n_motors = d_robot.getNumMotors(p);
		for (size_t m(0); m < n_motors; ++m, ++i) {
			if (i >= normal_values.size()) {
				throw StringException("normal to real conversion exceeds size of vector");
			}
			real_values[i] = d_robot.NormalToEncoderPosition(p, m, normal_values[i]);
		}
	}
	return real_values;
}

std::vector<double> Simulator::real_to_normal_motors(std::vector<double> real_values) {
	std::vector<double> normal_values(real_values.size());
	size_t i(0);
	int n_parts = d_robot.numBodyParts();
	for (size_t p(0); p < n_parts; ++p) {
		int n_motors = d_robot.getNumMotors(p);
		for (size_t m(0); m < n_motors; ++m, ++i) {
			if (i >= real_values.size()) {
				throw StringException("real to normal conversion exceeds size of vector");
			}
			normal_values[i] = d_robot.EncoderToNormalPosition(p, m, real_values[i]);
		}
	}
	return normal_values;
}
