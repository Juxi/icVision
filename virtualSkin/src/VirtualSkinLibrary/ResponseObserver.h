/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file ResponseObserver.h Header file for the ResponseObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef RESPONSEOBSERVER_H_
#define RESPONSEOBSERVER_H_

#include <QObject>
#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>

namespace VirtualSkin {
	
class CallObserver;
class RobotFilter;

class ResponseObserver: public QObject, public yarp::os::IObserver
{
	Q_OBJECT
	
public:
	ResponseObserver(RobotFilter* r, const int b);
	virtual ~ResponseObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);
	
signals:
	
	void setPosition(int,int,qreal);

private:
	RobotFilter* robotFilter;
	const int bodyPart;

	// the actual open RPC call
	yarp::os::Bottle activeCall;

	friend class CallObserver;
};
}
#endif /* RESPONSEOBSERVER_H_ */
