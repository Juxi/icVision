// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_GUI_MODULE_WIDGET_H
#define _ICVISION_GUI_MODULE_WIDGET_H

#include <QtGui>
#include "../module_info.h"

class StatusWidget;

///**
// *@brief    Dashboard
// */
//class ModuleWidget : public QWidget {
//	Q_OBJECT
//
//public:
//	// Constructor and Destructor
//	ModuleWidget();
//	~ModuleWidget();	
//	
//	// Methods
//	void update();
//	void addWidget(QWidget* w);	
//
//	QPushButton *btn_connect;	
//	QPushButton *btn_initialize;
//	
//private:
//	// GUI
//	QWidget *vwidget;
//	QVBoxLayout *layout;
//	QVBoxLayout *vlayout;
//	
//	StatusWidget *status_yarp;
//	StatusWidget *status_Robot;	
//	StatusWidget *status_Cameras;	
//	StatusWidget *status_Connection;
//	StatusWidget *status_VisionModule;	
//
//	
//protected:
//	// Methods
//	void checkiCubSimulator();
//
//	
//};

class ModuleWidget : public QWidget {
private:
	ModuleInfo *modInfo;
	
	QHBoxLayout *layout;	
	QLabel *lbl_Indicator;
	QLabel *lbl_Name;
	QLabel *lbl_StatusMsg;	
	
	void init() {
		layout = new QHBoxLayout();
		
		lbl_Indicator = new QLabel();
		lbl_Indicator->setMinimumSize(15, 15);
		lbl_Indicator->setMaximumSize(15, 15);	
		lbl_Indicator->setStyleSheet("background-color: red;");
		
		std::string s = "<b>" + modInfo->getName() + " </b>";
		lbl_Name = new QLabel(s.c_str());
		lbl_Name->setMinimumSize(80, 15);
		lbl_Name->setMaximumHeight(15);	
		
		lbl_StatusMsg = new QLabel();
		lbl_StatusMsg->setStyleSheet("font-size: 10pt");
		lbl_StatusMsg->setAlignment(Qt::AlignBottom);
		lbl_StatusMsg->setMinimumSize(80, 15);
		lbl_StatusMsg->setMaximumHeight(15);	
		
		layout->addWidget(lbl_Indicator);
		layout->addWidget(lbl_Name);	
		layout->addWidget(lbl_StatusMsg);	
		
		layout->setMargin(2);
		
		this->setLayout(layout);
		this->show();
	}
	
public:
	ModuleWidget() {}
	ModuleWidget(ModuleInfo &mod) {	
		this->modInfo = &mod;
		init();
	}
	ModuleWidget(ModuleInfo *mod) {
		this->modInfo = mod;
		init();
	}
	~ModuleWidget() {
		delete layout;
	}
	
	void setStatus(bool b, const char* msg = NULL);
};

#endif 