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

#include "module_widget.h"

using namespace yarp::sig;
using namespace yarp::os;


// Constructor
Window::Window() {
	initWindow();
	icVisionModule = NULL;
}

Window::Window(QString &t_in, QString &v_in) {
	initWindow();
	icVisionModule = NULL;	

	// set title
	title = t_in + " - " + v_in;
    setWindowTitle(title);
}

Window::Window(QString &t_in, QString &v_in, CoreModule *m) {
	initWindow();
	icVisionModule = m;

	// set title
	title = t_in + " - " + v_in;
    setWindowTitle(title);
}

void Window::initWindow() {	
	timer = new QTimer(this);	
	// srand( time(NULL) );
		
	window_layout = new QVBoxLayout();
	
	lbl_ModuleCount = new QLabel("Modules connected: 0");
	lbl_ModuleCount->setMinimumHeight(25);
	lbl_ModuleCount->setMaximumHeight(25);
	
	wdg_ModuleList = new QWidget(this);
	lay_ModuleList = new QVBoxLayout(wdg_ModuleList);	
	wdg_ModuleList->setLayout(lay_ModuleList);
	
	btn_quit	= new QPushButton("Quit", this);

	// add the elements to the window
	// Separator
	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	window_layout->addWidget(line);
	
	window_layout->addWidget(wdg_ModuleList);
	
	line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	window_layout->addWidget(line);
	
	window_layout->addWidget(lbl_ModuleCount);
	
	window_layout->addWidget(btn_quit);
	
	setLayout(window_layout);
	move(100,0);
	resize(0, 0);
	show();
	
	timer->start();
//	updateTimer();
}

// Destructor
Window::~Window() {
	delete btn_quit;
		
	delete lbl_ModuleCount;
	delete wdg_ModuleList;
	delete lay_ModuleList;
	delete window_layout;
	
	delete timer;
}

void Window::setupSignalsAndSlots() {
	QObject::connect(btn_quit, SIGNAL(clicked()), 
					 qApp,	   SLOT(quit()));
	timer->setInterval(400);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateList()));		

}

//void Window::addModule() {
//    QPushButton* button = new QPushButton("test",this);
//	lay_ModuleList->addWidget(button);
//    button->show();
//}

void Window::updateList() {
	static unsigned int lastSize = 0;
	// do this smarter!
	if (lastSize == icVisionModule->listOfModules.size()) 
		return;
	
	// Delete all the existing buttons in the layout
	QLayoutItem *wItem;
	while( (wItem = wdg_ModuleList->layout()->takeAt(0)) != 0) {
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
		wdg_ModuleList->layout()->addWidget(mod);
	}	
//	QFrame* line = new QFrame(this);
//	line->setFrameShape(QFrame::HLine);
//	line->setFrameShadow(QFrame::Sunken);
//	
//	wdg_ModuleList->layout()->addWidget(line);
	
	lbl_ModuleCount->setText(QString("Modules connected: %1").arg(icVisionModule->listOfModules.size()));
	lastSize = icVisionModule->listOfModules.size();	
}

void Window::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape ||
		e->key() == 'q' || e->key() == 'Q') {
        icVisionModule->interruptModule();
		close();
    } else
        QWidget::keyPressEvent(e);
}
