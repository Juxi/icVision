/*******************************************************************
 ***              Copyright (C) 2010 Gregor Kaufmann             ***
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef STATEOBSERVER_H_
#define STATEOBSERVER_H_

#include <QObject>
#include <yarp/os/all.h>
#include "yarp/os/IObserver.h"
#include "circularBuffer.h"
#include "robot.h"

namespace MoBeE
{
	class RobotFilter;
	class StateObserver;
}

/** \brief An IObserver implemented to monitor streaming state information (motor encoder values)
 */
class MoBeE::StateObserver : public QObject, public yarp::os::IObserver
{
	Q_OBJECT
	
public:

	StateObserver(RobotFilter *f, const int b);		//!< Nothing special to do here
													/**< \param r The RobotFilter to which this Observer belongs
														 \param b The index of the RobotModel::BodyPart to which this Observer applies */
	virtual ~StateObserver();						//!< Nothing special to do here

	//void setFilterStatus( char c ) { filterStatus = c; }
	virtual void onDataObserved(yarp::os::Bottle &b);	//!< The handler function is called whenever state information passes through the filter
	
	CircularBuffer::Item earliestPose() { return poseBuffer.getOldest(); }	//!< Returns the oldest pose in the PoseBuffer (which can't be colliding)
	CircularBuffer::Item currentPose() { return poseBuffer.getCurrent(); }		//!< Returns the most recent pose written to the PoseBuffer
	QVector< CircularBuffer::Item > getHistory() { return poseBuffer.getHistory(); }		//!< Returns the most recent pose written to the PoseBuffer
	qreal getPeriod() { return poseBuffer.getPeriod(); }			//!< Returns the average period between arriving state bottles
	
	void initPoseBuffer( CircularBuffer::Item v ) { poseBuffer.init(v); }	//!< Fills every slot in the PoseBuffer with the vector passed in as a parameter
	
	enum PoseStatus { COLLIDING, SAFE, WAYPOINT };
	
	void setWaypoint()
	{
		//printf("Set Waypoint!!!\n");
		poseBuffer.init(WAYPOINT);
	}
	
signals:
	void setPosition( int i, const QVector<qreal>& v);	//!< Set the motor encoder positions for an enitre RobotModel::BodyPart of the RobotModel::Robot
														/** \param i The index of the relevant RobotModel::BodyPart
															\param v The encoder positions to use */

private:
	
	//char filterStatus;				//! colliding 'c', safe, 's', waypoint 'w'
	RobotFilter *robotFilter;		//!< The RobotFilter to which this Observer belongs
	const int bodyPart;				//!< The RobotModel::BodyPart to which this Observer applies
	CircularBuffer poseBuffer;		//!< A CircularBuffer full of previous states of the robot
};
#endif
/** @} */