/*******************************************************************
 ***              Copyright (C) 2010 Gregor Kaufmann             ***
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef RESPONSEOBSERVER_H_
#define RESPONSEOBSERVER_H_

#include <QObject>
#include <yarp/os/all.h>
#include "yarp/os/IObserver.h"

namespace MoBeE
{	
	class CallObserver;
	class RobotFilter;
	class ResponseObserver;
}

/** \brief An IObserver implemented to monitor RPC responses
 */
class MoBeE::ResponseObserver: public QObject, public yarp::os::IObserver
{
	Q_OBJECT
	
public:
	ResponseObserver(RobotFilter* r, const int b);		//!< Nothing special to do here
														/**< \param r The RobotFilter to which this Observer belongs
															 \param b The index of the RobotModel::BodyPart to which this Observer applies */
	virtual ~ResponseObserver();						//!< Nothing special to do here

	virtual void onDataObserved(yarp::os::Bottle &b);	//!< The handler function is called whenever an RPC response passes through the filter
	
signals:
	
	void setPosition(int b,int j, qreal pos);			//!< Sets the position of a RobotModel::Motor in the RobotModel::Robot
														/** \param b The index of the relevant RobotModel::BodyPart
															\param j The index of the relevant RobotModel::Motor
															\param pos The encoder position to use */

private:
	RobotFilter* robotFilter;			//!< The RobotFilter to which this Observer belongs
	const int bodyPart;					//!< The RobotModel::BodyPart to which this Observer applies
	yarp::os::Bottle activeCall;		//!< The current open RPC call

	friend class CallObserver;
};
#endif /* RESPONSEOBSERVER_H_ */
/** @} */