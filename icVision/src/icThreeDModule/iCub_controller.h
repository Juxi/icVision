// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v3.0.
// based on the iCub Controller in:
//		Aquila - An Open-Source GPU-Accelerated Toolkit for
//		Cognitive and Neuro-Robotics Research               
//      Copyright (C) 2011, Martin Peniak(www.martinpeniak.com)
//						and Christopher Larcobme (christopher.larcombe@plymouth.ac.uk)

#ifndef ICUB_CONTROLLER_H
#define ICUB_CONTROLLER_H

#include <string>
#include <yarp/os/all.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/all.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

/**
*@brief    iCub controller
*/
class iCubController {
	
public:
	iCubController(); //Window *ptr_mw
	iCubController(bool isSim);
	~iCubController();
	
	void initializeRobot();

//?	void initializeRobot();
	
private:
    //structures
	struct Part {
		std::string				port_name;
        /*BufferedPort<Bottle>*/Port 	*port;			// Streaming Port
        bool                    initialized;
    };

	struct WorldObject {
		std::string             name;
		double x, y, z;	// last_position;
    };
	
public:
	// vars
	Part *head;
	Part *torso;	
	bool simulation;	
	

private:
	//pointers to objects
	Network *yarp_network;

	// methods		
	void initHead();
	void closeHead();
	void initTorso();
	void closeTorso();
};

#endif 