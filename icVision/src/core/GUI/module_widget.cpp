// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "module_widget.h"
#include "yarp/os/Network.h"
#include <iostream>


// allocate memory for the static QString holding the
// stylesheet definition
QString ModuleWidget::loadedStyleSheet;

// needed for stylesheet
void ModuleWidget::paintEvent(QPaintEvent *) {
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void ModuleWidget::loadStyleSheet() {
	QString prev = QDir::currentPath();
	int notGivingUp = 3;
	/* Let's use QFile and point to a resource... */
	QFile data("modules.css");
	do {
		if(data.open(QFile::ReadOnly)) {
			/* ...read file to a string. */
			QTextStream styleIn(&data);
			loadedStyleSheet = styleIn.readAll();
			data.close();
			std::cout << "Loaded style file: " << QDir::currentPath().toStdString() << "/";
			std::cout << data.fileName().toStdString() << std::endl;

		} else {
			QDir::setCurrent("styles");
			if(data.open(QFile::ReadOnly)) {
				/* ...read file to a string. */
				QTextStream styleIn(&data);
				loadedStyleSheet = styleIn.readAll();
				data.close();
				std::cout << "Loaded style file: " << QDir::currentPath().toStdString() << "/";
				std::cout << data.fileName().toStdString() << std::endl;
				
			} else {
				std::cout << "Could not load file: " << QDir::currentPath().toStdString() << "/";
				std::cout << data.fileName().toStdString() << std::endl;
				QDir::setCurrent("..");
			}
			QDir::setCurrent("..");			
		}
	}while(notGivingUp--);
	QDir::setCurrent(prev);	
	std::cout << "-----------" << std::endl;	
}
		
void ModuleWidget::init() {
	btn_Status = NULL;
	
	layout = new QHBoxLayout();
#ifdef USE_INDICATOR_TOO
	// indicator
	lbl_Indicator = new QLabel();
	lbl_Indicator->setMinimumSize(12, 12);
	lbl_Indicator->setMaximumSize(12, 12);	
	layout->addWidget(lbl_Indicator);
#endif

//	std::string s = "<b>" + modInfo->getName() + " </b>";
	lbl_Name = new QLabel(modInfo->getName().c_str());
	lbl_Name->setMinimumWidth(170);	
	layout->addWidget(lbl_Name);

//	lbl_StatusMsg = new QLabel();
//	lbl_StatusMsg->setStyleSheet("font-size: 10pt");
//	lbl_StatusMsg->setAlignment(Qt::AlignBottom);
//	lbl_StatusMsg->setMinimumSize(80, 15);
//	lbl_StatusMsg->setMaximumHeight(15);	
//	layout->addWidget(lbl_StatusMsg);	
//	layout->setMargin(2);
	
	this->setMouseTracking(true);
	this->setLayout(layout);
	this->setStatus(false);
	this->show();
}

void ModuleWidget::setStatus(bool b, const char* msg) {
	moduleStatus = b;	
	if( b ) {
		setStyleSheet("ModuleWidget { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
									"stop:0 #ada, "
									"stop:1 #aaa);}"
					  "ModuleWidget:hover {"
									"background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
									"stop:0 #ada, "
									"stop:1 #bbb);}"
					  );
#ifdef USE_INDICATOR_TOO		
		lbl_Indicator->setStyleSheet("background-color: green;");
#endif		
	} else {
		setStyleSheet("ModuleWidget {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
									"stop:0 #daa, "
									"stop:1 #aaa);}"
					  "ModuleWidget:hover {"
									"background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
									"stop:0 #daa, "
									"stop:1 #bbb);}"
					  );

#ifdef USE_INDICATOR_TOO
		lbl_Indicator->setStyleSheet("background-color: red;");
#endif
	}
	
}

void ModuleWidget::mousePressEvent(QMouseEvent *event){
//	if (event->button() == Qt::LeftButton) {
	//	// indicator ... maybe get rid of it?
	if(!btn_Status) {	
		btn_Status = new QPushButton();
		btn_Status->setMinimumWidth(45);
		btn_Status->setMaximumWidth(45);		
		layout->addWidget(btn_Status);		
	} else {
		int margin = 185;
#ifdef USE_INDICATOR_TOO		
		margin = 200;
#endif		
		if( event->posF().x() < margin) {
			// cleanup	
			layout->removeWidget(btn_Status);
			delete btn_Status;
			btn_Status = NULL;
		}		
		event->ignore();
		return;
	}


	if(moduleStatus) {
		btn_Status->setText("Stop");
		QObject::connect(btn_Status,	SIGNAL(clicked()), 
						 this,		SLOT(tryToStopTheModule()) );
		
	} else {
		btn_Status->setText("Start");
		QObject::connect(btn_Status,	SIGNAL(clicked()), 
						 this,		SLOT(tryToStartTheModule()) );		
	}

	std::cout << "clicked?! " <<std::endl;
}


void ModuleWidget::tryToStartTheModule() {
	btn_Status->setEnabled(false);
	// only if we are not started yet
	if(!moduleStatus) {
		// check if we have a core module
		if(modInfo->getID() == -1) {
			// now we need to start the module somehow ...
			// TODO
			
			// if(not successfull)
			{
				showErrorToolTip();
				btn_Status->setEnabled(true);			
				return;
			}			
		} else {
			// TODO
			// send a YARP RPC command to start...
			
			// if(not successfull)
			{
				showErrorToolTip();
				btn_Status->setEnabled(true);			
				return;
			}
		}
	}
	
	setStatus(true);
	// cleanup	
	layout->removeWidget(btn_Status);
	delete btn_Status;
	btn_Status = NULL;
}

void ModuleWidget::tryToStopTheModule() {
	btn_Status->setEnabled(false);	
	// only if we are started
	if(moduleStatus) {
		// TODO
		// send a YARP RPC command to stop...
		
		// if(not successfull)
		{
			showErrorToolTip();
			btn_Status->setEnabled(true);			
			return;
		}
	}
	
	setStatus(false);
	
	// cleanup
	layout->removeWidget(btn_Status);
	delete btn_Status;
	btn_Status = NULL;
}


void ModuleWidget::showErrorToolTip() {
	if(!moduleStatus) {
		QToolTip::showText( btn_Status->mapToGlobal( QPoint( 0, 0 ) ), "Could not start this module!!" );		
		QString s = QString("Could not start this module!! [%1]").arg(QDateTime::currentDateTime().toString());
		setToolTip(s.toStdString().c_str());
		btn_Status->setToolTip(s.toStdString().c_str());
	} else {
		QToolTip::showText( btn_Status->mapToGlobal( QPoint( 0, 0 ) ), "Could not stop this module!!" );		
		QString s = QString("Could not stop this module!! [%1]").arg(QDateTime::currentDateTime().toString());
		setToolTip(s.toStdString().c_str());
		btn_Status->setToolTip(s.toStdString().c_str());
	}		
}

//Dashboard::Dashboard() : iCubSimulator_accessible(false) {
//	// initialize all labels
//	status_yarp = new StatusWidget("YARP");
//	status_Robot = new StatusWidget("Robot");		// somehow sim or not?!
//	status_Cameras  = new StatusWidget("Cameras");
//	status_Connection  = new StatusWidget("Connection");
//	status_VisionModule = new StatusWidget("IDSIAVision");
//	
//	btn_connect = new QPushButton("Connect");	
//	btn_initialize = new QPushButton("Initialize");	
//	btn_initialize->setEnabled(false);
//	
//	// set the layout and add labels
//
//	vwidget = new QWidget();
//	vlayout = new QVBoxLayout();
//	vlayout->addWidget(btn_connect);
//	vlayout->addWidget(btn_initialize);	
//	vwidget->setLayout(vlayout);
//	
//	layout = new QVBoxLayout();
//	layout->addWidget(status_yarp);	
//	layout->addWidget(status_Robot);
//	layout->addWidget(status_Cameras);
//	layout->addWidget(status_Connection);
//	layout->addWidget(status_VisionModule);
//	layout->setMargin(0);
//	
//	setLayout(layout);
//	this->show();
//}

//void Dashboard::addWidget(QWidget* w) {
//	layout->addWidget(w);
//}
//

//void Dashboard::update() {
//	// YARP
//	status_yarp->setStatus(yarp::os::NetworkBase::checkNetwork());
//
//	// Robot
//	if(yarp::os::NetworkBase::exists("/icub/world")) {
//		robotName = "icub";
//		iCubSimulator_accessible = true;
//		status_Robot->setStatus(true, "iCub running");
//	} else if(yarp::os::NetworkBase::exists("/icubSim/world")) {
//		robotName = "icubSim";		
//		iCubSimulator_accessible = true;
//		status_Robot->setStatus(true, "iCubSim running");
//	} else {
//		robotName = "notfound";		
//		iCubSimulator_accessible = false;
//		status_Robot->setStatus(false);
//	}	
//	
//	// Cameras
//	std::string left_cam  = "/" + robotName + "/cam/left";
//	std::string right_cam = "/" + robotName + "/cam/right";	
//	status_Cameras->setStatus(
//			yarp::os::NetworkBase::exists(left_cam.c_str()) &&
//			yarp::os::NetworkBase::exists(right_cam.c_str())  );
//
//}
