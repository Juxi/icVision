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
//#include "constants.h"
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


//TODO: Leo get the Qt Signal from publishState() instead of calling robot->observe()
void ObjectMover::update() // mutex is already set in Model::computePose
{
	if (hasAttachedObjects) {
		for (int i = 0; i<objectList.size(); i++) {
			//KinematicModel::RobotObservation robotObs =(*robots)[robotList[i]]->observe(); // marker observation
			//QMatrix4x4 objectPose = robotObs.markerConfiguration(markerList[i]) * rtList[i]; // transform
			//objectList[i]->setT(objectPose); // set
		}
	}
}


void ObjectMover::check( )
{
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

	// tranformation matrix T = O / M, with O = object rototranslation matrix, M = marker rototranslatio matrix.
	//KinematicModel::RobotObservation robotObs = robot->observe();
	//QMatrix4x4 transform = robotObs.markerConfiguration(markerIndex).inverted() * object->getT();

	// add the object
	objectList.append(object);
	markerList.append(markerIndex);
	robotList.append(robotIndex);
	//rtList.append(transform);

	//printf("ObjectMover: grabbing object \"%s\" with marker \"%s\" on robot \"%s\"\n", object->getName().toStdString().c_str(), robotObs.markerName(markerIndex).toStdString().c_str(), robot->getName().toStdString().c_str());

	hasAttachedObjects = objectList.size() > 0;
}