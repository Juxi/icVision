/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef OBJECTMOVER_H
#define OBJECTMOVER_H

#include <QThread>
#include <QString>
#include <QVector>
#include <QMutex>
#include <QVector3D>
#include <QMatrix4x4>

//#include "model.h"

namespace KinematicModel
{
	class Model;
	class Robot;
	class CompositeObject;
	class PrimitiveObject;
	class ObjectMover;
}
	
/** \brief Moves object with marker
 *
 *	Set an object to move with a marker, for example, when a robot is moving an object with its hand.
 */
class KinematicModel::ObjectMover
{
		
public:

	ObjectMover(KinematicModel::Model* m, QVector<KinematicModel::Robot*> *r);			//!< Nothing special to do here
	virtual ~ObjectMover();	//!< Nothing special to do here
	void check(); //!< Synchronizes lists of objects with world, removing objects that are not in the world
	void update();
	void grabObject( CompositeObject* object, Robot* robot, int markerIndex );		//!< Attach object to marker on robot

private:
		
	KinematicModel::Model* model;
	QVector<KinematicModel::Robot*>* robots;
	
	bool hasAttachedObjects;

	QVector<KinematicModel::CompositeObject*> objectList;
	QVector<int> markerList;
	QVector<int> robotList;
	QVector<QMatrix4x4> rtList;  //<! Offset between marker and object at grab

	bool keepRunning;
};
#endif
/** @} */