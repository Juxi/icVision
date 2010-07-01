// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_KATANA400_
#define _YARP2_KATANA400_

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <kniBase.h>
#include <KNI/kmlMotBase.h>

#include <stdio.h>

namespace yarp {
	namespace dev {
		class Katana400;
	}
}

class yarp::dev::Katana400: public DeviceDriver,
		public IPositionControlRaw,
		public IPositionControl,
		public IEncoders,
		public IVelocityControl {

private:
	yarp::os::Semaphore mutex;

	std::auto_ptr<CCdlSocket> device;
	std::auto_ptr<CCplSerialCRC> protocol;
	std::auto_ptr<CLMBase> katana;

	short njoints;
	yarp::sig::Vector encOff, encMin, encMax, encPerCycle, angleOff, rotDir,
			targets;
	//	yarp::sig::Vector yarpEncOff; // , speed, acc;

	bool isCalibrated();
	inline double positionDegreeToPositionEnc(int j, double pos);
	inline double positionEncToPositionDegree(int j, double pos);
	inline bool isRawMoveOk(int j, double ref);
	inline double boundMove(int j, double ref);

	// IEncodersRaw
	//	virtual bool resetEncoderRaw(int j);
	//	virtual bool resetEncodersRaw();
	//	virtual bool setEncoderRaw(int j, double val);
	//	virtual bool setEncodersRaw(const double *vals);

	virtual bool getEncoderRaw(int j, double *v);
	virtual bool getEncodersRaw(double *encs);
	virtual bool getEncoderSpeedRaw(int j, double *sp);
	virtual bool getEncoderSpeedsRaw(double *spds);

	//	virtual bool getEncoderAccelerationRaw(int j, double *spds);
	//	virtual bool getEncoderAccelerationsRaw(double *accs);

	// IVelocityControlRaw
	//	virtual bool setVelocityMode();

	virtual bool velocityMoveRaw(int j, double sp);
	virtual bool velocityMoveRaw(const double *sp);

	//	virtual bool setRefAccelerationRaw(int j, double acc);
	//	virtual bool setRefAccelerationsRaw(const double *accs);
	//	virtual bool getRefAccelerationRaw(int j, double *acc);
	//	virtual bool getRefAccelerationsRaw(double *accs);
	//	virtual bool stopRaw(int j);
	//	virtual bool stopRaw();


	// IAmplifierControlRaw

	// IPidControlRaw

	// IControlDebug

	// IControlLimitsRaw

public:
	Katana400();

	// Implementing the DeviceDriver interface
	virtual bool open(yarp::os::Searchable& config);
	virtual bool close();

	// this is part of the following interfaces:
	//   IPositionControlRaw, IPositionControl, IEncodersRaw, IEncoders,
	//   IVelocityControlRaw, IVelocityControl, ...
	virtual bool getAxes(int *ax);

	virtual bool setPositionMode() {
		return true;
	}

	// Implementing the IPositionControlRaw interface
	virtual bool positionMoveRaw(int j, double ref);
	virtual bool positionMoveRaw(const double *refs);
	virtual bool relativeMoveRaw(int j, double delta);
	virtual bool relativeMoveRaw(const double *refs);
	virtual bool checkMotionDoneRaw(int j, bool *flag);
	virtual bool checkMotionDoneRaw(bool *flag);
	virtual bool setRefSpeedRaw(int j, double sp);
	virtual bool setRefSpeedsRaw(const double *spds);
	virtual bool setRefAccelerationRaw(int j, double acc);
	virtual bool setRefAccelerationsRaw(const double *accs);
	virtual bool getRefSpeedRaw(int j, double *ref);
	virtual bool getRefSpeedsRaw(double *spds);
	virtual bool getRefAccelerationRaw(int j, double *acc);
	virtual bool getRefAccelerationsRaw(double *accs);
	virtual bool stopRaw(int j);
	virtual bool stopRaw();

	// Implementing the IPositionControl interface
	virtual bool positionMove(int j, double ref);
	virtual bool positionMove(const double *refs);
	virtual bool relativeMove(int j, double delta);
	virtual bool relativeMove(const double *deltas);
	virtual bool checkMotionDone(int j, bool *flag);
	virtual bool checkMotionDone(bool *flag);
	virtual bool setRefSpeed(int j, double sp);
	virtual bool setRefSpeeds(const double *spds);
	virtual bool setRefAcceleration(int j, double acc);
	virtual bool setRefAccelerations(const double *accs);
	virtual bool getRefSpeed(int j, double *ref);
	virtual bool getRefSpeeds(double *spds);
	virtual bool getRefAcceleration(int j, double *acc);
	virtual bool getRefAccelerations(double *accs);
	virtual bool stop(int j);
	virtual bool stop();

	// Implementing the IEncoders interface
	virtual bool resetEncoder(int j) {
		// I do not know precisely what to do here
		return false;
	}

	virtual bool resetEncoders() {
		// I do not know precisely what to do here
		return false;
	}

	virtual bool setEncoder(int j, double val) {
		// I do not know precisely what to do here
		return false;
	}

	virtual bool setEncoders(const double *vals) {
		// I do not know precisely what to do here
		return false;
	}

	virtual bool getEncoder(int j, double *v);
	virtual bool getEncoders(double *encs);
	virtual bool getEncoderSpeed(int j, double *sp);
	virtual bool getEncoderSpeeds(double *spds);

	virtual bool getEncoderAcceleration(int j, double *spds) {
		// I do not know precisely what to do here
		return false;
	}

	virtual bool getEncoderAccelerations(double *accs) {
		// I do not know precisely what to do here
		return false;
	}

	// Implementing the IVelocityControl interface
	virtual bool setVelocityMode() {
		return true;
	}

	virtual bool velocityMove(int j, double sp);
	virtual bool velocityMove(const double *sp);

};

#endif  /* _YARP2_KATANA400_ */
