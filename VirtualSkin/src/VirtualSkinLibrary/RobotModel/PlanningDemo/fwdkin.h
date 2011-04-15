/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup PlanningDemo
 *	@{
 */

#ifndef FwdKin_H
#define FwdKin_H


#include <model.h>

using namespace RobotModel;


//!
//! interface between planning code and robot model
//!
class FwdKin : public QObject
{
	Q_OBJECT

public:
	FwdKin(bool visualize = false);
	virtual ~FwdKin();

	Model model;
	Robot& robot;
	World& world;

	void pose(const double* jointAngle, bool& collision, double* handpos, bool wait = false);

public slots:
	void obtainObservation(RobotObservation obs);

protected:

	bool m_hasObs;
	RobotObservation m_obs;
};


#endif
/** @} */

