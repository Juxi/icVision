/*
 *  featurewindow.h
 *  camview
 *
 *  Created by Alexander Förster on 7/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef GRABBERFEATUREWINDOW_ISDEF
#define GRABBERFEATUREWINDOW_ISDEF

#include <QtGui>
#include "grabber.h"

class AllGrabberFeatureWidget : public QWidget
	{
		Q_OBJECT
	public:
		AllGrabberFeatureWidget(Grabber* grabber, QWidget *parent = 0);
	};

class GrabberFeatureWindow : public QScrollArea
	{
		Q_OBJECT
	public:
		GrabberFeatureWindow(Grabber* grabber, QWidget *parent = 0);
	};

#endif
