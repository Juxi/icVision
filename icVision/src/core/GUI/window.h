// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.
// based on Nokia Corporation (qt-info@nokia.com) QT Example Toolkit

#ifndef _ICVISION_GUI_WINDOW_H
#define _ICVISION_GUI_WINDOW_H

#include <QWidget>
#include <QtGui>

#include "../core_module.h"
#include <iostream>
#include <fstream>

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
	void updateList();
	
protected:
    void keyPressEvent(QKeyEvent *event);
		
private:
	void initWindow();
	void showYarpImages();

	// Variables
private:
	CoreModule *icVisionModule;	

	QTimer *timer;
		
	
/******** GUI Variables **********/
private:
	QString title;

	QVBoxLayout *window_layout, *lay_ModuleList;
	QWidget *wdg_ModuleList;	

	QPushButton *btn_quit;
	QLabel *lbl_ModuleCount;
};

#endif
