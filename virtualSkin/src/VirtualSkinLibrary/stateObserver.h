/*******************************************************************
 ***              Copyright (C) 2010 Gregor Kaufmann             ***
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef STATEOBSERVER_H_
#define STATEOBSERVER_H_

#include <QObject>
#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include "circularBuffer.h"
#include "robot.h"

namespace VirtualSkin {
	
class RobotFilter;
	
/** \brief An IObserver implemented to monitor streaming state information (motor encoder values)
 */
class StateObserver: public QObject, public yarp::os::IObserver
{
	Q_OBJECT
	
public:
	
	StateObserver(RobotFilter *f, const int b);		//!< Nothing special to do here
													/**< \param r The RobotFilter to which this Observer belongs
														 \param b The index of the RobotModel::BodyPart to which this Observer applies */
	virtual ~StateObserver();						//!< Nothing special to do here

	virtual void onDataObserved(yarp::os::Bottle &b);	//!< The handler function is called whenever state information passes through the filter
	
	QVector<qreal> nonCollidingPose() { return poseBuffer.getOldest(); }	//!< Returns the oldest pose in the PoseBuffer (which can't be colliding)
	QVector<qreal> currentPose() { return poseBuffer.getCurrent(); }		//!< Returns the most recent pose written to the PoseBuffer
	void initPoseBuffer( const QVector<qreal>& v ) { poseBuffer.init(v); }	//!< Fills every slot in the PoseBuffer with the vector passed in as a parameter
	
signals:
	void setPosition( int i, const QVector<qreal>& v);	//!< Set the motor encoder positions for an enitre RobotModel::BodyPart of the RobotModel::Robot
														/** \param i The index of the relevant RobotModel::BodyPart
															\param v The encoder positions to use */

private:
	
	RobotFilter *robotFilter;		//!< The RobotFilter to which this Observer belongs
	const int bodyPart;				//!< The RobotModel::BodyPart to which this Observer applies
	CircularBuffer poseBuffer;		//!< A CircularBuffer full of previous states of the robot
};
}
#endif
/** @} */