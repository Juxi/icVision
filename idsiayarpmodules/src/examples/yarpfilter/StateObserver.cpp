/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file StateObserver.cpp Implementation file for the StateObserver class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "StateObserver.h"

#include <QtGlobal>
#include <QVector>

StateObserver::StateObserver(RobotInterface &r, const int b) :
	IObserver(), robot(r), branch(b) {
	// No special action to take for construction
}

StateObserver::~StateObserver() {
	// Nothing to destruct here
}

void StateObserver::onDataObserved(yarp::os::Bottle &b) {
	QVector<qreal> poss;
	for (int i = 0; i < b.size(); i++) {
		poss.append((qreal) b.get(i).asDouble());
	}
	robot.setPos(branch, poss);
}
