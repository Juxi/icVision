/*
 *  dashboard.cpp
 *  HelloQT
 *
 *  Created by Juxi Leitner on 3/16/11.
 *
 */

#include "dashboard.h"
#include "yarp/os/Network.h"
#include <iostream>

Dashboard::Dashboard() : iCubSimulator_accessible(false) {
	// initialize all labels
	status_yarp = new StatusWidget("YARP");
	status_Robot = new StatusWidget("Robot");		// somehow sim or not?!
	status_Cameras  = new StatusWidget("Cameras");
	status_Connection  = new StatusWidget("Connection");
	status_VisionModule = new StatusWidget("IDSIAVision");
	
	btn_connect = new QPushButton("Connect");	
	btn_initialize = new QPushButton("Initialize");	
	btn_initialize->setEnabled(false);
	
	// set the layout and add labels

	vwidget = new QWidget();
	vlayout = new QVBoxLayout();
	vlayout->addWidget(btn_connect);
	vlayout->addWidget(btn_initialize);	
	vwidget->setLayout(vlayout);
	
	layout = new QVBoxLayout();
	layout->addWidget(status_yarp);	
	layout->addWidget(status_Robot);
	layout->addWidget(status_Cameras);
	layout->addWidget(status_Connection);
	layout->addWidget(status_VisionModule);
	layout->setMargin(0);
	
	setLayout(layout);
	this->show();
}

Dashboard::~Dashboard() {
	delete btn_connect;
	delete btn_initialize;
	delete vlayout;	
	delete vwidget;
	delete layout;	
}

void Dashboard::addWidget(QWidget* w) {
	layout->addWidget(w);
}


/**
 *@brief     checks if the iCub simulator is running and updates the labels accordingly
 */
void Dashboard::checkiCubSimulator() {
//    if(system("yarp exists /icubSim/world") == 0) {
}

void Dashboard::update() {
	// YARP
	status_yarp->setStatus(yarp::os::NetworkBase::checkNetwork());

	// Robot
	if(yarp::os::NetworkBase::exists("/icub/world")) {
		robotName = "icub";
		iCubSimulator_accessible = true;
		status_Robot->setStatus(true, "iCub running");
	} else if(yarp::os::NetworkBase::exists("/icubSim/world")) {
		robotName = "icubSim";		
		iCubSimulator_accessible = true;
		status_Robot->setStatus(true, "iCubSim running");
	} else {
		robotName = "notfound";		
		iCubSimulator_accessible = false;
		status_Robot->setStatus(false);
	}	
	
	// Cameras
	std::string left_cam  = "/" + robotName + "/cam/left";
	std::string right_cam = "/" + robotName + "/cam/right";	
	status_Cameras->setStatus(
			yarp::os::NetworkBase::exists(left_cam.c_str()) &&
			yarp::os::NetworkBase::exists(right_cam.c_str())  );
	
//	...	
}

void Dashboard::updateConnectionStatus(bool b) {
	if(b) {
		status_Connection->setStatus(b, "Connected");
		btn_connect->setText("Connected!");
	}
	else {
		status_Connection->setStatus(b, "Disconnected");
		btn_connect->setText("Connect");	
	}	
	btn_connect->setEnabled(!b);
	btn_initialize->setEnabled(b);		
}
