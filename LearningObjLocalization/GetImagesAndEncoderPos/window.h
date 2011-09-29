/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QtGui>
#include "iCub_controller.h"
#include "dashboard.h"
#include "gl_widget.h"
#include "cv_widget.h"

#include <iostream>
#include <fstream>

#define HEAD_JOINTS 6

class iCubController;

class Window : public QWidget {
    Q_OBJECT

public:
	Window();
	Window(QString &t_in, QString &v_in);
	Window(QString &t_in, QString &v_in, iCubController *iCubCtrl_in);	
    ~Window();	

	void setupSignalsAndSlots();
	
public slots:
	void getYarpStatus();
	void updateTimer();
	void toggleTimer();	
	void timerTimeout();

protected:
    void keyPressEvent(QKeyEvent *event);
		
private:
	// Methods
	void initWindow();
	void showYarpImages();
	void showEncoderPositions();
	void showRedBall3DPosition();

	void doTheBabbling();
	
//	void doSomethingWithImages()
	void writeCSV();
	
	void changeBallPos();
		
	// GUI 
	QString title;
	QWidget *central, *vision_widget, *second_widget;
    GLWidget *gl_widget;	
	CVWidget *CV_left, *CV_right;
	CVWidget *CV_left_dev, *CV_right_dev;
	QHBoxLayout *central_layout;
	QVBoxLayout *second_layout;
	QGridLayout *vision_layout;	
	QPushButton *btn_quit, *btn_get, *btn_timer;
	QLabel *lbl_CV_left, *lbl_CV_right;
	QLabel *lbl_State_Head, *lbl_State_Torso, *lbl_BallPosition;
	QLabel *lbl_ImageCounter;
	QLineEdit *txt_State_Head, *txt_State_Torso;
	QLineEdit *txt_BallPosition;
	QSlider *sld_UpdateInterval;
	
	QWidget *chkH_widget, *chkT_widget;
	QHBoxLayout *chkH_layout, *chkT_layout;	
	QCheckBox *chk_head[HEAD_JOINTS], *chk_torso[3];
	
	double headjnt_pos[HEAD_JOINTS], torsojnt_pos[3];
	
//	QFile *csvfile;
	std::ofstream csvfile;
	int imageIndex;
	
	Dashboard *dash;
	iCubController *iCubCtrl;
	
	QTimer *timer;
	
	std::string imgNameLeft, imgNameRight;
};

#endif
