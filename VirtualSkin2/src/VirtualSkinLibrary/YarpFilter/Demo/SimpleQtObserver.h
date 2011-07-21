/*
 * SimpleQtObserver.h
 *
 *  Created on: Mar 9, 2010
 *      Author: gregor
 */

#ifndef SIMPLEQTOBSERVER_H_
#define SIMPLEQTOBSERVER_H_

#include <yarp/os/all.h>
#include <yarp/os/IObserver.h>
#include <QString>
#include <QObject>

class SimpleQtObserver: public QObject, public yarp::os::IObserver {
Q_OBJECT

public:
	SimpleQtObserver(QObject *parent = 0);
	virtual ~SimpleQtObserver();

	virtual void onDataObserved(yarp::os::Bottle &b);

signals:
	void dataObserved(const QString &);

private:
	// a text representation of the message in the bottle
	QString m_message_text;
};

#endif /* SIMPLEQTOBSERVER_H_ */
