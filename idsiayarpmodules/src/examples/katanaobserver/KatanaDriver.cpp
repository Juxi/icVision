// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>

#include <string>
#include <iostream>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp;

int main(int argc, char *argv[]) 
{
    Network yarp;

    Property params;
    params.fromCommand(argc, argv);

    if (!params.check("robot"))
    {
        fprintf(stderr, "Please specify the YARP name of the Katana\n");
        fprintf(stderr, "--robot name (e.g. /katana400/arm)\n");
        return -1;
    }
    std::string robotName=params.find("robot").asString().c_str();
    std::string remotePorts=robotName;
//    std::string remotePorts="/";
//    remotePorts+=robotName;
//    remotePorts+="/right_arm";

    std::string localPorts="/test/client";

    Property options;
    options.put("device", "remote_controlboard");
    options.put("local", localPorts.c_str());   //local port names
    options.put("remote", remotePorts.c_str());         //where we connect to

    // create a device
    PolyDriver robotDevice(options);
    if (!robotDevice.isValid()) {
        printf("Device not available.  Here are the known devices:\n");
        printf("%s", Drivers::factory().toString().c_str());
        return 0;
    }

    IPositionControl *pos;
    IEncoders *encs;

    bool ok;
    ok = robotDevice.view(pos);
    ok = ok && robotDevice.view(encs);

    if (!ok) {
        printf("Problems acquiring interfaces\n");
        return 0;
    }

    int nj=0;
    pos->getAxes(&nj);

	const double angles00[] = {  10.1656, 122.355,   56.4904,  67.0156,  12.0156, -16.8664 };
	const double angles01[] = { 177.052,  124.167,   52.791,   63.6195, 182.235,  -16.8594 };
//	const double angles02[] = { 177.763,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
//	const double angles03[] = { 255.57,   68.8908,  147.302,  199.435,  181.56,   -16.8523 };
//	const double angles04[] = { 101.797,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
//	const double angles05[] = { 142.367,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
//	const double angles06[] = { 220.639,  68.8908,  147.302,  199.435,  181.56,   -16.8523 };
	const double angles02[] = { 177.763,  68.8908,  147.302,  220.435,  181.56,   -16.8523 };
	const double angles03[] = { 255.57,   68.8908,  147.302,  220.435,  181.56,   -16.8523 };
	const double angles04[] = { 101.797,  68.8908,  147.302,  220.435,  181.56,   -16.8523 };
	const double angles05[] = { 142.367,  68.8908,  147.302,  220.435,  181.56,   -16.8523 };
	const double angles06[] = { 220.639,  68.8908,  147.302,  220.435,  181.56,   -16.8523 };
	const double angles07[] = { 180.589,  47.1375,  151.668,  280.393,  181.56,   -16.8523 };
	const double angles08[] = { 248.504,  33.7155,  176.344,  286.813,  181.56,   -16.8523 };
	const double angles09[] = { 180.589,  88.7564,   89.6112, 263.328,  181.56,   -16.8523 };
	const double angles10[] = { 108.898,  33.7193,  176.336,  286.805,  181.56,   -16.8453 };
	const double angles11[] = { 141.847,   9.00573, 192.408,  229.888,  181.56,   -16.8453 };
	const double angles12[] = { 178.48,   17.8829,  200.451,  267.709,  181.56,   -16.8453 };
	const double angles13[] = { 214.578,   9.00573, 192.408,  229.888,  181.56,   -16.8453 };

	bool motionDone = false;

	char c;

//	// press a button to advance
//	std::cout << "Press 'c' to continue";
//	std::cin >> c;

	// move to angles-00
	pos->positionMove(angles00);
	do {
		yarp::os::Time::delay(0.5);
		pos->checkMotionDone(&motionDone);
	} while (motionDone == false);

//	// press a button to advance
//	std::cout << "Press 'c' to continue";
//	std::cin >> c;

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

    robotDevice.close();
    
    return 0;
}
