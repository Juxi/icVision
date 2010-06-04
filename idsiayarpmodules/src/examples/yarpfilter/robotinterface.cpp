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
static const QString robot_name("icubSim");
static const QString first_branch_name("left_arm");
static const QString second_branch_name("right_arm");
static const QString third_branch_name("left_leg");
static const QString fourth_branch_name("right_leg");
static const QString fifth_branch_name("torso");
static const QString sixth_branch_name("head");

/**
 * \note: THIS IS A DUMMY IMPLEMENTATION OF THE RobotInterface CLASS THAT IS
 *        ONLY PROVIDED TO ALLOW THE IMPLEMENTATION OF A DEMO PROGRAM FOR
 *        THE RobotFilter CLASS.
 */
RobotInterface::RobotInterface() {
	// No special action to take for construction
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
	return 6;
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
	case 1:
		branchName = &second_branch_name;
		break;
	case 2:
		branchName = &third_branch_name;
		break;
	case 3:
		branchName = &fourth_branch_name;
		break;
	case 4:
		branchName = &fifth_branch_name;
		break;
	case 5:
		branchName = &sixth_branch_name;
		break;
	}
	return branchName;
}

int RobotInterface::getNumMotors(int branchNum) const {
	// number of motors in this control group (returns 0 if branchNum is invalid)
	int numMotors = 0;

	switch (branchNum) {
	case 0:
		numMotors = 16;
		break;
	case 1:
		numMotors = 16;
		break;
	case 2:
		numMotors = 6;
		break;
	case 3:
		numMotors = 6;
		break;
	case 4:
		numMotors = 3;
		break;
	case 5:
		numMotors = 6;
		break;
	}
	return numMotors;
}

bool RobotInterface::setPos(int branchNum, const QVector<qreal>& pos) {
	// Not needed in this dummy implementation
	std::cout << "branch[" << branchNum << "] - set pos: ( ";
	for (int i = 0; i < pos.size(); i++) {
		std::cout << pos[i] << " ";
	}
	std::cout << " )" << std::endl;
	return true;
}

const QString* RobotInterface::getMotorName(int branchNum, int motorNum) const {
	// Not needed in this dummy implementation
	return NULL;
}

bool RobotInterface::setPos(int branchNum, int motorNum, qreal pos) {
	// Not needed in this dummy implementation
	std::cout << "branch[" << branchNum << "] - set pos[" << motorNum << "]: "
			<< pos << std::endl;
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
