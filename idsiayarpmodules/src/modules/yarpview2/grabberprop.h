/*
 *  camprop.h
 *  camview
 *
 *  Created by Alexander Förster on 1/28/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GRABBERPROP
#define GRABBERPROP

#include <QtGui>
#include "grabber.h"

class GrabberProp : public QWidget
{
	Q_OBJECT

public:
	GrabberProp(Grabber * grabber, GrabberFeature feature, QWidget *parent = 0);
	~GrabberProp();

public slots:
	void updateFeature();

private slots:
	void changeOnoff(int state);
	void changeValue0(int value);
	void changeValue0(double value);
	void changeValue1(int value);
	void changeValue1(double value);
	void changeValue2(int value);
	void changeValue2(double value);
	void changeModeAuto(bool checked);
	void changeModeManual(bool checked);
	void changeModeOne(bool checked);
	void activateOne(bool checked);

private:
	Grabber *grabber;
	GrabberFeature feature;
    QLabel *propname;
	QCheckBox *onoff;
	QDoubleSpinBox *valuebox[3];
    QLabel *maxname;
	QSlider *slider[3];
    QLabel *minname;
	QRadioButton *autoradio;
	QRadioButton *manualradio;
	QRadioButton *oneradio;
	QPushButton *onebutton;
};
#endif
