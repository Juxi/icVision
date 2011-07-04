/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

//#include <QMutex>
#include <QMatrix4x4>
#include "SOLID.h"
#include "displaylist.h"
#include "compositeObject.h"

namespace RobotModel
{
	//class CompositeObject;
	class PrimitiveObject;
}

/*! \brief A physical object modeled as a geometric primitive
 *
 * Includes a SOLID object and associated OpenGL display list. Supported sub-types are Box, Cylinder and Sphere.
 */
class RobotModel::PrimitiveObject : public DisplayList
{
	
public:
	
	enum GeomType {
		SPHERE,
		CYLINDER,
		BOX
	};
	
	PrimitiveObject();				//!< Nothing special to do here
	virtual ~PrimitiveObject();		//!< Nothing special to do here
	
	DT_ObjectHandle getSolidHandle() { return solidObject; }	//!< Returns the handle to the solid object
	
	void setName( const QString& aName ) { name = aName; }		//!< Sets a human readable name for the primitive
	const QString& getName() { return name; }					//!< Returns the human readable name of the primitive

	void setParent( CompositeObject* object );					//!< Assigns the primitive to a CompositeObject
	CompositeObject* getParent() { return parent; }				//!< Returns the CompositeObject to which the primitive has been assigned
	
	void setL( const QMatrix4x4& txfr );						//!< Sets a transformation matrix that specifies the position/orientation of the primitive relative to the CompositeObject
	void update( const QMatrix4x4& txfr );						//!< Updates the position/orientation of this primitive relative to the world coordinate system (DisplayList.T)
																/**< This computation is done using the parent.T and L, and is necessary whenever the CompositeObject is moved */ 
	
	void setCartesianOrientation( const QVector3D& rot );						//!< Set the orientation of the CompositeObject absolutely with respect to the world coordinate system	by rotating about z, then y, then x
																				/**< See DisplMatrix.setCartesianOrientation( const QVector3D& ) */
	void cartesianRotate( const QVector3D& rot );								//!< Rotate the CompositeObject from its current orientation about z, then y, then x 
																				/**< See DisplMatrix.setCartesianRotation( const QVector3D& ) */
	void setAxisAngleOrientation( const QVector3D& axis, qreal angle );			//!< Set the orientation of the CompositeObject absolutely with respect to the world coordinate system, using axis-angle rotation
																				/**< See DisplMatrix.setAxisAngleOrientation( const QVector3D&, qreal ) */ 
	void axisAngleRotate( const QVector3D& axis, qreal angle );					//!< Rotate the CompositeObject from its current orientation using axis-angle rotation
																				/**< See DisplMatrix.axisAngleRotate( const QVector3D&, qreal ) */ 
	void setSpecialEulerOrientation( const QVector3D& axis, qreal angle = 0 );	//!< Rotate the CompositeObject from its current orientation using euler angles
																				/**< See DisplMatrix.setSpecialEulerOrientation( const QVector3D&, qreal ) */ 
	void setPosition( const QVector3D& trans );								
	//!< Set the position of the CompositeObject absolutely with respect to the world coordinate system
																				/**< See DisplMatrix.setPosition( const QVector3D& ) */ 
	void translate( const QVector3D& trans );									//!< Translate the CompositeObject form its current position
																				/**< See DisplMatrix.translate( const QVector3D& ) */ 
	void render();
	
	CompositeObject::ObjType getParentObjectType();		//!< BODY_PART, OBSTACLE, or TARGET
	GeomType getGeomType() { return geomType; }			//!< SPHERE, CYLINDER, or BOX
	
	void setShape( DT_ShapeHandle s ) { shape = s; }
	DT_ShapeHandle getShape() { return shape; }
	void setObject( DT_ObjectHandle o ) { solidObject = o; }
	DT_ObjectHandle getObject() { return solidObject; }
	
protected:
	
	QString				name;		//!< A human readable name for the primitive
	GeomType			geomType;
	CompositeObject*    parent;		//!< The parent of the primitive (for the XML parser)
	DisplMatrix			L;			//!< Transformation from the PrimitiveObjectCS to the ObjectCS
	
	DT_ShapeHandle		shape;		//!< The particular shape of the primitive
	DT_ObjectHandle		solidObject;	//!< A representation of the object in the Solid library
};

#endif

/** @} */
