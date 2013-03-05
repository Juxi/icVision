// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.
// based on Nokia Corporation (qt-info@nokia.com) QT Example Toolkit

#ifndef _ICVISION_GUI_WINDOW_H
#define _ICVISION_GUI_WINDOW_H

#include <QWidget>
#include <QtGui>

#include <iostream>
#include <fstream>

#include "../core_module.h"
#include "module_widget.h"

class Window : public QWidget {
    Q_OBJECT

public:
	Window();
	Window(QString &t_in, QString &v_in);
	Window(QString &t_in, QString &v_in, CoreModule *m);	
	~Window();
	
	// Methods
	void setupSignalsAndSlots();

public slots:
//	void addModule();
//	void toggleTimer();	
	void refreshClicked();
	
protected:
    void keyPressEvent(QKeyEvent *event);
		
private:
	void initWindow();
	void createCoreModulesList();

	void updateList();
	
	//?void showYarpImages();

	// Variables
private:
	CoreModule *icVisionModule;	
	QString loadedStyleSheet;	

	QTimer *timer;
		
	void loadStyleSheet();
	
/******** GUI Variables **********/
private:
	QString title;
	
	QVBoxLayout *window_layout, *lay_FilterModuleList;
	QWidget *wdg_FilterModuleList;	
	QLabel *lbl_FilterModuleCount;
	
	QVBoxLayout *lay_CoreModuleList;
	QWidget *wdg_CoreModuleList;	
	QLabel *lbl_CoreModuleCount;
	
	QWidget *wdg_BottomButtons;
	QPushButton *btn_refresh, *btn_quit;

	// icVision::Core module widgets
	ModuleWidget *coreModule_Localisation, *coreModule_HTTP;
	// TODO not yet impl
	ModuleWidget *coreModule_Disparity, *coreModule_Saliency;
};

#endif
