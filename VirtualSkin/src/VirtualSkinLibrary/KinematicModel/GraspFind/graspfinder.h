#ifndef GRASPFINDER_H_
#define GRASPFINDER_H_

#include <cmath>
#include <iostream>
#include <QVector>
#include <cassert>

#include "model.h"
#include "sphere.h"

//class KinematicModel::Model;
#include "xnes.h"

class GraspFinder
{
	
public:
	GraspFinder( KinematicModel::Model& m,  KinematicModel::Robot& robot) : theModel(m), theRobot(robot), d_total_motors(0){
		int total_motors(0);
		int n_parts = theRobot.numBodyParts();
		std::vector<double> home_pos;
		for (size_t i(0); i < n_parts; ++i) {
			std::cout << theRobot.getPart(i)->name().toStdString() << " " << theRobot.getNumMotors(i) << std::endl;
			int n_motors = theRobot.getNumMotors(i);
			total_motors += n_motors;
			for (size_t m(0); m < n_motors; ++m) {
				home_pos.push_back(theRobot.getPart(i)->at(m)->normHomePos());
			}
		}
		d_total_motors = total_motors;
		d_home_pos = home_pos;
		d_last_position = d_home_pos;
	}

	~GraspFinder() {}
	
	
	void set_motors(std::vector<double> &motor_values) {
		assert(motor_values.size() == d_total_motors);
		size_t i(0);
		int n_parts = theRobot.numBodyParts();
		for (size_t p(0); p < n_parts; ++p) {
			int n_motors = theRobot.getNumMotors(p);
			for (size_t m(0); m < n_motors; ++m, ++i)
				theRobot.setNormalPosition(p, m, motor_values[i]);
		}
	}

	void find_pose(unsigned int maxevals = 100000, double fitness_threshold = 0., double std = .4, int population_size = 150);

	void add_ball(float x, float y, float z) const {
		  KinematicModel::CompositeObject *composite = new KinematicModel::CompositeObject( theModel.OBSTACLE() );
		  KinematicModel::Sphere *sphere = new KinematicModel::Sphere( .02 );
		  QColor color( 0, 0, 255, 255 );
		  QVector3D pos(x, y, z);
		  sphere->translate(pos);
		  sphere->setFreeColor(color);
		  sphere->setCollidingColor(color);
		  composite->append(sphere);
		  theModel.appendObject(composite);
	}

	void add_point(float x, float y, float z) const {
		  KinematicModel::CompositeObject *composite = new KinematicModel::CompositeObject( theModel.TARGET() );
		  KinematicModel::Sphere *sphere = new KinematicModel::Sphere( .01 );
		  QColor color( 0, 0, 0, 255 );
		  QVector3D pos(x, y, z);
		  sphere->translate(pos);
		  sphere->setFreeColor(color);
		  sphere->setCollidingColor(color);
		  composite->append(sphere);
		  theModel.appendObject(composite);
	}

	std::vector<double> get_best_point();

public:
	KinematicModel::Model& theModel;	// the world model	
	KinematicModel::Robot& theRobot;
	std::vector<double> d_home_pos, d_last_position;
	std::vector<std::vector<double> > d_points;
	std::vector<std::vector<double> > d_configuration_points;
	std::vector<double> d_best_point;

	int d_total_motors;
	
};

#endif
/** @} */
