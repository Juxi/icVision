/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file SimpleQtSimulator.h Header file for the \c SimpleQtSimulator class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#ifndef SIMPLEQTSIMULATOR_H_
#define SIMPLEQTSIMULATOR_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include <yarp/os/IReplier.h>
#include <yarp/os/ControlBoardFilter.h>
#include <QString>
#include <QObject>

class SimpleQtSimulator: public QObject,
		public yarp::os::IObserver,
		public yarp::os::IReplier {
Q_OBJECT

public:
	SimpleQtSimulator(QObject *parent = 0);
	virtual ~SimpleQtSimulator();

	virtual void onDataObserved(yarp::os::Bottle &b);
	virtual yarp::os::Bottle* getNextReponse();

	void setControlBoardFilter(yarp::os::ControlBoardFilter *cbf);

signals:
	void dataObserved(const QString &);
	void cutStatusChanged(const QString &);

private:
	yarp::os::Bottle lastObservedBottle;
	bool answered;

	// a text representation of the message in the bottle
	QString m_message_text;

	// the ControlBoardFilter that may be cut
	yarp::os::ControlBoardFilter *cbf;
	bool isCut;
};

#endif /* SIMPLEQTSIMULATOR_H_ */
