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
 * @file demoModule.h
 * @brief this file contains the definition of the main module code (that is the module yarp implmentation using the RFModule class.
 */

#ifndef WORLDMAPPING_H_
#define WORLDMAPPING_H_


/**
 * @ingroup icub_module
 *
 * \defgroup icub_demo demoModule
 *
 * This is a simple example to illustrate a module that is compliant with iCub Software Standards
 *
 * Addressing:
 * - configuration
 * - graceful shut-down
 * - thread-based execution
 * - run-time user interaction
 * - documentation and coding standards
 *
 * Functionally, the module just converts an input image to a binary image based on the supplied threshold
 *
 * A complete tutorial for this example is available on the iCub wiki at
 * http://eris.liralab.it/wiki/Summary_of_iCub_Software_Development_Guidelines
 *
 *
 * \section lib_sec Libraries
 *
 * YARP.
 *
 * \section parameters_sec Parameters
 *
 * <b>Command-line Parameters</b>
 *
 * The following key-value pairs can be specified as command-line parameters by prefixing \c -- to the key
 * (e.g. \c --from file.ini. The value part can be changed to suit your needs; the default values are shown below.
 *
 * - \c from \c demo.ini \n
 *   specifies the configuration file
 *
 * - \c context \c demo/conf \n
 *   specifies the sub-path from \c $ICUB_ROOT/icub/app to the configuration file
 *
 * - \c name \c demo \n
 *   specifies the name of the module (used to form the stem of module port names)
 *
 * - \c robot \c icub \n
 *   specifies the name of the robot (used to form the root of robot port names)
 *
 *
 * <b>Configuration File Parameters </b>
 *
 * The following key-value pairs can be specified as parameters in the configuration file
 * (they can also be specified as command-line parameters if you so wish).
 * The value part can be changed to suit your needs; the default values are shown below.
 *
 * - \c myInputPort \c /demo/image:i \n
 *   specifies the input port name (this string will be prefixed by \c /myModule
 *   or whatever else is specifed by the name parameter
 *
 * - \c myOutputPort \c /demo/image:o \n
 *   specifies the output port name (this string will be prefixed by \c /myModule
 *   or whatever else is specifed by the name parameter
 *
 * - \c cameraConfig \c icubEyes.ini \n
 *   specifies the camera configuration file containing the intrinsic parameters of
 *   the left and right cameras
 *
 * - \c threshold \c 8 \n
 *   specifies the threshold value
 *
 *
 * \section portsa_sec Ports Accessed
 *
 * - None
 *
 * \section portsc_sec Ports Created
 *
 *  <b>Input ports</b>
 *
 *  - \c /demo \n
 *    This port is used to change the parameters of the module at run time or stop the module. \n
 *    The following commands are available
 *
 *    \c help \n
 *    \c quit \n
 *    \c set \c thr \c <n>   ... set the threshold for binary segmentation of the input RGB image
 *    (where \c <n> is an integer number)
 *
 *    Note that the name of this port mirrors whatever is provided by the \c --name parameter value
 *    The port is attached to the terminal so that you can type in commands and receive replies.
 *    The port can be used by other modules but also interactively by a user through the yarp rpc directive, viz.: \c yarp \c rpc \c /myModule
 *    This opens a connection from a terminal to the port and allows the user to then type in commands and receive replies.
 *
 *  - \c /demo/image:i \n
 *
 * <b>Output ports</b>
 *
 *  - \c /myModule \n
 *    see above
 *
 *  - \c /myModule/image:o \n
 *
 * <b>Port types </b>
 *
 * The functional specification only names the ports to be used to communicate with the module
 * but doesn't say anything about the data transmitted on the ports. This is defined by the following code.
 *
 * \c BufferedPort<ImageOf<PixelRgb> >   \c myInputPort; \n
 * \c BufferedPort<ImageOf<PixelRgb> >   \c myOutputPort;
 *
 * \section in_files_sec Input Data Files
 *
 * None
 *
 * \section out_data_sec Output Data Files
 *
 * None
 *
 * \section conf_file_sec Configuration Files
 *
 * \c myModule.ini  in \c $ICUB_ROOT/app/myModule/conf \n
 * \c icubEyes.ini  in \c $ICUB_ROOT/app/myModule/conf
 *
 * \section tested_os_sec Tested OS
 *
 * Windows
 *
 * \section example_sec Example Instantiation of the Module
 *
 * <tt>demo --name demo --context demo/conf --from demo.ini --robot icub</tt>
 *
 * \author your name
 *
 * Copyright (C) 2009 RobotCub Consortium\n
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.\n
 * This file can be edited at \c $ICUB_ROOT/main/src/examples/demoModule/include/iCub/myModule.h
 *
 */

//Get all OS and signal processing YARP classes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

#include <iCub/iKin/iKinFwd.h>
#include <opencv2/opencv.hpp>

#include "FeatureDetectionThread.h"
#include "CameraiCub.h"

class WorldMapping: public yarp::os::RFModule {

	std::string moduleName;
	std::string robotName;
    std::string handlerPortName;

	CameraiCub *cameraLeft, *cameraRight;   //Camera class declarations
    yarp::os::Port handlerPort;      //a port to handle messages

    /* pointer to a new thread to be created and started in configure() and stopped in close() */
    FeatureDetectionThread *fdThread;

    bool isImageLeft, isImageRight;

public:
	//WorldMapping();
	//virtual ~WorldMapping();

   /**
	 * document your methods too.
	 */
	bool configure(yarp::os::ResourceFinder &rf); // configure all the module parameters and return true if successful
	bool interruptModule();                       // interrupt, e.g., the ports
	bool close();                                 // close and shut down the module
	bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
	double getPeriod();
	bool updateModule();

};

#endif /* WORLDMAPPING_H_ */

//----- end-of-file --- ( next line intentionally left blank ) ------------------
