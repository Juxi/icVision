/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup WorldModelDemo
 *	@{
 */

#ifndef GRASPFINDER_H_
#define GRASPFINDER_H_

#include <QThread>
#include "model.h"
#include "assert.h"
#include <cmath>
#include <iostream>

//class KinematicModel::Model;
#include "xnes.h"

class GraspFinder : public QThread
{
	
public:
	GraspFinder( KinematicModel::Model& m,  KinematicModel::Robot& robot, bool v = false ) : theModel(m), theRobot(robot), d_total_motors(0), verbose(v), keepRunning(true) {
		int total_motors(0);
		int n_parts = theRobot.numBodyParts();
		std::vector<double> home_pos;
		for (size_t i(0); i < n_parts; ++i) {
			int n_motors = theRobot.getNumMotors(i);
			total_motors += n_motors;
			for (size_t m(0); m < n_motors; ++m) {
				home_pos.push_back(theRobot.getPart(i)->at(m)->normHomePos());
				std::cout << theRobot.getPart(i)->at(m)->normHomePos() << " ";
			}
		}
		d_total_motors = total_motors;
		d_home_pos = home_pos;

	}
	~GraspFinder() {}
	
	void stop();
	
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

	double distance_to_home(std::vector<double> &motor_values) {
		double dist = 0.0;
		for (size_t i(0); i < motor_values.size(); ++i)
			dist += pow(d_home_pos[i] - motor_values[i], 2.0);
		dist = sqrt(dist) / motor_values.size();
		return dist;
	}

	void do_test();

public:
	KinematicModel::Model& theModel;	// the world model	
	KinematicModel::Robot& theRobot;
	std::vector<double> d_home_pos;

	int d_total_motors;
	bool verbose;
	bool keepRunning;
	
	void run();
};

#endif
/** @} */
