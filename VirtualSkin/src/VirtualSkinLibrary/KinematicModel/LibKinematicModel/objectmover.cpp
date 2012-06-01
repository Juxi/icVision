/*
*  kinematicModel
*
*  Created by Leo Pape on 20120601.
*  Copyright 2010 __MyCompanyName__. All rights reserved.
*
*/

#include <QString>
#include "objectmover.h"
#include "model.h"
#include "constants.h"
#include "compositeobject.h"

using namespace KinematicModel;

ObjectMover::ObjectMover(KinematicModel::Model* m, QVector<KinematicModel::Robot*> *r)
{
	hasAttachedObjects = false;
	keepRunning = false;
	model = m;
	robots = r;
	printf("Object Mover module active.\n");

}
ObjectMover::~ObjectMover()
{
}


void ObjectMover::update() // is using the mutex from Model::computePose
{
	if (hasAttachedObjects) {
		for (int i = 0; i<objectList.size(); i++) {
			KinematicModel::RobotObservation robotObs =(*robots)[robotList[i]]->observe();
			QMatrix4x4 pose = robotObs.markerConfiguration(markerList[i]);
			pose = rtList[i] * (pose); // get current marker pose and add offset
			objectList[i]->setT(pose); // get current marker pose and add offset
		}
	}
}


void ObjectMover::check( )
{
	QVector<KinematicModel::CompositeObject*>::iterator i;
	for (int i = objectList.size()-1; i>=0; i--) {
		if (!model->getObject(objectList[i]->getName())) {
			printf("ObjectMover: detaching object \"%s\".\n", objectList[i]->getName().toStdString().c_str());
			objectList.remove(i);
			markerList.remove(i);
			rtList.remove(i);
			robotList.remove(i);
		}
	}
	hasAttachedObjects = objectList.size() > 0;
}


void ObjectMover::grabObject( CompositeObject* object, Robot* robot, int markerIndex ) {

	// check if object is already in list
	int objectIndex = objectList.indexOf(object);
	if (objectIndex >= 0) {
		objectList.remove(objectIndex);
		markerList.remove(objectIndex);
		rtList.remove(objectIndex);
		robotList.remove(objectIndex);
	}

	if (markerIndex < 0) {
		printf("ObjectMover: ungrab object \"%s\".\n", object->getName().toStdString().c_str());
		return;
	}

	int robotIndex = (*robots).indexOf(robot);

	KinematicModel::RobotObservation robotObs = robot->observe();
	QMatrix4x4 markerPose = robotObs.markerConfiguration(markerIndex);
	QMatrix4x4 objectPose = object->getT();
	QMatrix4x4 offset = objectPose - markerPose;
	
	QMatrix4x4 translation;
	
	translation(0,3) = offset(0,3);
	translation(1,3) = offset(1,3);
	translation(2,3) = offset(2,3);
	
	// add the object
	objectList.append(object);
	markerList.append(markerIndex);
	robotList.append(robotIndex);
	rtList.append(translation);

	printf("ObjectMover: grabbing object \"%s\" with marker \"%s\" on robot \"%s\"\n", object->getName().toStdString().c_str(), robotObs.markerName(markerIndex).toStdString().c_str(), robot->getName().toStdString().c_str());

	hasAttachedObjects = objectList.size() > 0;
}