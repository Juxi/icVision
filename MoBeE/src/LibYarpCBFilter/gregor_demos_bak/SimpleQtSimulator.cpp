/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file SimpleQtSimulator.cpp Implementation file for the \c SimpleQtSimulator
 * class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "SimpleQtSimulator.h"
#include "SimpleQtSimulator.moc"

SimpleQtSimulator::SimpleQtSimulator(QObject *parent) :
	QObject(parent), yarp::os::IObserver(), yarp::os::IReplier(),
			lastObservedBottle(), answered(false), cbf(NULL), isCut(false) {
}

SimpleQtSimulator::~SimpleQtSimulator() {
}

void SimpleQtSimulator::onDataObserved(yarp::os::Bottle &b) {
	lastObservedBottle = b;
	answered = false;

	m_message_text = b.toString();
	emit dataObserved(m_message_text);

	if (NULL != cbf) {
		if (true == m_message_text.contains("[set] [pos] 0 20.0")) {
			isCut = !isCut;
			cbf->cutConnection(isCut);
			emit cutStatusChanged(isCut ? "connection CUT" : "connected");
		}
	}
}

yarp::os::Bottle* SimpleQtSimulator::getNextReponse() {
	yarp::os::Bottle* response(new yarp::os::Bottle());
	response->clear();

//	yarp::os::Time::delay(1.0);

	QString bottleText(lastObservedBottle.toString());

	if (false == answered) {
		answered = true;

		// [get] [axes] -> [is] [axes] 16 [ok]
		if (true == bottleText.contains("[get] [axes]")) {
			response->addVocab(yarp::os::Vocab::encode("is"));
			response->addVocab(yarp::os::Vocab::encode("axes"));
			response->addInt(16);
			response->addVocab(yarp::os::Vocab::encode("ok"));
			return response;
		}

		// [set] [vel] -> [ok]
		if (true == bottleText.contains("[set] [vel]")) {
			response->addVocab(yarp::os::Vocab::encode("ok"));
			return response;
		}

		// [get] [dons] -> [is] [dons] 1 [ok]
		if (true == bottleText.contains("[get] [dons]")) {
			response->addVocab(yarp::os::Vocab::encode("is"));
			response->addVocab(yarp::os::Vocab::encode("dons"));
			response->addInt(1);
			response->addVocab(yarp::os::Vocab::encode("ok"));
			return response;
		}

		// [set] [pos] -> [ok]
		if (true == bottleText.contains("[set] [pos]")) {
			response->addVocab(yarp::os::Vocab::encode("ok"));
			return response;
		}

		// all other bottles
		response->addVocab(yarp::os::Vocab::encode("fail"));
		return response;
	}

	return NULL;
}

void SimpleQtSimulator::setControlBoardFilter(yarp::os::ControlBoardFilter *cbf) {
	SimpleQtSimulator::cbf = cbf;
}

