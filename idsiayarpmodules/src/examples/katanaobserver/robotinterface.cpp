/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file robotinterface.cpp Implementation file for the RobotInterface class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "robotinterface.h"

#include <iostream>

// only used in this dummy implementation
static const QString robot_name("katana400");
static const QString first_branch_name("arm");

/**
 * \note: THIS IS A DUMMY IMPLEMENTATION OF THE RobotInterface CLASS THAT IS
 *        ONLY PROVIDED TO ALLOW THE IMPLEMENTATION OF A DEMO PROGRAM FOR
 *        THE RobotFilter CLASS.
 */
RobotInterface::RobotInterface() {
	// No special action to take for construction
	for (int i = 0; i < 6; i++) {
		lastPoss[i] = -99999.9;
	}
}

// robot
bool RobotInterface::configure() {
	// Not needed in this dummy implementation
	return true;
}

const QObject* RobotInterface::getQObject() const {
	// Not needed in this dummy implementation
	return NULL;
}

const QString& RobotInterface::getName() const {
	// (YARP) name of the robot
	return robot_name;
}

int RobotInterface::getNumBranches() const {
	// get the number of branches (YARP control groups, such as 'torso')
	return 1;
}

void RobotInterface::setPos(qreal pos) {
	// Not needed in this dummy implementation
}

void RobotInterface::printJoints() {
	// Not needed in this dummy implementation
}

void RobotInterface::printBranches() {
	// Not needed in this dummy implementation
}

// branch
const QString* RobotInterface::getBranchName(int branchNum) const {
	// name of the branch (YARP control group, such as 'torso')
	const QString *branchName = NULL;
	switch (branchNum) {
	case 0:
		branchName = &first_branch_name;
		break;
	}
	return branchName;
}

int RobotInterface::getNumMotors(int branchNum) const {
	// number of motors in this control group (returns 0 if branchNum is invalid)
	int numMotors = 0;

	switch (branchNum) {
	case 0:
		numMotors = 6;
		break;
	}
	return numMotors;
}

bool RobotInterface::setPos(int branchNum, const QVector<qreal>& poss) {
	if (poss.size() != 6) {
		return false;
	}

	bool moved = false;
	const double eps = 0.0001;
	for (int i = 0; i < poss.size(); i++) {
		if (fabs(poss[i] - lastPoss[i]) > eps) {
			lastPoss[i] = poss[i];
			moved = true;
		}
	}

	if (moved == true) {
		std::cout << "set pos: (";
		for (int i = 0; i < poss.size(); i++) {
			std::cout << " " << poss[i];
		}
		std::cout << " )" << std::endl;
	}

	return true;
}

const QString* RobotInterface::getMotorName(int branchNum, int motorNum) const {
	// Not needed in this dummy implementation
	return NULL;
}

bool RobotInterface::setPos(int branchNum, int motorNum, qreal pos) {
	// Not needed in this dummy implementation
	return true;
}

// private methods
bool RobotInterface::branchIdxInRange(int idx) const {
	// Not needed in this dummy implementation
	return false;
}

bool RobotInterface::motorIdxInRange(int idx, int branchNum) const {
	// Not needed in this dummy implementation
	return false;
}
