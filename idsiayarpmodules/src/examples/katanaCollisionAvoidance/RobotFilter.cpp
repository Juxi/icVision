/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RobotFilter.cpp Implementation file for the RobotFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "RobotFilter.h"

#include "StateObserver.h"
#include "CallObserver.h"
#include "ResponseObserver.h"

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <iostream>

static int getNumberOfAxes(std::string remoteBaseName) {
	int failure = 0;
	std::string localBaseName = "/test/client";

    yarp::os::Property options;
    options.put("device", "remote_controlboard");
    options.put("local", localBaseName.c_str());
    options.put("remote", remoteBaseName.c_str());

	// create a device
    yarp::dev::PolyDriver robotDevice(options);
    if (!robotDevice.isValid()) {
        return failure;
    }

    // connect the position controller
    yarp::dev::IPositionControl *pos;
    bool ok = robotDevice.view(pos);
    if (!ok) {
        robotDevice.close();
        return failure;
    }

    // get the number of axes
    int numberOfAxes = 0;
    pos->getAxes(&numberOfAxes);

    robotDevice.close();

    return numberOfAxes;
}

RobotFilter::RobotFilter(QObject *parent) :
	QObject(parent), isOpen(false), robotModel(NULL), cbFilters(), observers() {
	// No special action to take for construction
}

RobotFilter::~RobotFilter() {
	if (true == isOpen) {
		close();
	}
}

bool RobotFilter::open(Robot &robot) {
	if (true == isOpen) {
		close();
	}

    // check whether a network is available
    if (false == yarp::os::Network::checkNetwork()) {
    	return false;
    }

	// Get the name of the robot. It is used as the base of the YARP port
	// name of the ports the ControlBoardFilter are going to connect to
	const QString deviceBaseName(robot.getName());
	const QString filterBaseName(robot.getName() + "F");

	// For each branch
	//   control the number of axes
	//   create a ControlBoardFilter
	//   create an observer for the encoder readings
	//   create an observer for the rpc responses
	// TODO: create observers for commands
	int numberOfAxes;
	yarp::os::ControlBoardFilter *p_cbf;
	StateObserver *p_so;
	CallObserver *p_co;
	ResponseObserver *p_ro;
	QString targetName;
	QString filterName;

	std::cout << "number of branches: " << robot.getNumBranches() << std::endl;

	for (int branch = 0; branch < robot.getNumBranches(); branch++) {
		std::cout << "branch: " << branch << std::endl;

		filterName = "/" + robot.getName() + "F/" + *(robot.getBranchName(
				branch));
		targetName = "/" + robot.getName() + "/"
				+ *(robot.getBranchName(branch));

		std::cout << "  filterName: " << filterName.toStdString() << std::endl;
		std::cout << "  targetName: " << targetName.toStdString() << std::endl;

		numberOfAxes = robot.getNumMotors(branch);
		std::cout << "  number of axes: " << numberOfAxes << std::endl;

//		if (numberOfAxes != getNumberOfAxes(targetName.toStdString())) {
//			std::cout << "##  NUMBER OF MOTORS DOES NOT MATCH!" << std::endl;
//			close();
//			return false;
//		}
//		std::cout << "  number of axes does match" << std::endl;

		p_cbf = new yarp::os::ControlBoardFilter();

		std::cout << "  ControlBoardFilter created" << std::endl;

		if (true == p_cbf->open(filterName.toStdString().c_str(),
				targetName.toStdString().c_str())) {

			std::cout << "  ControlBoardFilter opened" << std::endl;

			cbFilters.append(p_cbf);

			// create and set state observer
			p_so = new StateObserver(robot, branch);
			p_cbf->setStateObserver(p_so);
			observers.append(p_so);

			std::cout << "  StateObserver set" << std::endl;

			// create and set call observer
			p_co = new CallObserver(robot, branch);
			p_cbf->setCallObserver(p_co);
			observers.append(p_co);

			std::cout << "  CallObserver set" << std::endl;

			// create and set response observer
			p_ro = new ResponseObserver(robot, branch);
			p_cbf->setResponseObserver(p_ro);
			observers.append(p_ro);

			p_co->setResponseObsever(p_ro);

			std::cout << "  ResponseObserver set" << std::endl;
		} else {
			std::cout << "  ELSE PART" << std::endl;

			delete p_cbf;
			close();
			return false;
		}
	}

	isOpen = true;
	robotModel = &robot;
	return true;
}

void RobotFilter::close() {
	// remove all observers from the ControlBoardFilters, close the filters
	// and delete them
	QVector<yarp::os::ControlBoardFilter*>::const_iterator cbfIter;
	for (cbfIter = cbFilters.begin(); cbfIter != cbFilters.end(); ++cbfIter) {
		(*cbfIter)->setResponseObserver(NULL);
		(*cbfIter)->setCallObserver(NULL);
		(*cbfIter)->setCommandObserver(NULL);
		(*cbfIter)->setStateObserver(NULL);
		(*cbfIter)->close();
		yarp::os::ControlBoardFilter *cbf = *cbfIter;
		delete cbf;
	}
	cbFilters.clear();

	// delete all observers
	QVector<yarp::os::IObserver*>::const_iterator obsIter;
	for (obsIter = observers.begin(); obsIter != observers.end(); ++obsIter) {
		delete (*obsIter);
	}
	observers.clear();

	isOpen = false;
}

Robot* RobotFilter::getRobot() {
	return robotModel;
}

void RobotFilter::stop() {
	std::cout << "STOP!!!" << std::endl;
	QVector<yarp::os::ControlBoardFilter *>::iterator cbf;
	for (cbf = cbFilters.begin(); cbf != cbFilters.end(); ++cbf ) {
		// cut the connection
		(*cbf)->cutConnection(true);
	
		// send stop to ALL motors
		yarp::os::Bottle cmd;
		cmd.addVocab(VOCAB_SET);
		cmd.addVocab(VOCAB_STOPS);
		(*cbf)->injectCall(cmd);
	}
}