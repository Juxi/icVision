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
	lbl_iCubSim_Status = new QLabel("<center> <b> iCubSim </b> </center>");
	lbl_iCubSim_Status->setMinimumSize(80, 55);
	lbl_iCubSim_Status->setMaximumHeight(55);	

	btn_connect = new QPushButton("Connect");	
	btn_initialize = new QPushButton("Initialize");	
	btn_initialize->setEnabled(false);

	lbl_Connection_Status = new QLabel("<center> Connection </center>");
	lbl_Connection_Status->setMinimumSize(80, 55);
	lbl_Connection_Status->setMaximumHeight(55);	
	lbl_Connection_Status->setText("<center> Connection <br/> (not established) </center>");
	lbl_Connection_Status->setStyleSheet("background-color: red;");
		
	// set the layout and add labels

	vwidget = new QWidget();
	vlayout = new QVBoxLayout();
	vlayout->addWidget(btn_connect);
	vlayout->addWidget(btn_initialize);	
	vwidget->setLayout(vlayout);
	
	layout = new QHBoxLayout();
	layout->addWidget(lbl_iCubSim_Status);
	layout->addWidget(vwidget);	
	layout->addWidget(lbl_Connection_Status);
	layout->setMargin(0);
	
	setLayout(layout);
	
	setMinimumHeight(50);
	setMaximumHeight(60);
}

Dashboard::~Dashboard() {
	delete btn_connect;
	delete btn_initialize;
	delete lbl_iCubSim_Status;
	delete vlayout;	
	delete vwidget;
	delete layout;	
}

/**
 *@brief     checks if the iCub simulator is running and updates the labels accordingly
 */
void Dashboard::checkiCubSimulator() {
//    if(system("yarp exists /icubSim/world") == 0) {
	if(yarp::os::NetworkBase::exists("/icubSim/world")) {
        iCubSimulator_accessible = true;
        lbl_iCubSim_Status->setText("<center> <b> iCubSim </b><br/> (running) </center>"); //<font color=green>running</b> </font>");
		lbl_iCubSim_Status->setStyleSheet("background-color: green;");
    } else {
        iCubSimulator_accessible = false;
        lbl_iCubSim_Status->setText("<center> <b> iCubSim </b><br/> (not found) </center>"); //<font color=red> <b>iCubSim not running</b> </font>");
		lbl_iCubSim_Status->setStyleSheet("background-color: red;");
    }
}

/**
 *@brief     checks if the iCub simulator is running and updates the labels accordingly
 */
void Dashboard::checkiCub() {
	if(yarp::os::NetworkBase::exists("/icubF/head/state:o")) {
        iCub_accessible = true;
        lbl_iCubSim_Status->setText("<center> <b> iCubF </b><br/> (running) </center>"); //<font color=green>running</b> </font>");
		lbl_iCubSim_Status->setStyleSheet("background-color: green;");
    } else {
        iCub_accessible = false;
        checkiCubSimulator();
    }
}

void Dashboard::update() {
	checkiCub();
}

void Dashboard::updateConnectionStatus(bool b) {
	if(b) {
		lbl_Connection_Status->setText("<center> Connection <br/> (established) </center>");
		lbl_Connection_Status->setStyleSheet("background-color: green;");
		btn_connect->setText("Connected!");
	}
	else {
		lbl_Connection_Status->setText("<center> Connection <br/> (not established) </center>");
		lbl_Connection_Status->setStyleSheet("background-color: red;");	
		btn_connect->setText("Connect");	
	}	
	btn_connect->setEnabled(!b);
	btn_initialize->setEnabled(b);		
}
