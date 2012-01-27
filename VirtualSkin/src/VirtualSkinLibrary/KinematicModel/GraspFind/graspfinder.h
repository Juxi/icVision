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

//class KinematicModel::Model;

class GraspFinder : public QThread
{
	
public:
	GraspFinder( KinematicModel::Model& m,  KinematicModel::Robot& robot, bool v = false ) : theModel(m), theRobot(robot), d_total_motors(0), verbose(v), keepRunning(true) {
		int total_motors(0);
		int n_parts = theRobot.numBodyParts();
		for (size_t i(0); i < n_parts; ++i) {
			int n_motors = theRobot.getNumMotors(i);
			total_motors += n_motors;
		}
		d_total_motors = total_motors;
	}
	~GraspFinder() {}
	
	void stop();
	
private:
	KinematicModel::Model& theModel;	// the world model	
	KinematicModel::Robot& theRobot;
	int d_total_motors;
	bool verbose;
	bool keepRunning;
	
	void run();
};

#endif
/** @} */
