/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef PRIMITIVEOBJECT_H
#define PRIMITIVEOBJECT_H

#include <QMutex>
#include <QTime>
#include <qgl.h>
#include <QColor>

#include <SOLID.h>
#include "modelconstants.h"
#include "transformable.h"


namespace KinematicModel
{
	class CompositeObject;
	class PrimitiveObject;
	typedef unsigned int GL_DisplayList;
	enum GeomType {
					SPHERE = 0,
					CYLINDER = 1,
					BOX = 2
	};
	enum CollisionType {
					FREE = 0,
					COLLISION = 1,
					CONSTRAINT = 2,
					FIELD = 3
	};
}

/*! \brief A physical object modeled as a geometric primitive
 *
 * Includes a SOLID object and associated OpenGL display list. Supported sub-types are Box, Cylinder and Sphere.
 */
class KinematicModel::PrimitiveObject : public Transformable
{
	
public:

	PrimitiveObject();				//!< Nothing special to do here
	virtual ~PrimitiveObject();		//!< Nothing special to do here
	
	void setIdx( uint i ) { index = i; }
	uint idx() { return index; }
	
	void setName( const QString& aName ) { name = aName; }		//!< Sets a human readable name for the primitive
	QString getName() const;
	
	void setListPending( bool b );
	bool listIsPending() const  { return listPending; }
	
	bool setColliding(CollisionType t=COLLISION);				//!< Sets the time of last collision to now
	CollisionType isColliding();					//!< Whether or not this primitive is currently considered colliding
	
	//virtual DT_ObjectHandle solidify() = 0;	//!< Initialize object in SOLID
	//virtual GL_DisplayList	display() = 0;	//!< Make an openGL display list

	void updateSolid( const QMatrix4x4& M );
	void render();
	
	QString&		getName() { return name; }					//!< Returns the human readable name of the primitive
	GeomType		getGeomType() { return geomType; }			//!< SPHERE, CYLINDER, or BOX
	DT_ObjectHandle getSolidObjectHandle() { return solidObject; }
	GL_DisplayList	getDisplayList() { return displayList; }
	
	void setCollidingColor( QColor color );
	void setConstraintColor( QColor color );
	void setFreeColor( QColor color );
	
	void setCompositeObject( CompositeObject* o ) { parentObject = o; }
	CompositeObject* getCompositeObject() { return parentObject; }
	
	void display();  //!< Only call this from the openGL thread or pay the price !!!
	virtual GL_DisplayList makeDisplayList() = 0;
	
protected:
	
	uint				index;
	QString				name;			//!< A human readable name for the primitive
	CompositeObject*	parentObject;
	GeomType			geomType;		//!< Sphere, cylinder, or box
	
	DT_ShapeHandle		solidShape;		//!< The particular shape of the primitive
	DT_ObjectHandle		solidObject;	//!< A representation of the object in the Solid library
	//QVector<QTime*>		collisionTimer;	//!< The time stamp of the last collision event
	QVector<QPair<CollisionType, QTime*> > collisionTimers;

	GL_DisplayList		displayList;
	bool				listPending;
	GLfloat				collidingColor[4];
	GLfloat				constraintColor[4];
	GLfloat				freeColor[4];
	GLfloat				black[4];
	
	QMutex mutex;
};

#endif

/** @} */
