//########################################################################################################//
//Aquila - An Open-Source GPU-Accelerated Toolkit for Cognitive and Neuro-Robotics Research               //
//                                                                                                        //
//Copyright (C) 2011    Martin Peniak(www.martinpeniak.com)                                               //
//                      Christopher Larcobme (christopher.larcombe@plymouth.ac.uk)                        //
//                                                                                                        //
//This program is free software: you can redistribute it and/or modify it under the terms of the GNU      //
//General Public License as published by the Free Software Foundation, either version 3 of the License,   //
//or (at your option) any later version.                                                                  //
//########################################################################################################//

#ifndef ICUB_CONTROLLER_H
#define ICUB_CONTROLLER_H

#include <yarp/os/all.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/all.h>
#include "window.h"
#include "partController.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

/**
*@brief    iCub control
*/
class iCubController : public QObject {
	Q_OBJECT
	
public:
	iCubController(); //Window *ptr_mw
	~iCubController();

signals:
	void connectionStatus(bool);
	
public slots:
	void toggleConnection();
	void initializeRobot();
	
private:	//vars
    //structures
    struct Camera {
        QString                             port_name;
        BufferedPort<ImageOf<PixelRgb> >    *port;
        bool                                initialized;
        ImageOf<PixelRgb>                   *last_image;
    };
	
	QString left_camera_port_name;
	QString right_camera_port_name;

	struct Part {
        QString                 port_name;
        Port					*port;			// Streaming Port
        bool                    initialized;
		PartController			*ctrl;
    };

	struct WorldObject {
        QString                 name;
		double x, y, z;	// last_position;
    };
	
public: // vars
    Camera *left_camera, *right_camera;
	Part *head, *torso;	
	

private: // methods		
	//pointers to objects
	//    Window *main_window;
	Network *yarp_network;
	bool simulation;

	void initCameras();
    void closeCameras();	
	
	void initRightCamera();
	void initLeftCamera();
	void closeRightCamera();
	void closeLeftCamera();
	
	void initHead();
	void closeHead();

	void initTorso();
	void closeTorso();

public:
	void getWorldObjectPosition(const char *name, double &x, double &y, double &z);
	void setWorldObjectPosition(const char *name, double x, double y, double z);
};

#endif 