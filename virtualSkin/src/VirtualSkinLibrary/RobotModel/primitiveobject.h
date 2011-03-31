/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include <QMutex>
#include <QMatrix4x4>
#include "SOLID/solid.h"
#include "displaylist.h"

namespace RobotModel
{
	class CompositeObject;
	class PrimitiveObject;
}

/*! \brief A physical object modeled as a geometric primitive
 *
 * Includes a FreeSOLID object and associated OpenGL display list. Supported sub-types are Box, Cylinder and Sphere.
 */
class RobotModel::PrimitiveObject : public DisplayList
{
	
public:
	
	PrimitiveObject();				//!< Nothing special to do here
	virtual ~PrimitiveObject();		//!< Nothing special to do here
	
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
	void setPosition( const QVector3D& trans );									//!< Set the position of the CompositeObject absolutely with respect to the world coordinate system
																				/**< See DisplMatrix.setPosition( const QVector3D& ) */ 
	void translate( const QVector3D& trans );									//!< Translate the CompositeObject form its current position
																				/**< See DisplMatrix.translate( const QVector3D& ) */ 
protected:
	
	QString				name;		//!< A human readable name for the primitive
	CompositeObject*    parent;		//!< The parent of the primitive (for the XML parser)
	DtShapeRef			shape;		//!< The particular shape of the primitive
	DisplMatrix			L;			//!< Transformation from the PrimitiveObjectCS to the ObjectCS
	QMutex				mutex;

	void doNotCollideWith( CompositeObject* object );	//!< Disables collision response between this primitive and the ones in the specified CompositeObject
};

#endif

/** @} */
