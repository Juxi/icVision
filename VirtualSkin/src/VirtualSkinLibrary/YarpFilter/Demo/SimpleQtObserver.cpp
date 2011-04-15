/*
 * SimpleQtObserver.cpp
 *
 *  Created on: Mar 9, 2010
 *      Author: gregor
 */

#include "SimpleQtObserver.h"
#include "SimpleQtObserver.moc"

SimpleQtObserver::SimpleQtObserver(QObject *parent) :
	QObject(parent), yarp::os::IObserver(), m_message_text("") {
	// No special action to take for construction
}

SimpleQtObserver::~SimpleQtObserver() {
	// Nothing to destruct here
}

void SimpleQtObserver::onDataObserved(yarp::os::Bottle &b) {
	m_message_text = b.toString();
	emit dataObserved(m_message_text);
}
