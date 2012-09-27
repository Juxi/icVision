/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef SPHERE_H
#define SPHERE_H

#include "primitiveobject.h"
#include "modelexception.h"

namespace KinematicModel { class Sphere; }

/** \brief Represents a sphere using SOLID and OpenGL
 *
 * This class creates a sphere of variable radius. The local coordinate system is located at the centroid of the sphere.
 */
class KinematicModel::Sphere : public PrimitiveObject
{
	
public:
	
	Sphere( qreal radius, int lod = 3 ) throw(KinematicModelException);	//!< Creates the SOLID object
	virtual ~Sphere();													//!< Destroys the SOLID object
	
	qreal r() { return radius; }
	//DT_ObjectHandle solidify() { return solidObject = DT_CreateObject(this, solidShape = DT_NewSphere(radius)); }
	
	GL_DisplayList makeDisplayList();											//!< Makes an OpenGL display list by recursive subdivision of an icosahedron

private:
	
	qreal radius;													//!< Radius of the sphere
	int lod;														//!< Level of Detail (number of recursions to compute)

	void drawTriangle( float* v1, float* v2, float* v3 );			//!< Makes an OpenGL triangle with GL_TRIANGLES
	void subdivide( float* v1, float* v2, float* v3, int lod );		//!< Subdivides a triangle into 4 smaller ones by bisecting each side
	void normalize( float v[3] );									//!< Normalizes a 3D vector...  for portability QVector3D is not used
	
	static float X,Z;
	static GLfloat vdata[12][3];
	static GLint tindices[20][3];
};

#endif

/** @} */
