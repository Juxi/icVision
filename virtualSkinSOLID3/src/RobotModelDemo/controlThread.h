/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModelDemo
 *	@{
 */

#ifndef CONTROLTHREAD_H_
#define CONTROLTHREAD_H_

#include <QThread>
#include "model.h"


class ControlThread : public QThread
{
	Q_OBJECT

public:
	
	ControlThread();			//!< Connects the RobotModel::Model to the ControlThread, and initializes some members
	
	RobotModel::Model	model;	//!< The model that contains the robot
								/**< It is public so that the user has access to the information that was read out of the robot configuration file, 
									 such as the name of the robot and the number of controllable axes for example */
	
private:
	
	void run();					//!< Collision response is handled in a separate thread so as not to interrupt anything
	
};
#endif
/** @} */