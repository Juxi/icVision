// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Authors: David Vernon
 * email:   yuor_email
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
 * @file demoModule.cpp
 * @brief implementation of the demoModule methods following the RFModule standards.
 */

/*
 * Audit Trail
 * -----------
 *  .... if any
 */

#include "WorldMapping.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;


/*
 * Configure method. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * If you are migrating from the old Module, this is the
 * equivalent of the "open" method.
 */

bool WorldMapping::configure(yarp::os::ResourceFinder &rf) {
    /*
     * PLEASE remove useless comments when writing actual code. If needed then use Doxygen comments and tags.
     */


    /* Process all parameters from both command-line and .ini file */
    /* get the module name which will form the stem of all module port names */
    moduleName            = rf.check("name",
                           Value("WorldMappingModule"),
                           "module name (string)").asString();

    /*
    * before continuing, set the module name before getting any other parameters,
    * specifically the port names which are dependent on the module name
    */
    setName(moduleName.c_str());

    /* now, get the rest of the parameters */
    /*
    * get the robot name which will form the stem of the robot ports names
    * and append the specific part and device required
    */
    robotName             = rf.check("robot",
                           Value("icub"),
                           "Robot name (string)").asString();


    //Set-up camera ports, input and output
    cameraLeft = new CameraiCub(moduleName, "left");
    cameraRight = new CameraiCub(moduleName, "right");


    /* do all initialization here */
    /*
    * attach a port of the same name as the module (prefixed with a /) to the module
    * so that messages received from the port are redirected to the respond method
    */
    handlerPortName =  "/";
    handlerPortName += getName();         // use getName() rather than a literal

    if (!handlerPort.open(handlerPortName.c_str())) {
        cout << getName() << ": Unable to open port " << handlerPortName << endl;
        return false;
    }

    attach(handlerPort);                  // attach to port

    //Connect Port
    cameraLeft->connect("/icub/cam/left");
    cameraRight->connect("/icub/cam/right");

//    /* create the thread and pass pointers to the module parameters */
//    dThread = new demoThread(&thresholdValue);
//
//    /* now start the thread to do the work */
//    dThread->start(); // this calls threadInit() and it if returns true, it then calls run()

    return true ;     // let the RFModule know everything went well
                      // so that it will then run the module
}

bool WorldMapping::interruptModule() {
    handlerPort.interrupt();
    return true;
}

bool WorldMapping::close() {
    handlerPort.close();

    cameraLeft->close();
    cameraRight->close();
    /* stop the thread */
   // dThread->stop();

    return true;
}

bool WorldMapping::respond(const Bottle& command, Bottle& reply) {
  string helpMessage =  string(getName().c_str()) +
                        " commands are: \n" +
                        "help \n" +
                        "quit \n" +
                        "set thr <n> ... set the threshold \n" +
                        "(where <n> is an integer number) \n";

  reply.clear();

  if (command.get(0).asString()=="quit") {
       reply.addString("quitting");
       return false;
   }
   else if (command.get(0).asString()=="help") {
      cout << helpMessage;
      reply.addString("ok");
   }
   else if (command.get(0).asString()=="set") {
      if (command.get(1).asString()=="thr") {
 //        thresholdValue = command.get(2).asInt(); // set parameter value
         reply.addString("ok");
      }
   }
   return true;
}

/* Called periodically every getPeriod() seconds */
bool WorldMapping::updateModule() {

	isImageLeft = cameraLeft->getImageOnOutputPort();
	isImageRight = cameraRight->getImageOnOutputPort();

	if(isImageLeft && isImageRight){
		//do something
		cameraLeft->getFeaturesOnOutputPort(HARRIS);
		cameraLeft->getGaborDescriptorOnOutputPort();

		//TODO CHANGE IN CameraiCub.cpp "HARRIS" linea 48
		cameraRight->getFeaturesOnOutputPort(HARRIS);
		cameraRight->getGaborDescriptorOnOutputPort();

		vector<KeyPoint> keypointsLeft = cameraLeft->getKeypoints();
        vector<KeyPoint> keypointsRight = cameraRight->getKeypoints();

		Mat gaborDescrLeft = cameraLeft->getGaborDescriptors();
		Mat gaborDescrRight = cameraRight->getGaborDescriptors();

		Mat outImageLeft, outImageRight;
		namedWindow("Test della minchia left");
		namedWindow("Test della minchia right");
		cv::drawKeypoints(cameraLeft->getImage(), keypointsLeft, outImageLeft, CV_RGB(255,0,0));
		cv::drawKeypoints(cameraRight->getImage(), keypointsRight, outImageRight, CV_RGB(255,0,0));
		imshow("Test della minchia left", outImageLeft);
		imshow("Test della minchia right", outImageRight);
		cvWaitKey();

//		DMatch test;
//		test.queryIdx = indexKeypointleft;
//		test.trainIdx = indexKeypointright;
//		test.distance = similarita
		// matches.push_back(test);

		// matches = vector<DMatch>;
		// Mat image_right;
		//drawMatches(image_left, keypoints_left, image_right, keypoints_right, matches, resultImage,CV_RGB(255,0,0), CV_RGB(0,0,255));
		//namedWindow("nomedellafinestra");
		// imshow("nomefinestra", resulImage);
		// cvWaitKey();
	}

    return true;
}

double WorldMapping::getPeriod() {
    /* module periodicity (seconds), called implicitly by myModule */
    return 0.1;
}

