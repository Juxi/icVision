// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Authors: $YOUR_NAME
 * email:   $YOUR_EMAIL
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

/**
 * @file demoThread.h
 * @brief this file contains the definition of an additional thread that does the job for the main module.
 */

#ifndef LOOKATLOCATION_H_
#define LOOKATLOCATION_H_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <yarp/math/Math.h>
#include <string>
#include <iostream>

YARP_DECLARE_DEVICES(icubmod)

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

class LookAtLocation : public yarp::os::Thread //RateThread
{

  PolyDriver *clientGaze;
  PolyDriver *head;

  IGazeControl *igaze;

  IPositionControl *ipos;
  IVelocityControl *ivel;
  IEncoders        *iencs;

  int state;
  int startup_context_id;

  yarp::sig::Vector setpoints;
  yarp::sig::Vector tmp;
  yarp::sig::Vector encoders;
  yarp::sig::Vector commands_position;
  yarp::sig::Vector commands_velocity;
  yarp::sig::Vector checkpos;
  yarp::sig::Vector fp;

  int joints;
  double velocityFactor;

  BufferedPort<Bottle> MotionPort;
  Bottle *target;

  std::string inputLookAt3DPortName;
  std::string check;

  double x, y, z;


public:
  /**
   * contructor.
   * @param threshold threshold for image filtering.
   */
  LookAtLocation();

  /**
   * destructor.
   */
  ~LookAtLocation() {}

  // and so forth... don't forget to document all your methods.
  bool threadInit();
  void threadRelease();
  void run();
  void onStop();
  //void printStatus();
  void storeInterestingPoint();
  bool connect(std::string port2connect);


};

#endif /* LOOKATLOCATION_H_ */
