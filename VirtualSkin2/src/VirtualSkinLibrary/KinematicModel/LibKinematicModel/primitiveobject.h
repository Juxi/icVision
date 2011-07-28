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
	
	void setName( const QString& aName ) { name = aName; }		//!< Sets a human readable name for the primitive
	
	void setListPending( bool b );
	bool listIsPending() const  { return listPending; }
	
	void setColliding() { collisionTimer.restart(); }							//!< Sets the time of last collision to now
	bool isColliding() { return collisionTimer.elapsed() < COLLISION_TIMEOUT; }	//!< Whether or not this primitive is currently considered colliding
	
	//virtual DT_ObjectHandle solidify() = 0;	//!< Initialize object in SOLID
	//virtual GL_DisplayList	display() = 0;	//!< Make an openGL display list

	void updateSolid( const QMatrix4x4& M );
	void render();
	
	QString&		getName() { return name; }					//!< Returns the human readable name of the primitive
	GeomType		getGeomType() { return geomType; }			//!< SPHERE, CYLINDER, or BOX
	DT_ObjectHandle getSolidObjectHandle() { return solidObject; }
	GL_DisplayList	getDisplayList() { return displayList; }
	
	void setCollidingColor( GLfloat color[4] );
	void setFreeColor( GLfloat color[4] );
	
	void setCompositeObject( CompositeObject* o ) { parentObject = o; }
	CompositeObject* getCompositeObject() { return parentObject; }
	
	void display();  //!< Only call this from the openGL thread or pay the price !!!
	virtual GL_DisplayList makeDisplayList() = 0;
	
protected:
	
	QString				name;			//!< A human readable name for the primitive
	CompositeObject*	parentObject;
	GeomType			geomType;		//!< Sphere, cylinder, or box
	
	DT_ShapeHandle		solidShape;		//!< The particular shape of the primitive
	DT_ObjectHandle		solidObject;	//!< A representation of the object in the Solid library
	QTime				collisionTimer;	//!< The time stamp of the last collision event
	
	GL_DisplayList		displayList;
	bool				listPending;
	GLfloat				collidingColor[4];
	GLfloat				freeColor[4];
	GLfloat				black[4];
	
	QMutex mutex;
};

#endif

/** @} */
