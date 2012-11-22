/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef DISPLMATRIX_H
#define DISPLMATRIX_H

#include <QMatrix4x4>
#include <math.h>

namespace KinematicModel { class Transformable; }

/** \brief Represents a 4x4 rigid-body transformation matrix
 *
 * This class wraps QMatrix4x4, providing some functions to encode rigid-body transformations in the QMatrix4x4.
 */
class KinematicModel::Transformable
{
	
public:
	Transformable() {};
	~Transformable() {};

	void setPosition( const QVector3D& pos );							//!< Set the absolute position of the object 
	void translate( const QVector3D& trans );							//!< Set the position of the object relative to its current position
	
	void setCartesianOrientation( const QVector3D& rot );				//!< Replace the rotational part of the transformation with a new 3x3 matrix that rotates about the z, then y, then x axes
																		/**< The new 3x3 rotation matrix is built as follows...
																			 Let [Rx], [Ry] and [Rz] be 3x3 rotation matrices that rotate by some angles
																			 about the x, y and z axes respectively. Letting [R]=[Rx][Ry][Rz], a point P
																			 (a 1x3 column vector) thus transformed: P*=[R]P, is rotated first about z, 
																			 then about y and finally about x. */
	void cartesianRotate( const QVector3D& rot );						//!< Concatenate the current transformation and a new rotational transformation that rotates about the z, then y, then x axes
																		/**< The new transformation is built as follows...
																			 Let [Rx], [Ry] and [Rz] be 4x4 rotation matrices that rotate (in 3D only) by some angles
																			 about the x, y and z axes respectively. Let [T*] be the new transformation and let [T]
																			 be the current transformation. Then [T*]=[Rx][Ry][Rz][T], which has the effect that a point
																			 P (a 1x4 column vector representing a point in 3D), transformed by [T*] is first transformed
																			 by the old matrix [T] and then by the new rotation [Rx][Ry][Rz]. */
	void setAxisAngleOrientation( const QVector3D& axis, qreal angle );	//!< Replace the rotational part of the transformation with a new 3x3 matrix that rotates a particular angle about a particular axis
	void axisAngleRotate( const QVector3D& axis, qreal angle );			//!< Concatenate the current transformation and a new rotational transformation that rotates a particular angle about a particular axis
																		/**< The concatenation is done as follows: Let [R] be the new axis-angle rotation, let [T*]
																			 be the new transformation resultant of concatenation, and let [T] be the old transformation.
																			 Then, [T*]=[R][T], which has the effect that a point P (a 1x4 column vector representing a point
																			 in 3D), transformed by [T*] is first transformed by the old matrix [T] and then by the new rotation [R]. */
	
	void setSpecialEulerOrientation( const QVector3D& axis, qreal angle = 0 );	//!< Concatenate the current transformation and a new rotational transformation that

	//void operator =( const QMatrix4x4& m ) { QMatrix4x4::operator=(m); }
	//void operator *( const QMatrix4x4& m ) { QMatrix4x4::operator*(m); }
    
    QVector3D getPos() { return QVector3D(T(0,3),T(1,3),T(2,3)); }
	
	QMatrix4x4 getT() { return T; }
	void setT( const QMatrix4x4& _T ) { T = _T; }
	
	//virtual void updateChildren() {} //!< In case changing T has consequences elsewhere
	
protected:
	
	QMatrix4x4 T;
	
};

#endif

/** @} */
