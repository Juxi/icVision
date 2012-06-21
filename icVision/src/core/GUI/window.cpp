// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.
// based on the helloQt main_window.cpp from Nokia Corporation (Contact: qt-info@nokia.com)

#include <QtGui>
#include "window.h"

#include <yarp/sig/all.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>

#include <opencv/cv.h>

#include <iostream>	//for cout
#include <iomanip>	// for setw

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace yarp::sig;
using namespace yarp::os;


// Constructor
Window::Window() : timer(NULL) {
	initWindow();
	icVisionModule = NULL;
}

Window::Window(QString &t_in, QString &v_in) : timer(NULL) {
	initWindow();
	icVisionModule = NULL;	

	// set title
	title = t_in + " - " + v_in;
    setWindowTitle(title);
}

Window::Window(QString &t_in, QString &v_in, CoreModule *m) : timer(NULL) {
	initWindow();
	icVisionModule = m;

	// set title
	title = t_in + " - " + v_in;
    setWindowTitle(title);
}

void Window::initWindow() {	
	ModuleWidget::loadStyleSheet();
	qApp->setStyleSheet(ModuleWidget::loadedStyleSheet);		

	// todo change to something?
	// timer = new QTimer(this);
	
	// define layout of window
	window_layout = new QVBoxLayout();
	window_layout->setMargin(0);
	window_layout->setSpacing(0);	

	// create widget and layout of core modules (and populate it)
	createCoreModulesList();
	
	// widget and layout of list of filters
	wdg_FilterModuleList = new QWidget(this);
	lay_FilterModuleList = new QVBoxLayout(wdg_FilterModuleList);
	lay_FilterModuleList->setMargin(0);
	lay_FilterModuleList->setSpacing(0);	
	wdg_FilterModuleList->setLayout(lay_FilterModuleList);

	// filter module count label
	lbl_FilterModuleCount = new QLabel("Modules connected: 0");
	lbl_FilterModuleCount->setMinimumHeight(25);
	lbl_FilterModuleCount->setMaximumHeight(25);
		
	// bottom buttons
	btn_quit	= new QPushButton("Quit", this);
	btn_refresh	= new QPushButton("Refresh", this);	
	btn_quit->setMinimumHeight(37);
	btn_quit->setMaximumHeight(37);	
	btn_refresh->setMinimumHeight(37);
	btn_refresh->setMaximumHeight(37);	
//	btn_quit->setMinimumWidth(67);
//	btn_quit->setMaximumWidth(67);	
//	btn_refresh->setMinimumWidth(67);
//	btn_refresh->setMaximumWidth(67);	
	
	
	// widget
	wdg_BottomButtons = new QWidget(this);
	wdg_BottomButtons->setLayout(new QHBoxLayout());
	wdg_BottomButtons->layout()->addWidget(btn_refresh);
	wdg_BottomButtons->layout()->addWidget(btn_quit);

	// add the elements to the window
	QLabel *ql = new QLabel("icVision Core Modules");
	ql->setMinimumHeight(27);
	ql->setMaximumHeight(27);	
	window_layout->addWidget(ql);	

	window_layout->addWidget(wdg_CoreModuleList);	

	// first separator
	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	line->setMinimumHeight(10);
	window_layout->addWidget(line);
	
	// list of filters running
	ql = new QLabel("icVision Filter Modules");
	ql->setMinimumHeight(27);
	ql->setMaximumHeight(27);	
	window_layout->addWidget(ql);
	window_layout->addWidget(wdg_FilterModuleList);
	
	// second separator
	line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	line->setMinimumHeight(10);
	
	window_layout->addWidget(line);
	
	// count of filters running
	window_layout->addWidget(lbl_FilterModuleCount);
	

	// second separator
	line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	line->setMinimumHeight(10);	
	window_layout->addWidget(line);
	
	// bottom buttons
	window_layout->addWidget(wdg_BottomButtons);
	
	
	// set window overall settings and show it
	
	setLayout(window_layout);
	move(100,0);
	resize(sizeHint());
	
	show();
	
//	timer->start();
//	updateTimer();
}

void Window::createCoreModulesList() {
	// widget and layout of list of filters
	wdg_CoreModuleList = new QWidget(this);
	lay_CoreModuleList = new QVBoxLayout(wdg_CoreModuleList);	
	lay_CoreModuleList->setMargin(0);
	lay_CoreModuleList->setSpacing(0);	
	
	wdg_CoreModuleList->setLayout(lay_CoreModuleList);
	
	coreModule_HTTP = new ModuleWidget("HTTP Server");
	wdg_CoreModuleList->layout()->addWidget(coreModule_HTTP);
//	coreModule_HTTP->setEnabled(false);
//	coreModule_HTTP->setStatus(false);

	coreModule_Localisation = new ModuleWidget("Localisation 3D");
	wdg_CoreModuleList->layout()->addWidget(coreModule_Localisation);
//	coreModule_Localisation->setEnabled(false);
//	coreModule_Localisation->setStatus(true);	
	
	// to add
	coreModule_Disparity = new ModuleWidget("Disparity Map");
	wdg_CoreModuleList->layout()->addWidget(coreModule_Disparity);
	
	coreModule_Saliency = new ModuleWidget("Saliency Map");
	wdg_CoreModuleList->layout()->addWidget(coreModule_Saliency);
}

// Destructor
Window::~Window() {
	delete btn_quit;
	delete btn_refresh;
	delete wdg_BottomButtons;

	delete lbl_CoreModuleCount;
	delete lay_CoreModuleList;
	delete wdg_CoreModuleList;

	delete lbl_FilterModuleCount;
	delete lay_FilterModuleList;
	delete wdg_FilterModuleList;
	delete window_layout;
	
	delete timer;
}

void Window::setupSignalsAndSlots() {
	QObject::connect(btn_quit, SIGNAL(clicked()), 
					 qApp,	   SLOT(quit()));
	QObject::connect(btn_refresh,	SIGNAL(clicked()),
					 this,			SLOT(refreshClicked()));
	if(timer) timer->setInterval(400);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateList()));		

}

void Window::refreshClicked() {
	// this was for testing only
//	ModuleWidget::loadStyleSheet();
//	qApp->setStyleSheet(ModuleWidget::loadedStyleSheet);	
	updateList();
}
	
void Window::updateList() {
	static unsigned int lastSize = 0;
	
	if(icVisionModule) {
		// do this smarter!
		if (lastSize == icVisionModule->listOfModules.size()) 
			return;
		
		// Delete all the existing buttons in the layout
		QLayoutItem *wItem;
		while( (wItem = wdg_FilterModuleList->layout()->takeAt(0)) != 0) {
			delete wItem->widget();
			delete wItem;
		}
			
	//	lay_ModuleList = new QVBoxLayout(this);
	//	wdg_ModuleList->setLayout(lay_ModuleList);
		
		std::vector<ModuleInfo>::iterator itr;
		for ( itr = icVisionModule->listOfModules.begin(); itr != icVisionModule->listOfModules.end(); ++itr ) {
			std::cout << (*itr).toStdString() << std::endl;
			
	//		QLabel *lbl = new QLabel(this);
	//		lbl->setText((*itr).toStdString().c_str());
	//		wdg_ModuleList->layout()->addWidget(lbl);
			ModuleWidget *mod = new ModuleWidget(*itr);
			wdg_FilterModuleList->layout()->addWidget(mod);
		}	
	//	QFrame* line = new QFrame(this);
	//	line->setFrameShape(QFrame::HLine);
	//	line->setFrameShadow(QFrame::Sunken);
	//	
	//	wdg_ModuleList->layout()->addWidget(line);
		
		lbl_FilterModuleCount->setText(QString("Modules connected: %1").arg(icVisionModule->listOfModules.size()));
		lastSize = icVisionModule->listOfModules.size();	
	}
}

void Window::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape ||
		e->key() == 'q' || e->key() == 'Q') {
		if(icVisionModule) icVisionModule->interruptModule();
		close();
    } else
        QWidget::keyPressEvent(e);
}
