// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "Katana400.h"

#include <yarp/os/Property.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>

#include <stdio.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::dev;

void testMotor(PolyDriver& driver) {
	IPositionControlRaw *rawpos;
	IPositionControl *pos;
	IEncoders *enc;
	IVelocityControl *vel;

	if (driver.view(enc)) {
		int ct = 0;
		enc->getAxes(&ct);

		double pos;
		for (int j = 0; j < ct; j++) {
			enc->getEncoder(j, &pos);
			std::cout << "  " << j << ": " << pos << std::endl;
		}
	} else {
		printf("  could not find IPositionControl interface\n");
	}

//	if (driver.view(rawpos) && driver.view(enc)) {
//		int ct = 0;
//		rawpos->getAxes(&ct);
//		printf("  number of axes is: %d\n", ct);
//
//		// gripper open: 30500.0
//		const double poss00[] = { 30500.0, -30500.0, -30500.0, 30500.0, 30500.0, 15860.0 };
//		const double poss01[] = {  6766.0, -30982.0, -30991.0, 30989.0,  6291.0, 15860.0 };
//		const double poss02[] = {  6664.0, -16395.0, -18521.0, 11667.0,  6387.0, 15860.0 };
//		const double poss03[] = { -4402.0, -16395.0, -18521.0, 11667.0,  6387.0, 15860.0 };
//		const double poss04[] = { 17468.0, -16395.0, -18521.0, 11667.0,  6387.0, 15860.0 };
//		const double poss05[] = { 11698.0, -16395.0, -18521.0, 11667.0,  6387.0, 15860.0 };
//		const double poss06[] = {   566.0, -16395.0, -18521.0, 11667.0,  6387.0, 15860.0 };
//		const double poss07[] = {  6262.0, -10656.0, -17945.0,   153.0,  6387.0, 15860.0 };
//		const double poss08[] = { -3397.0,  -7115.0, -14690.0,  -760.0,  6387.0, 15860.0 };
//		const double poss09[] = {  6262.0, -21636.0, -26131.0,  2580.0,  6387.0, 15860.0 };
//		const double poss10[] = { 16458.0,  -7116.0, -14691.0,  -759.0,  6387.0, 15860.0 };
//		const double poss11[] = { 11772.0,   -597.0, -12571.0,  7336.0,  6387.0, 15860.0 };
//		const double poss12[] = {  6562.0,  -2938.0, -11510.0,  1957.0,  6387.0, 15860.0 };
//		const double poss13[] = {  1428.0,   -597.0, -12571.0,  7336.0,  6387.0, 15860.0 };
//
//		bool motionDone = false;
//		double pos;
//
//		// move to poss-00
//		rawpos->positionMoveRaw(poss00);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 000\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-01
//		rawpos->positionMoveRaw(poss01);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 001\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 002\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-03
//		rawpos->positionMoveRaw(poss03);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 003\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-04
//		rawpos->positionMoveRaw(poss04);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 004\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-05
//		rawpos->positionMoveRaw(poss05);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 005\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-06
//		rawpos->positionMoveRaw(poss06);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 006\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-07
//		rawpos->positionMoveRaw(poss07);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 007\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-08
//		rawpos->positionMoveRaw(poss08);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 008\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-09
//		rawpos->positionMoveRaw(poss09);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 009\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-10
//		rawpos->positionMoveRaw(poss10);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 010\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-11
//		rawpos->positionMoveRaw(poss11);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 011\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-12
//		rawpos->positionMoveRaw(poss12);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 012\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-13
//		rawpos->positionMoveRaw(poss13);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//		printf("POINT 013\n");
//		for (int j = 0; j < ct; j++) {
//			enc->getEncoder(j, &pos);
//			std::cout << "  " << j << ": " << pos << std::endl;
//		}
//
//		// move to poss-02
//		rawpos->positionMoveRaw(poss02);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-01
//		rawpos->positionMoveRaw(poss01);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//
//		// move to poss-00
//		rawpos->positionMoveRaw(poss00);
//		do {
//			yarp::os::Time::delay(0.5);
//			rawpos->checkMotionDoneRaw(&motionDone);
//		} while (motionDone == false);
//	} else {
//		printf("  could not find IPositionControlRaw interface\n");
//	}

	if (driver.view(pos)) {
		int ct = 0;
		pos->getAxes(&ct);
		printf("  number of axes is: %d\n", ct);

		const double angles00[] = {  10.1656, 122.355,   56.4904,  67.0156,  12.0156, -16.8664 };
		const double angles01[] = { 177.052,  124.167,   52.791,   63.6195, 182.235,  -16.8594 };
		const double angles02[] = { 177.763,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
		const double angles03[] = { 255.57,   68.8908,  147.302,  199.435,  181.56,   -16.8523 };
		const double angles04[] = { 101.797,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
		const double angles05[] = { 142.367,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
		const double angles06[] = { 220.639,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
		const double angles07[] = { 180.589,  47.1375,  151.668,  280.393,  181.56,   -16.8523 };
		const double angles08[] = { 248.504,  33.7155,  176.344,  286.813,  181.56,   -16.8523 };
		const double angles09[] = { 180.589,  88.7564,   89.6112, 263.328,  181.56,   -16.8523 };
		const double angles10[] = { 108.898,  33.7193,  176.336,  286.805,  181.56,   -16.8453 };
		const double angles11[] = { 141.847,   9.00573, 192.408,  229.888,  181.56,   -16.8453 };
		const double angles12[] = { 178.48,   17.8829,  200.451,  267.709,  181.56,   -16.8453 };
		const double angles13[] = { 214.578,   9.00573, 192.408,  229.888,  181.56,   -16.8453 };

		bool motionDone = false;

		// move to angles-00
		pos->positionMove(angles00);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-01
		pos->positionMove(angles01);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-03
		pos->positionMove(angles03);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-04
		pos->positionMove(angles04);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-05
		pos->positionMove(angles05);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-06
		pos->positionMove(angles06);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-07
		pos->positionMove(angles07);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-08
		pos->positionMove(angles08);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-09
		pos->positionMove(angles09);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-10
		pos->positionMove(angles10);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-11
		pos->positionMove(angles11);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-12
		pos->positionMove(angles12);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-13
		pos->positionMove(angles13);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-02
		pos->positionMove(angles02);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-01
		pos->positionMove(angles01);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

		// move to angles-00
		pos->positionMove(angles00);
		do {
			yarp::os::Time::delay(0.5);
			pos->checkMotionDone(&motionDone);
		} while (motionDone == false);

//		const double poss0[] = { 179.65,  54.25, 177.69, 178.49, 179.49, -123.2 };
//		const double poss1[] = { 352.64, -15.74, 302.69, 293.49, 350.49,    0.0 };
//		const double poss2[] = { 6.65, 124.25, 52.7, 63.5, 8.5, -123.2 };
//		const double poss3[] = { 179.65, 124.25, 120.0, 63.5, 8.5, -123.2 };
//		const double deltas1[] = { 180.0, 0.0, 60.0, 0.0, 0.0, 0.0 };
//		const double deltas2[] = { -180.0, 0.0, -60.0, 0.0, 0.0, 0.0 };
//		const double sp0[] = { 25.0, 30.0, 30.0, 30.0, 30.0, 30.0 };
//		const double sp1[] = { 75.0, 90.0, 90.0, 90.0, 90.0, 90.0 };
//		const double sp2[] = { 150.0, 180.0, 180.0, 180.0, 180.0, 180.0 };
//
//		bool motionDone = false;
//
//		pos->setRefSpeeds(sp2);
//
//		//		pos->positionMove(poss0);
//		//		pos->positionMove(poss1);
//		//		pos->positionMove(poss2);
//		//		do {
//		//			pos->checkMotionDone(&motionDone);
//		//		} while (motionDone == false);
//
//		for (int i = 0; i < 1; i++) {
//			pos->positionMove(poss3);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		if (pos->relativeMove(0, 180)) {
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(0, &motionDone);
//			} while (motionDone == false);
//			if (pos->relativeMove(0, -180)) {
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(0, &motionDone);
//				} while (motionDone == false);
//			} else {
//				pos->positionMove(poss2);
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(&motionDone);
//				} while (motionDone == false);
//			}
//		} else {
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		if (pos->relativeMove(1, -30)) {
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(1, &motionDone);
//			} while (motionDone == false);
//			if (pos->relativeMove(1, 30)) {
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(1, &motionDone);
//				} while (motionDone == false);
//			} else {
//				pos->positionMove(poss2);
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(&motionDone);
//				} while (motionDone == false);
//			}
//		} else {
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		if (pos->relativeMove(2, 120)) {
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(2, &motionDone);
//			} while (motionDone == false);
//			if (pos->relativeMove(2, -120)) {
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(2, &motionDone);
//				} while (motionDone == false);
//			} else {
//				pos->positionMove(poss2);
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(&motionDone);
//				} while (motionDone == false);
//			}
//		} else {
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		if (pos->relativeMove(3, 90)) {
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(3, &motionDone);
//			} while (motionDone == false);
//			if (pos->relativeMove(3, -90)) {
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(3, &motionDone);
//				} while (motionDone == false);
//			} else {
//				pos->positionMove(poss2);
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(&motionDone);
//				} while (motionDone == false);
//			}
//		} else {
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		if (pos->relativeMove(4, 200)) {
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(4, &motionDone);
//			} while (motionDone == false);
//			if (pos->relativeMove(4, -200)) {
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(4, &motionDone);
//				} while (motionDone == false);
//			} else {
//				pos->positionMove(poss2);
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(&motionDone);
//				} while (motionDone == false);
//			}
//		} else {
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		if (pos->relativeMove(5, 100)) {
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(5, &motionDone);
//			} while (motionDone == false);
//			if (pos->relativeMove(5, -100)) {
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(5, &motionDone);
//				} while (motionDone == false);
//			} else {
//				pos->positionMove(poss2);
//				do {
//					yarp::os::Time::delay(0.5);
//					pos->checkMotionDone(&motionDone);
//				} while (motionDone == false);
//			}
//		} else {
//			pos->positionMove(poss2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		for (int i = 0; i < 1; i++) {
//			yarp::os::Time::delay(0.5);
//			pos->relativeMove(deltas1);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//			pos->relativeMove(deltas2);
//			do {
//				yarp::os::Time::delay(0.5);
//				pos->checkMotionDone(&motionDone);
//			} while (motionDone == false);
//		}
//
//		pos->setRefSpeed(0, 240.0);
//		double sp;
//		pos->getRefSpeed(0, &sp);
//		std::cout << "ref speed 0: " << sp << std::endl;
//		pos->setRefAcceleration(0, 0.1);
//		double acc;
//		pos->getRefAcceleration(0, &acc);
//		std::cout << "ref acc 0: " << acc << std::endl;
//		pos->positionMove(0, 179.65);
//		do {
//			yarp::os::Time::delay(0.5);
//			pos->checkMotionDone(0, &motionDone);
//		} while (motionDone == false);
//		//		pos->stop();
//
//		pos->setRefSpeed(0, 127.0);
//		pos->setRefAcceleration(0, 10.0);
//		pos->positionMove(0, 352.64);
//		do {
//			yarp::os::Time::delay(0.5);
//			pos->checkMotionDone(0, &motionDone);
//		} while (motionDone == false);
//
//		pos->setRefAcceleration(0, 1.0);
//		pos->positionMove(0, 6.65);
//		do {
//			yarp::os::Time::delay(0.5);
//			pos->checkMotionDone(0, &motionDone);
//		} while (motionDone == false);
//
//		pos->positionMove(2, 177.69);
//		do {
//			yarp::os::Time::delay(0.5);
//			pos->checkMotionDone(2, &motionDone);
//		} while (motionDone == false);
//		pos->positionMove(2, 52.7);
//		do {
//			yarp::os::Time::delay(0.5);
//			pos->checkMotionDone(2, &motionDone);
//		} while (motionDone == false);
	} else {
		printf("  could not find IPositionControl interface\n");
	}
//
//	if (driver.view(vel)) {
//		int ct = 0;
//		pos->getAxes(&ct);
//		printf("  number of axes is: %d\n", ct);
//
//		const double sp1[] = { 5.0, -5.0, -5.0, 5.0, -5.0, 5.0 };
//		const double sp2[] = { -5.0, 5.0, 5.0, -5.0, 5.0, -5.0 };
//
//		vel->velocityMove(0, 35);
//		yarp::os::Time::delay(3.0);
//		vel->velocityMove(0, -35);
//		yarp::os::Time::delay(3.0);
//		vel->stop(0);
//
//		vel->velocityMove(sp1);
//		yarp::os::Time::delay(3.0);
//		vel->velocityMove(sp2);
//		yarp::os::Time::delay(3.0);
//		vel->stop();
//
//		printf("Test over\n");
//	} else {
//		printf("  could not find IVelocityControl interface\n");
//	}
}

int main(int argc, char *argv[]) {
	Drivers::factory().add(new DriverCreatorOf<Katana400> ("katana400",
			"controlboard", "Katana400"));

//		printf("============================================================\n");
//		printf("check our device can be instantiated directly\n");
//
//		Property directConfig;
//		if (!directConfig.fromConfigFile("katanaDirect.ini")) {
//			std::cout << "Configuration file \"katanaDirect.ini\" not found!!"
//					<< std::endl;
//			exit(1);
//		}
//		std::cout << directConfig.toString().c_str() << std::endl;
//		std::cout << std::endl;
//
//		PolyDriver direct(directConfig);
//		if (direct.isValid()) {
//			printf("Direct instantiation worked\n");
//			testMotor(direct);
//		} else {
//			printf("Direct instantiation failed\n");
//		}
//		direct.close();

//		// check our device can be wrapped in the controlboard network wrapper
//		printf("\n\n");
//		printf("============================================================\n");
//		printf("check our device can be wrapped in controlboard\n");
//
//		Property indirectConfig;
//		if (!indirectConfig.fromConfigFile("katanaIndirect.ini", true)) {
//			std::cout << "Configuration file \"katanaIndirect.ini\" not found!!"
//					<< std::endl;
//			exit(1);
//		}
//		std::cout << indirectConfig.toString().c_str() << std::endl;
//		std::cout << std::endl;
//
//		PolyDriver indirect(indirectConfig);
//		if (indirect.isValid()) {
//			printf("Indirect instantiation worked\n");
//			testMotor(indirect);
//		} else {
//			printf("Indirect instantiation failed\n");
//		}
//		indirect.close();
//
	// check our device can be wrapped in the controlboard network wrapper
	// and accessed remotely
	printf("\n\n");
	printf("============================================================\n");
	printf("check our device can be accessed via remote_controlboard\n");

	Property remoteServerConfig;
	if (!remoteServerConfig.fromConfigFile("katanaRemoteServer.ini")) {
		std::cout
				<< "Configuration file \"katanaRemoteServer.ini\" not found!!"
				<< std::endl;
		exit(1);
	}
	std::cout << remoteServerConfig.toString().c_str() << std::endl;
	std::cout << std::endl;

	PolyDriver server(remoteServerConfig);
	if (server.isValid()) {
		printf("Server instantiation worked\n");
		Property remoteClientConfig;
		remoteClientConfig.put("device", "remote_controlboard");
		remoteClientConfig.put("local", "/client");
		remoteClientConfig.put("remote", remoteServerConfig.find("name"));
		PolyDriver client(remoteClientConfig);
		if (client.isValid()) {
			printf("Client instantiation worked\n");

			printf("\n");
			printf("  yarp read /read\n");
			printf("  yarp connect /katana400/rpc:i /rpc tcp+log.in\n");
			printf("  yarp connect /katana400/command:i /command tcp+log.in\n");
			printf("  yarp connect /katana400/state:o /state\n");
			printf("\n");
			printf("Press a key\n");
			printf("\n");
			scanf("c");

			testMotor(client);

			printf("Test over\n");
			//			while (true) {
			//				;
			//			}
		} else {
			printf("Client instantiation failed\n");
		}
		client.close();
	}
	server.close();

	return 0;
}
