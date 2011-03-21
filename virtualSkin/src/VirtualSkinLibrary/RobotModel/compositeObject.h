/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <QMatrix4x4>
#include "displaylist.h"

namespace RobotModel
{
	class CompositeObject;
	class PrimitiveObject;
}

/** \brief Facilitates the modeling of a physical object as the union of two or more primitives.
 *
 * This class provides an
 */
class RobotModel::CompositeObject : public QVector<PrimitiveObject*>, public DisplayList
{
	
public:
	
    CompositeObject( const QString& aName = "unnamedObject" );							//!< Simply names the object and initializes indices
    virtual ~CompositeObject();															//!< Deletes the primitives in the QVector first then this object
	
	void setName( const QString& aName ) { objectName = aName; }						//!< Set a human readable identifier for the physical object
	const QString& getName() { return objectName; }										//!< Return a human readable identifier for the physical object
	
	PrimitiveObject* newSphere( double r, const QVector3D& pos = QVector3D(0,0,0) );				//!< Create a new Sphere and append it to the CompositeObject
	PrimitiveObject* newCylinder( double r, double h, const QVector3D& pos = QVector3D(0,0,0) );	//!< Create a new Cylinder and append it to the CompositeObject
	PrimitiveObject* newBox( const QVector3D& size, const QVector3D& pos = QVector3D(0,0,0) );		//!< Create a new Box and append it to the CompositeObject
	
	virtual void append( PrimitiveObject* primitiveObject );							//!< Append a PrimitiveObject to the CompositeObject
	PrimitiveObject* getPrimitiveByName( const QString& objectName ) const;				//!< Get a pointer to a PrimitiveObject by looking up its name
	
	virtual bool remove( PrimitiveObject* primitive );									//!< Remove a PrimitiveObject from the CompositeObject and delete that primitive
	
    void setT( const QMatrix4x4& txfr );												//!< Sets the rotation/translation of the CompositeObject 
																						/**< ...by supplying the 4x4 transformation matrix directly. NOTE: There
																						     is no check that the supplied matrix represents a rigidbody transformation.
																							 Non-rigidbody transformations have not been tested. Don't use them! */
	void setCartesianRotation( const QVector3D& rot );									//!< Set the object orientation as absolute angles about the x, y and z axes
																						/**< The CompositeObject is rotated as follows...
																						     Let [Rx], [Ry] and [Rz] be rotation matrices that rotate by some angles
																							 about the x, y and z axes respectively. Let [R]=[Rx][Ry][Rz]. Since the
																							 rotation is absolute, the object is first assumed to be in the reference 
																							 position where the local coordinate system is aligned with the world
																							 corrdinate system. From that position, each of its points P is transformed
																							 thus: P*=[R]P So the object is rotated first about z, then about y and 
																							 finally about x. */
	void cartesianRotate( const QVector3D& rot );										//!< Rotate the object about x, y, z from its current orientation
																						/**< The CompositeObject is rotated as follows...
																							 Let [Rx], [Ry] and [Rz] be rotation matrices that rotate by some angles
																							 about the x, y and z axes respectively. Let [R]=[Rx][Ry][Rz]. Since the
																							 rotation is relative, the object is transformed directly from its current
																							 position. Each of its points P is transformed thus: P*=[R]P So the object
																							 is rotated first about z, then about y and finally about x. */
	void setAxisAngleRotation( const QVector3D& axis, qreal angle );					//!< Set the object orientation as an absolute angle about a particular axis
																						/**< The CompositeObject is rotated as follows...
																							 Since the rotation is absolute, the object is first assumed to be in the
																							 reference position where the local coordinate system is aligned with the
																							 world corrdinate system. From that position, each of its points P is
																							 transformed thus: P*=[R]P, where [R] is the axis-angle rotation matrix
																							 implied by the parameters*/
	void axisAngleRotate( const QVector3D& axis, qreal angle );							//!< Rotate the object about a particular axis from its current orientation
																						/**< The CompositeObject is rotated as follows...
																							 Since the rotation is relative, each of its points P is transformed from
																							 its current possition thus: P*=[R]P, where [R] is the axis-angle rotation
																							 matrix implied by the parameters*/
	void specialRotate( const QVector3D& axis, qreal angle = 0 );						//!< Rotate the object from its current orientation such that its principal direction is aligned with axis
																						//TODO: change to euler angle rotate
	void setTranslation( const QVector3D& trans );										//!< Set the absolute position of the object 
	void translate( const QVector3D& trans );											//!< Set the position of the object relative to its current position
	
	void makeDisplayList();																//!< Creates an OpenGL display list for the object coordinate system
    virtual void render();																//!< Call the displaylist of the object and the display lists of its children
	
	void doNotCheckCollision( const CompositeObject* a ) const;							//!< Set FreeSOLID not to compute collisions between this object and the one passed in as a parameter
	void doNotCheckCollision( PrimitiveObject* a ) const;								//!< Set FreeSOLID not to compute collisions between this object and the PrimitiveObject passed in as a parameter
	
	virtual void update();																//!< 

protected:
	
	QString objectName;
	int numSpheres,
		numCylinders,
		numBoxes;
	
};

#endif

/** @} */
