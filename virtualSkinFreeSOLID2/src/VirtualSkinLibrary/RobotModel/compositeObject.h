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

/** \brief A physical object modeled as the union of n PrimitiveObjects.
 *
 * 
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
	
	virtual void append( PrimitiveObject* primitiveObject );					//!< Append a PrimitiveObject to the CompositeObject
	PrimitiveObject* getPrimitiveByName( const QString& objectName ) const;		//!< Get a pointer to a PrimitiveObject by looking up its name
	virtual bool remove( PrimitiveObject* primitive );							//!< Remove a PrimitiveObject from the CompositeObject and delete that primitive
	
    void setT( const QMatrix4x4& txfr );												//!< Sets the rotation/translation of the CompositeObject 
																						/**< ...by supplying the 4x4 transformation matrix directly. This is used to replace the
																							 transformation with a new one each time forward kinematics are computed. NOTE: There
																							 is no check that the supplied matrix represents a rigid-body transformation.
																							 non-rigid-body transformations have not been tested. Don't use them! */
	virtual void setCartesianOrientation( const QVector3D& rot );						//!< Set the orientation of the CompositeObject absolutely with respect to the world coordinate system	by rotating about z, then y, then x
																						/**< See DisplMatrix.setCartesianOrientation( const QVector3D& ) */
	virtual void cartesianRotate( const QVector3D& rot );								//!< Rotate the CompositeObject from its current orientation about z, then y, then x 
																						/**< See DisplMatrix.setCartesianRotation( const QVector3D& ) */
	virtual void setAxisAngleOrientation( const QVector3D& axis, qreal angle );			//!< Set the orientation of the CompositeObject absolutely with respect to the world coordinate system, using axis-angle rotation
																						/**< See DisplMatrix.setAxisAngleOrientation( const QVector3D&, qreal ) */ 
	virtual void axisAngleRotate( const QVector3D& axis, qreal angle );					//!< Rotate the CompositeObject from its current orientation using axis-angle rotation
																						/**< See DisplMatrix.axisAngleRotate( const QVector3D&, qreal ) */ 
	virtual void setSpecialEulerOrientation( const QVector3D& axis, qreal angle = 0 );	//!< Rotate the CompositeObject from its current orientation using euler angles
																						/**< See DisplMatrix.setSpecialEulerOrientation( const QVector3D&, qreal ) */ 
	virtual void setPosition( const QVector3D& trans );									//!< Set the position of the CompositeObject absolutely with respect to the world coordinate system
																						/**< See DisplMatrix.setPosition( const QVector3D& ) */ 
	virtual void translate( const QVector3D& trans );									//!< Translate the CompositeObject form its current position
																						/**< See DisplMatrix.translate( const QVector3D& ) */ 

	void makeDisplayList();														//!< Creates an OpenGL display list for the coordinate system associated with the CompositeObject
    virtual void render();														//!< Call the DisplayList of the object and its children
	
	void doNotCheckCollision( const CompositeObject* a ) const;					//!< Set FreeSOLID not to compute collisions between this CompositeObject and the one passed in as a parameter
	void doNotCheckCollision( PrimitiveObject* a ) const;						//!< Set FreeSOLID not to compute collisions between this CompositeObject and the PrimitiveObject passed in as a parameter
	
	virtual void update();														//!< Update the positions/orientations of the primitives (PrimitiveObject) within this CompositeObject
																				/**< This is called whenever the position/orientation of the CompositeObject is changed */

protected:
	
	QString objectName;				//!< A human readable identifier for the object
	int numSpheres,					//!< Counts calls to newSphere( double r, const QVector3D& pos ) so that spheres can be named 'sph1', 'sph2', ect.
		numCylinders,				//!< Counts calls to newCylinder( double r, double h, const QVector3D& pos ) so that cylinders can be named 'cyl1', 'cyl2', ect.
		numBoxes;					//!< Counts calls to newBox( const QVector3D& size, const QVector3D& pos ) so that boxes can be named 'box1', 'box2', ect.
	
};

#endif

/** @} */
