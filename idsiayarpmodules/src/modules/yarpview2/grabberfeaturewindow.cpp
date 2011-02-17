/*
 *  featurewindow.cpp
 *  camview
 *
 *  Created by Alexander Förster on 7/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "grabberfeaturewindow.h"
#include "grabberprop.h"

AllGrabberFeatureWidget::AllGrabberFeatureWidget(Grabber* grabber, QWidget *parent)
: QWidget(parent)
{
    QGridLayout *gridLayout = new QGridLayout;

    const vector<GrabberFeature> & features = grabber->getFeatures();

	for (unsigned int i=0;i < features.size();i++)
	{
		GrabberProp* grabberProp = new GrabberProp(grabber, features[i]);
		gridLayout->addWidget(grabberProp, 0, i);
	};
    setLayout(gridLayout);
	setWindowTitle(tr("Grabber Features"));
}

GrabberFeatureWindow::GrabberFeatureWindow(Grabber* grabber, QWidget *parent)
: QScrollArea(parent)
{	
		AllGrabberFeatureWidget* allfeatures = new AllGrabberFeatureWidget(grabber, this);
		setWidget(allfeatures);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setMinimumHeight(allfeatures->minimumSizeHint().height()+horizontalScrollBar()->sizeHint().height());
		setWidgetResizable ( true );
		setWindowTitle(tr("Grabber Features"));
}
