/*
 *  camprop.cpp
 *  camview
 *
 *  Created by Alexander Förster on 1/28/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "grabberprop.h"
#include <string>

// TODO: call update readable values in auto mode with a timer

GrabberProp::GrabberProp(Grabber* grabber, GrabberFeature feature,
		QWidget *parent) :
	QWidget(parent), grabber(grabber), feature(feature) {
	propname = new QLabel();
	onoff = new QCheckBox(tr("Enabled"));
	for (unsigned int i = 0; i < feature.values; i++) {
		valuebox[i] = new QDoubleSpinBox();
		valuebox[i]->setAlignment(Qt::AlignRight);
		valuebox[i]->setSingleStep(0.01);
		valuebox[i]->setDecimals(2);
		valuebox[i]->setEnabled(false);
		valuebox[i]->setReadOnly(true);
	}
	maxname = new QLabel();
	for (unsigned int i = 0; i < feature.values; i++)
		slider[i] = new QSlider(Qt::Vertical);
	minname = new QLabel();
	autoradio = new QRadioButton(tr("Auto"));
	manualradio = new QRadioButton(tr("Manual"));
	oneradio = new QRadioButton(tr("One Touch"));
	onebutton = new QPushButton(tr("One Touch"));

	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->addWidget(propname, 0, 0, 1, feature.values, Qt::AlignHCenter);
	gridLayout->addWidget(onoff, 1, 0, 1, feature.values);
	for (unsigned int i = 0; i < feature.values; i++)
		gridLayout->addWidget(valuebox[i], 2, i);
	gridLayout->addWidget(maxname, 3, 0, 1, feature.values, Qt::AlignHCenter);
	for (unsigned int i = 0; i < feature.values; i++)
		gridLayout->addWidget(slider[i], 4, i, 1, 1, Qt::AlignHCenter);
	gridLayout->addWidget(minname, 5, 0, 1, feature.values, Qt::AlignHCenter);
	gridLayout->addWidget(autoradio, 6, 0, 1, feature.values);
	gridLayout->addWidget(manualradio, 7, 0, 1, feature.values);
	gridLayout->addWidget(oneradio, 8, 0, 1, feature.values);
	gridLayout->addWidget(onebutton, 9, 0, 1, feature.values);
	setLayout(gridLayout);

	updateFeature();

QObject::connect(onoff,SIGNAL(stateChanged(int)),this,SLOT(changeOnoff(int)));
QObject::connect(slider[0],SIGNAL(valueChanged(int)),this,SLOT(changeValue0(int)));
if (feature.values>1)QObject::connect(slider[1],SIGNAL(valueChanged(int)),this,SLOT(changeValue1(int)));
if (feature.values>2)QObject::connect(slider[2],SIGNAL(valueChanged(int)),this,SLOT(changeValue2(int)));
QObject::connect(autoradio,SIGNAL(toggled(bool)),this,SLOT(changeModeAuto(bool)));
QObject::connect(manualradio,SIGNAL(toggled(bool)),this,SLOT(changeModeManual(bool)));
QObject::connect(oneradio,SIGNAL(toggled(bool)),this,SLOT(changeModeOne(bool)));
QObject::connect(onebutton,SIGNAL(clicked(bool)),this,SLOT(activateOne(bool)));
}

GrabberProp::~GrabberProp() {
}

void GrabberProp::activateOne(bool) {
	grabber->setMode(feature.id, GrabberFeature::One);
	updateFeature();
}

void GrabberProp::changeModeAuto(bool checked) {
	if (checked) {
		feature.mode = GrabberFeature::Auto;
		grabber->setMode(feature.id, feature.mode);
		updateFeature();
	};
}

void GrabberProp::changeModeManual(bool checked) {
	if (checked) {
		feature.mode = GrabberFeature::Manual;
		grabber->setMode(feature.id, feature.mode);
		updateFeature();
	};
}

void GrabberProp::changeModeOne(bool checked) {
	if (checked) {
		feature.mode = GrabberFeature::One;
		grabber->setMode(feature.id, GrabberFeature::Manual);
		updateFeature();
	};
}

void GrabberProp::changeValue0(int value) {
	printf("POINT 00a\n");
	printf("  value: %d\n", value);

	changeValue0((double) value / 100);
}

void GrabberProp::changeValue0(double value) {
	printf("POINT 00b\n");
	printf("  value: %f\n", value);

	feature.value0 = value;
	if (feature.id == GrabberFeature::WhiteBalance) {
		grabber->setRelativeValues(feature.id, feature.value0, feature.value1,
				feature.value2);
		grabber->getRelativeValues(feature.id, &feature.value0,
				&feature.value1, &feature.value2);
		printf("  actual values on grabber: (%f, %f, %f)\n", feature.value0,
				feature.value1, feature.value2);
		valuebox[0]->setValue(feature.value0);
		valuebox[1]->setValue(feature.value1);
	} else {
		grabber->setRelativeValue(feature.id, feature.value0);
		grabber->getRelativeValues(feature.id, &feature.value0,
				&feature.value1, &feature.value2);
		printf("  actual value on grabber: %f\n", feature.value0);
		valuebox[0]->setValue(feature.value0);
	}

	//	updateFeature();
}

void GrabberProp::changeValue1(int value) {
	printf("POINT 01a\n");
	printf("  value: %d\n", value);

	changeValue1((double) value / 100);
}

void GrabberProp::changeValue1(double value) {
	printf("POINT 01b\n");
	printf("  value: %f\n", value);

	feature.value1 = value;
	if (feature.id == GrabberFeature::WhiteBalance) {
		grabber->setRelativeValues(feature.id, feature.value0, feature.value1,
				feature.value2);
		grabber->getRelativeValues(feature.id, &feature.value0,
				&feature.value1, &feature.value2);
		printf("  actual values on grabber: (%f, %f, %f)\n", feature.value0,
				feature.value1, feature.value2);
		valuebox[0]->setValue(feature.value0);
		valuebox[1]->setValue(feature.value1);
	} else {
		grabber->setRelativeValue(feature.id, feature.value1);
		grabber->getRelativeValues(feature.id, &feature.value0,
				&feature.value1, &feature.value2);
		printf("  actual value on grabber: %f\n", feature.value1);
	}

	//	updateFeature();
}

void GrabberProp::changeValue2(int value) {
	printf("POINT 02a\n");
	printf("  value: %d\n", value);

	changeValue2((double) value / 100);
}

void GrabberProp::changeValue2(double value) {
	printf("POINT 02b\n");
	printf("  value: %f\n", value);

	feature.value2 = value;
	if (feature.id == GrabberFeature::WhiteBalance) {
		grabber->setRelativeValues(feature.id, feature.value0, feature.value1,
				feature.value2);
		grabber->getRelativeValues(feature.id, &feature.value0,
				&feature.value1, &feature.value2);
		printf("  actual values on grabber: (%f, %f, %f)\n", feature.value0,
				feature.value1, feature.value2);
		valuebox[0]->setValue(feature.value0);
		valuebox[1]->setValue(feature.value1);
	} else {
		grabber->setRelativeValue(feature.id, feature.value2);
		grabber->getRelativeValues(feature.id, &feature.value0,
				&feature.value1, &feature.value2);
		printf("  actual value on grabber: %f\n", feature.value2);
	}

	//	updateFeature();
}

void GrabberProp::changeOnoff(int state) {
	feature.on = (state == Qt::Checked);
	grabber->setPower(feature.id, feature.on ? GrabberFeature::ON
			: GrabberFeature::OFF);
	updateFeature();
}

void GrabberProp::updateFeature() {
	propname->setText(feature.getName().c_str());

	if (feature.present) {
		onoff->setEnabled(feature.switchable);
		if (feature.switchable) {
			onoff->setChecked(feature.on);
		} else {
			onoff->setChecked(true);
		};
		for (unsigned int i = 0; i < feature.values; i++) {
			//			valuebox[i]->setReadOnly(!feature.readable);
		}
		if (!feature.readable || !feature.on) {
			maxname->setText("");
			minname->setText("");
			for (unsigned int i = 0; i < feature.values; i++) {
				valuebox[i]->setValue(0.0);
				valuebox[i]->setRange(0.0, 1.0);
				valuebox[i]->setSingleStep(0.01);
				valuebox[i]->setEnabled(false);
				slider[i]->setValue(0);
				slider[i]->setMaximum(100);
				slider[i]->setMinimum(0);
				slider[i]->setEnabled(false);
			};
			autoradio->setEnabled(false);
			manualradio->setEnabled(false);
			oneradio->setEnabled(false);
			autoradio->setChecked(false);
			manualradio->setChecked(false);
			oneradio->setChecked(false);
			autoradio->setCheckable(false);
			manualradio->setCheckable(false);
			oneradio->setCheckable(false);
			onebutton->setEnabled(false);

		} else /* feature is on and readable */
		{
			printf("  ***** POINT 000\n");

			maxname->setText(QString::number(feature.max_value, 'f', 2));
			minname->setText(QString::number(feature.min_value, 'f', 2));
			autoradio->setCheckable(true);
			manualradio->setCheckable(true);
			oneradio->setCheckable(true);
			autoradio->setEnabled(feature.possible_modes & GrabberFeature::Auto);
			manualradio->setEnabled(feature.possible_modes
					& GrabberFeature::Manual);
			oneradio->setEnabled(feature.possible_modes & GrabberFeature::One);
			onebutton->setEnabled(feature.mode == GrabberFeature::One);
			switch (feature.mode) {
			case GrabberFeature::Auto:
				autoradio->setChecked(true);
				for (unsigned int i = 0; i < feature.values; i++) {
					valuebox[i]->setEnabled(false);
					valuebox[i]->setValue(0.0);
					valuebox[i]->setRange(0.0, 1.0);
					valuebox[i]->setSingleStep(0.01);
					slider[i]->setEnabled(false);
					slider[i]->setValue(0);
					slider[i]->setMaximum(100);
					slider[i]->setMinimum(0);
				}
				;
				break;
			case GrabberFeature::Manual:
				printf("  ***** POINT 001\n");
				grabber->getRelativeValues(feature.id, &feature.value0,
						&feature.value1, &feature.value2);
				printf("    values: (%f, %f, %f)\n", feature.value0, feature.value1, feature.value2);

				printf("  ***** POINT 002\n");

				manualradio->setChecked(true);

				printf("  ***** POINT 003\n");

				for (unsigned int i = 0; i < feature.values; i++) {
					printf("  ***** POINT 004 %d %d\n", feature.id, i);

					double v = (i == 0 ? feature.value0
							: (i == 1 ? feature.value1 : feature.value2));

					valuebox[i]->setRange(feature.min_value, feature.max_value);
					valuebox[i]->setSingleStep(0.01);
					valuebox[i]->setValue(v);
					//					valuebox[i]->setEnabled(i < feature.setable);

					slider[i]->setMaximum(feature.max_value * 100);
					slider[i]->setMinimum(feature.min_value * 100);
					slider[i]->setValue(v * 100);
					//					slider[i]->setEnabled(i < feature.setable);
				}
				;
				break;
			case GrabberFeature::One:

				grabber->getRelativeValues(feature.id, &feature.value0,
						&feature.value1, &feature.value2);

				oneradio->setChecked(true);
				for (unsigned int i = 0; i < feature.values; i++) {
					valuebox[i]->setRange(feature.min_value, feature.max_value);
					valuebox[i]->setSingleStep(0.01);
					slider[i]->setMaximum(feature.max_value * 100);
					slider[i]->setMinimum(feature.min_value * 100);
					unsigned int v = (i == 0 ? feature.value0
							: (i == 1 ? feature.value1 : feature.value2));
					slider[i]->setValue(v * 100);
					valuebox[i]->setValue(v);
					valuebox[i]->setEnabled(false);
					slider[i]->setEnabled(false);
				}
				;
				break;
			default:
				break;
			};
		};
	} else /* feature not present */
	{
		onoff->setEnabled(false);
		for (unsigned int i = 0; i < feature.values; i++)
			valuebox[i]->setEnabled(false);
		maxname->setEnabled(false);
		for (unsigned int i = 0; i < feature.values; i++)
			slider[i]->setEnabled(false);
		minname->setEnabled(false);
		autoradio->setEnabled(false);
		manualradio->setEnabled(false);
		oneradio->setEnabled(false);
		onebutton->setEnabled(false);
		for (unsigned int i = 0; i < feature.values; i++) {
			valuebox[i]->setValue(0.0);
			valuebox[i]->setRange(0.0, 0.0);
			valuebox[i]->setSingleStep(0.01);
			valuebox[i]->setValue(0.0);
			slider[i]->setValue(0);
			slider[i]->setMaximum(100);
			slider[i]->setMinimum(0);
		};
		maxname->setText("");
		minname->setText("");
	};
}
