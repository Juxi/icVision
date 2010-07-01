// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "Katana400.h"

#include <yarp/os/all.h>

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace yarp::dev;

Katana400::Katana400() :
	mutex(1), device(), protocol(), katana(), njoints(0), encOff(), encMin(),
			encMax(), encPerCycle(), angleOff(), rotDir(), targets() /*,
 yarpEncOff() */{
}

bool Katana400::open(yarp::os::Searchable& config) {
	mutex.wait();

	// get the configuration parameters of the KNI driver
	// TODO - Fix default values for IP and port
	yarp::os::Value ipAddress = config.findGroup("KNI").check("ipAddr",
			"192.168.168.232", "[KNI] ipAddr must be set");
	int port = config.findGroup("KNI").check("port", "5566",
			"[KNI] ipAddr must be set").asInt();
	yarp::os::Value configFile = config.findGroup("KNI").check("configfile",
			"katana6M180.cfg", "[KNI] configfile must be set");

	// set up the KNI driver
	try {
		//--------------------------------------------------------//
		// init the communication socket:
		//--------------------------------------------------------//
		device.reset(new CCdlSocket(
				const_cast<char *> (ipAddress.asString().c_str()), port));
		std::cout << "-------------------------------------------\n";
		std::cout << "success:  port " << port << " open\n";
		std::cout << "-------------------------------------------\n";

		//--------------------------------------------------------//
		// init protocol:
		//--------------------------------------------------------//
		protocol.reset(new CCplSerialCRC());
		protocol->init(device.get());
		std::cout << "-------------------------------------------\n";
		std::cout << "success: protocol initiated\n";
		std::cout << "-------------------------------------------\n";

		//--------------------------------------------------------//
		// init robot:
		//--------------------------------------------------------//
		katana.reset(new CLMBase());
		katana->create(configFile.asString().c_str(), protocol.get());
	} catch (std::exception &e) {
		// FIXME: Exception is not thrown when device.reset does not work -
		//        check handling of exceptions in KNI
		std::cerr << "ERROR: " << e.what() << std::endl;
		mutex.post();
		return false;
	}

	// get information on the actual robot configuration
	njoints = katana->getNumberOfMotors();
	encOff.size(njoints);
	encMin.size(njoints);
	encMax.size(njoints);
	encPerCycle.size(njoints);
	angleOff.size(njoints);
	rotDir.size(njoints);
	targets.size(njoints);

	// calibrate the robot if necessary
	if (isCalibrated() == false) {
		katana->calibrate();
	}
	for (int j = 0; j < njoints; j++) {
		targets[j] = katana->getMotorEncoders(j);
		std::cout << "Motor " << j << ":" << std::endl;
		std::cout << "  initial target:     " << targets[j] << std::endl;
	}

	// read the configuration
	for (int j = 0; j < njoints; j++) {
		const TMotInit *motInit =
				katana->GetBase()->GetMOT()->arr[j].GetInitialParameters();
		encOff[j] = motInit->encoderOffset;
		encMin[j] = katana->GetBase()->GetMOT()->arr[j].GetEncoderMinPos();
		encMax[j] = katana->GetBase()->GetMOT()->arr[j].GetEncoderMaxPos();
		encPerCycle[j] = motInit->encodersPerCycle;
		angleOff[j] = motInit->angleOffset;
		rotDir[j] = motInit->rotationDirection;

		// TODO: remove after initial verification
		std::cout << "Motor " << j << ":" << std::endl;
		std::cout << "  encoder off:       " << encOff[j] << std::endl;
		std::cout << "  encoder min:       " << encMin[j] << std::endl;
		std::cout << "  encoder max:       " << encMax[j] << std::endl;
		std::cout << "  encoder per cycle: " << encPerCycle[j] << std::endl;
		std::cout << "  angle off:         " << angleOff[j] << std::endl;
		std::cout << "  rotation dir:      " << rotDir[j] << std::endl;
	}

	// try to handle gripper correctly
	try {
		katana->openGripper(true);
		encMax[njoints - 1] = katana->getMotorEncoders(njoints - 1, true);
		std::cout << "Gripper (Motor " << (njoints - 1) << "):" << std::endl;
		std::cout << "  encoder max:       " << encMax[njoints - 1]
				<< std::endl;
	} catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}

	//	yarpEncOff.size(njoints);
	//	speed.size(njoints);
	//	acc.size(njoints);
	for (int i = 0; i < njoints; i++) {
		//		yarpEncOff[i] = 0;
		//		speed[i] = 0;
		//		acc[i] = 0;
	}

	mutex.post();
	return true;
}

bool Katana400::close() {
	mutex.wait();

	// yarpEncOff.clear();
	targets.clear();
	rotDir.clear();
	angleOff.clear();
	encPerCycle.clear();
	encMax.clear();
	encMin.clear();
	encOff.clear();

	njoints = 0;

	katana.reset();
	protocol.reset();
	device.reset();

	mutex.post();
	return true;
}

//
// Implementing the IPositionControl interface
//

bool Katana400::positionMoveRaw(int j, double ref) {
	//	if (!isRawMoveOk(j, ref)) {
	//		return false;
	//	}
	targets[j] = boundMove(j, ref);

	try {
		mutex.wait();
		katana->moveMotorToEnc(j, targets[j], false);
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::positionMoveRaw(const double *refs) {
	std::vector<int> refVec;

	for (int j = 0; j < njoints; j++) {
		//		if (!isRawMoveOk(j, refs[j])) {
		//			return false;
		//		}
		targets[j] = boundMove(j, refs[j]);
		refVec.push_back(targets[j]);
	}

	try {
		mutex.wait();
		katana->moveRobotToEnc(refVec, false);
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::relativeMoveRaw(int j, double delta) {
	if (j < 0 || j >= njoints) {
		return false;
	}

	try {
		mutex.wait();
		targets[j] = boundMove(j, katana->getMotorEncoders(j, true) + delta);
		katana->mov(j, targets[j], false);
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::relativeMoveRaw(const double *deltas) {
	std::vector<int> refVec;

	try {
		mutex.wait();
		for (int j = 0; j < njoints; j++) {
			targets[j] = boundMove(j, katana->getMotorEncoders(j, true)
					+ deltas[j]);
			refVec.push_back(targets[j]);
		}
		katana->moveRobotToEnc(refVec, false);
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::checkMotionDoneRaw(int j, bool *flag) {
	*flag = false;
	if (j >= njoints || j < 0) {
		return false;
	}

	const int tol = 50;

	try {
		mutex.wait();
		katana->GetBase()->GetMOT()->arr[j].recvPVP();
		const TMotPVP * pvp = katana->GetBase()->GetMOT()->arr[j].GetPVP();
		if ((pvp->msf == MSF_DESPOS) && (pvp->pos + tol >= targets[j])
				&& (pvp->pos - tol <= targets[j])) {
			*flag = true;
		}
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::checkMotionDoneRaw(bool *flag) {
	*flag = true;
	for (int j = 0; j < njoints; j++) {
		if (checkMotionDoneRaw(j, flag) == false) {
			return false;
		}
		if (*flag == false) {
			break;
		}
	}
	return true;
}

bool Katana400::setRefSpeedRaw(int j, double sp) {
	if (j >= njoints || j < 0) {
		return false;
	}

	try {
		mutex.wait();
		katana->setMotorVelocityLimit(j, sp);
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::setRefSpeedsRaw(const double *spds) {
	bool success = true;
	for (int j = 0; j < njoints; j++) {
		if (setRefSpeed(j, spds[j]) == false) {
			success = false;
		}
	}
	return success;
}

bool Katana400::setRefAccelerationRaw(int j, double acc) {
	if (j >= njoints || j < 0) {
		return false;
	}

	try {
		mutex.wait();
		katana->setMotorAccelerationLimit(j, acc);
		mutex.post();
	} catch (std::exception &e) {
		mutex.post();
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool Katana400::setRefAccelerationsRaw(const double *accs) {
	bool success = true;
	for (int j = 0; j < njoints; j++) {
		if (setRefAcceleration(j, accs[j]) == false) {
			success = false;
		}
	}
	return success;
}

bool Katana400::getRefSpeedRaw(int j, double *ref) {
	if (j >= njoints || j < 0) {
		return false;
	}

	mutex.wait();
	*ref = katana->getMotorVelocityLimit(j);
	mutex.post();

	return true;
}

bool Katana400::getRefSpeedsRaw(double *spds) {
	bool success = true;
	for (int j = 0; j < njoints; j++) {
		if (getRefSpeed(j, &(spds[j])) == false) {
			success = false;
		}
	}
	return success;
}

bool Katana400::getRefAccelerationRaw(int j, double *acc) {
	if (j >= njoints || j < 0) {
		return false;
	}

	mutex.wait();
	*acc = katana->getMotorAccelerationLimit(j);
	mutex.post();

	return true;
}

bool Katana400::getRefAccelerationsRaw(double *accs) {
	bool success = true;
	for (int j = 0; j < njoints; j++) {
		if (getRefAcceleration(j, &(accs[j])) == false) {
			success = false;
		}
	}
	return success;
}

bool Katana400::stopRaw(int j) {
	mutex.wait();

	katana->freezeMotor(j);

	mutex.post();
	return true;
}

bool Katana400::stopRaw() {
	mutex.wait();
	katana->freezeRobot();
	mutex.post();

	return true;
}

//
// Implementing the IPositionControl interface
//

// getAxes is part of the IPositionControl as well as IEncoder interfaces
bool Katana400::getAxes(int *ax) {
	*ax = njoints;
	return true;
}

bool Katana400::positionMove(int j, double ref) {
	return positionMoveRaw(j, positionDegreeToPositionEnc(j, ref));
}

bool Katana400::positionMove(const double *refs) {
	double enc_refs[njoints];
	for (int j = 0; j < njoints; j++) {
		enc_refs[j] = positionDegreeToPositionEnc(j, refs[j]);
	}
	return positionMoveRaw(enc_refs);
}

bool Katana400::relativeMove(int j, double delta) {
	return relativeMoveRaw(j, (delta / 360.0) * encPerCycle[j] * (-rotDir[j]));
}

bool Katana400::relativeMove(const double *deltas) {
	double enc_deltas[njoints];
	for (int j = 0; j < njoints; j++) {
		enc_deltas[j] = (deltas[j] / 360.0) * encPerCycle[j] * (-rotDir[j]);
	}
	return relativeMoveRaw(enc_deltas);
}

bool Katana400::checkMotionDone(int j, bool *flag) {
	return checkMotionDoneRaw(j, flag);
}

bool Katana400::checkMotionDone(bool *flag) {
	return checkMotionDoneRaw(flag);
}

bool Katana400::setRefSpeed(int j, double sp) {
	return setRefSpeedRaw(j, sp);
}

bool Katana400::setRefSpeeds(const double *spds) {
	return setRefSpeedsRaw(spds);
}

bool Katana400::setRefAcceleration(int j, double acc) {
	return setRefAccelerationRaw(j, acc);
}

bool Katana400::setRefAccelerations(const double *accs) {
	return setRefAccelerationsRaw(accs);
}

bool Katana400::getRefSpeed(int j, double *ref) {
	return getRefSpeedRaw(j, ref);
}

bool Katana400::getRefSpeeds(double *spds) {
	return getRefSpeedsRaw(spds);
}

bool Katana400::getRefAcceleration(int j, double *acc) {
	return getRefAccelerationRaw(j, acc);
}

bool Katana400::getRefAccelerations(double *accs) {
	return getRefAccelerationsRaw(accs);
}

bool Katana400::stop(int j) {
	return stopRaw(j);
}

bool Katana400::stop() {
	return stopRaw();
}

//
// Implementing the IEncoders interface
//
bool Katana400::getEncoder(int j, double *v) {
	double enc;
	if (getEncoderRaw(j, &enc) == false) {
		return false;
	}
	*v = positionEncToPositionDegree(j, enc);

	return true;
}

bool Katana400::getEncoders(double *encs) {
	double encsraw[njoints];
	if (getEncodersRaw(encsraw) == false) {
		return false;
	}
	for (int j = 0; j < njoints; j++) {
		encs[j] = positionEncToPositionDegree(j, encsraw[j]);
	}

	return true;
}

bool Katana400::getEncoderSpeed(int j, double *sp) {
	return getEncoderSpeedRaw(j, sp);
}

bool Katana400::getEncoderSpeeds(double *spds) {
	return getEncoderSpeedsRaw(spds);
}

//
// Implementing the IVelocityControl interface
//
bool Katana400::velocityMove(int j, double sp) {
	return velocityMoveRaw(j, sp);
}

bool Katana400::velocityMove(const double *sp) {
	return velocityMoveRaw(sp);
}

// --------------------------------------------------------------------
// private method implementations
// --------------------------------------------------------------------
bool Katana400::isCalibrated() {
	double pos;
	pos = katana->getMotorEncoders(0, true);
	try {
		if (pos - 1 <= encMin[0]) {
			katana->mov(0, pos + 1, true);
		} else {
			katana->mov(0, pos - 1, true);
		}
		katana->mov(0, pos, true);
	} catch (FirmwareException &e) {
		return false;
	} catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return false;
	}

	return true;
}

// angles are given in degrees (NOT radians)
double Katana400::positionDegreeToPositionEnc(int j, double pos) {
	double encPos = (((pos * 2.0 * 3.141592654 / 360.0) - angleOff[j])
			* (encPerCycle[j] / (2.0 * 3.141592654)) * (-rotDir[j]))
			+ encOff[j];
	return encPos;
}

// angles are given in degrees (NOT radians)
double Katana400::positionEncToPositionDegree(int j, double pos) {
	double decPos = ((((pos - encOff[j]) / -rotDir[j]) * (2 * 3.141592645
			/ encPerCycle[j])) + angleOff[j]) * (360.0 / (2 * 3.141592645));
	return decPos;
}

bool Katana400::isRawMoveOk(int j, double ref) {
	const double epsilon = 1e-5;

	// is j in range?
	if (j < 0 || j >= njoints) {
		return false;
	}

	// is ref in range?
	if (ref < (encMin[j] - epsilon) || ref > (encMax[j] + epsilon)) {
		std::cout << "## move for joint " << j << " (" << ref
				<< ") is out of range (" << encMin[j] << "," << encMax[j]
				<< ")" << std::endl;
		return false;
	}

	return true;
}

double Katana400::boundMove(int j, double ref) {
	if (ref < encMin[j]) {
		return encMin[j];
	} else if (ref > encMax[j]) {
		return encMax[j];
	}
	return ref;
}

bool Katana400::getEncoderRaw(int j, double *v) {
	if (j >= njoints || j < 0) {
		return false;
	}

	mutex.wait();
	*v = katana->getMotorEncoders(j, true);
	mutex.post();

	return true;
}

bool Katana400::getEncodersRaw(double *encs) {
	std::vector<int> enc_v;

	mutex.wait();
	enc_v = katana->getRobotEncoders(true);
	mutex.post();

	for (int j = 0; j < njoints; j++) {
		encs[j] = enc_v[j];
	}

	return true;
}

bool Katana400::getEncoderSpeedRaw(int j, double *sp) {
	if (j >= njoints || j < 0) {
		return false;
	}

	mutex.wait();
	katana->GetBase()->GetMOT()->arr[j].recvPVP();
	*sp = katana->GetBase()->GetMOT()->arr[j].GetPVP()->vel;
	mutex.post();

	return true;
}

bool Katana400::getEncoderSpeedsRaw(double *spds) {
	bool success = true;
	for (int j = 0; j < njoints; j++) {
		if (getEncoderSpeedRaw(j, &(spds[j])) == false) {
			success = false;
		}
	}
	return success;
}

bool Katana400::velocityMoveRaw(int j, double sp) {
	if (j >= njoints || j < 0) {
		return false;
	}

	if (setRefSpeedRaw(j, fabs(sp)) == false) {
		return false;
	}

	if (sp < 0.0) {
		targets[j] = encMax[j];
	} else {
		targets[j] = encMin[j];
	}

	return positionMoveRaw(j, targets[j]);
}

bool Katana400::velocityMoveRaw(const double *sp) {
	bool success = true;

	for (int j = 0; j < njoints; j++) {
		if (velocityMoveRaw(j, sp[j]) == false) {
			success = false;
		}
	}

	return success;
}
