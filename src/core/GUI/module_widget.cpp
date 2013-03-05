// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "module_widget.h"
#include "yarp/os/Network.h"
#include <iostream>


// allocate memory for the static QString holding the
// stylesheet definition
//QString ModuleWidget::loadedStyleSheet;

bool ModuleWidget::checkStatus(bool alsoSetStatus) {
	std::string portName;
	portName = modInfo->getPortName();
	// check if port exists
	if( Network::exists(portName.c_str()) ) {
		std::cout << portName << " exists!" << std::endl;
		// TODO check status ...
		if(alsoSetStatus) setStatus(true);
		return true;
	}
	std::cout << portName << " does not exist!" << std::endl;		
	if(alsoSetStatus) setStatus(false);
	return false;
}

// needed for stylesheet
void ModuleWidget::paintEvent(QPaintEvent *) {
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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
//				return;
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