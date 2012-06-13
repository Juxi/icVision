// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_GUI_MODULE_WIDGET_H
#define _ICVISION_GUI_MODULE_WIDGET_H

#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QtGui>
#include <QFile>
#include <QDir>
#include "../module_info.h"


// #define USE_INDICATOR_TOO

// class StatusWidget;

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
	Q_OBJECT
private:
	ModuleInfo *modInfo;
	bool moduleStatus;
		
	QHBoxLayout *layout;	
#ifdef USE_INDICATOR_TOO
	QLabel *lbl_Indicator;
#endif	
	QLabel *lbl_Name;
	QPushButton *btn_Status;

	void init();
	void showErrorToolTip();
	
public slots:
	void tryToStartTheModule();
	void tryToStopTheModule();	
	
public:
	static QString loadedStyleSheet;	
	static void loadStyleSheet();	
	
	ModuleWidget() { }
	ModuleWidget(ModuleInfo &mod) {	
		this->modInfo = &mod;
		init();
	}
	ModuleWidget(ModuleInfo *mod) {
		this->modInfo = mod;
		init();
	}
	ModuleWidget(const char* name) {
		this->modInfo = new ModuleInfo();
		modInfo->set(name);
		init();
	}
	~ModuleWidget() {
		delete layout;
	}

	void setStatus(bool b, const char* msg = NULL);

	// for drawing and stylesheeting
	void paintEvent(QPaintEvent *);
	
protected:
	// for clicks, qwidget does not have signals
	void mousePressEvent(QMouseEvent *event);
//	void mouseMoveEvent(QMouseEvent *event);
//	void mouseReleaseEvent(QMouseEvent *event);
//	void resizeEvent(QResizeEvent *event);	
};




#endif 