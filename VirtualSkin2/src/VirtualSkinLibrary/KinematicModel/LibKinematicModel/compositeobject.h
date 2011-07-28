/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef COMPOSITEOBJECT_H
#define COMPOSITEOBJECT_H

#include <QMutex>
#include "SOLID.h"
#include "transformable.h"
#include "primitiveobject.h"

namespace KinematicModel
{
	class Model;
	class CompositeObject;
	class PrimitiveObject;
	//enum ObjType {							// these correspond to the DT_ResponseType objects in KinematicModel::Model
	//				NO_TYPE = NULL,			// for objects not yet appended to World or Robot.tree
	//				obstacle,
	//				target,
	//				robot
	//};
	///struct Garbage {
	//	DT_ObjectHandle solidObject;
	//	GL_DisplayList	displayList;
	//};
}

/** \brief A physical object modeled as the union of n PrimitiveObjects.
 *
 * 
 */
class KinematicModel::CompositeObject : public Transformable
{
	
public:
	
    CompositeObject( DT_ResponseClass c, DT_RespTableHandle t = NULL );	//!< Simply initializes indices
    virtual ~CompositeObject();					//!< Deletes the primitives in the QVector first then this object
	
	//virtual Robot* robot() const { return NULL; }
	
	//virtual bool	isColliding();
	
	void			setName( const QString& aName ) { objectName = aName; }	//!< Set a human readable identifier for the physical object
	const QString&	getName() const { return objectName; }						//!< Return a human readable identifier for the physical object
	
	void				setResponseClass( DT_ResponseClass c ) { responseClass = c; }
	DT_ResponseClass	getResponseClass() const { return responseClass; }
	
	void append( PrimitiveObject* primitive );	//!< Append a PrimitiveObject to the CompositeObject
	bool remove( PrimitiveObject* primitive );	//!< Remove a PrimitiveObject from the CompositeObject and delete that primitive

	//int getNumPrimitives() { return primitives.size() }
	
	const QVector<PrimitiveObject*>& data() { return primitives; }
	PrimitiveObject* get( const QString& primitiveName ) const;		//!< Get a pointer to a PrimitiveObject by looking up its name
																	/**< If there is more than one primitive with the same name, the first one in the vector is returned **/
	
	//void setListPending();
	
	
	void updateSolid();
	void render();
	
	void setInModel( bool b ) { inModel = b; }
	bool isInModel() { return inModel; }
	
	void kill();									//!< Sets a flag that the Composite object should be deleted ASAP (in a thread safe way by Model)
	bool isDead() const { return deathWish; }
	
protected:
	
	QString				objectName;				//!< A human readable identifier for the object
	DT_ResponseClass	responseClass;
	bool				inModel;
	bool				deathWish;
	
	QVector<PrimitiveObject*> primitives;

	int numSpheres,					//!< Counts calls to newSphere( double r, const QVector3D& pos ) so that spheres can be named 'sph1', 'sph2', ect.
		numCylinders,				//!< Counts calls to newCylinder( double r, double h, const QVector3D& pos ) so that cylinders can be named 'cyl1', 'cyl2', ect.
		numBoxes;					//!< Counts calls to newBox( const QVector3D& size, const QVector3D& pos ) so that boxes can be named 'box1', 'box2', ect.
	
	//QMutex mutex;
	
	friend class Model;
};

#endif

/** @} */
