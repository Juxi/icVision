/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef WORLD_H
#define WORLD_H

#include <QThread>
#include <QMutex>
#include <QVector>

#include "renderList.h"
#include "compositeObject.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

namespace RobotModel { class World; }

class RobotModel::World : public RenderList
{

public:
	
	World();
	~World();
	
	CompositeObject* newSphere( double r, const QVector3D& pos );
	CompositeObject* newSSphere( double r, const QVector3D& pos );
	CompositeObject* newCylinder( double r, double h, const QVector3D& pos );
	CompositeObject* newSCylinder( double r, double h, const QVector3D& pos );
	CompositeObject* newBox( const QVector3D& size, const QVector3D& pos );
	CompositeObject* newSBox( const QVector3D& size, const QVector3D& pos );
	CompositeObject* newObject( const QString& name, const QVector3D& pos );
	
	CompositeObject* append( CompositeObject* obj );
	CompositeObject* getObjectByName( const QString& name );
	
	bool remove( CompositeObject* object );
	bool removePrimitive( CompositeObject* object, PrimitiveObject* primitive );
	
	void clear();

	void filterCollisions( const CompositeObject* );
	void filterCollisions( PrimitiveObject* );
	
	void render();
	void notColliding();

private:

	QVector<CompositeObject*> objectList;
	int numSpheres, numCylinders, numBoxes,
		numSSpheres,numSCylinders,numSBoxes;
	QMutex mutex;
};

#endif

/** @} */
