/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef SPHERE_H
#define SPHERE_H

#include "primitiveobject.h"

namespace RobotModel { class Sphere; }

/** \brief Represents a sphere using FreeSOLID and OpenGL
 *
 * This class creates a sphereof variable radius. The local coordinate system is located at the centroid of the sphere.
 */
class RobotModel::Sphere : public PrimitiveObject
{
	
public:
	
	Sphere( qreal radius, int lod = 3 );	//!< Creates the FreeSOLID object
	virtual ~Sphere();						//!< Destroys the FreeSOLID object

	void makeDisplayList();					//!< Makes an OpenGL display list by recursive subdivision of an icosahedron

private:
	
	int lod;								//!< Level of Detail (number of recursions to compute)
	qreal radius;							//!< Radius of the sphere

	void drawTriangle( float* v1, float* v2, float* v3 );			//!< Makes an OpenGL triangle with GL_TRIANGLES
	void subdivide( float* v1, float* v2, float* v3, int lod );		//!< Subdivides a triangle into 4 smaller ones by bisecting each side
	void normalize( float v[3] );									//!< Normalizes a 3D vector...  for portability QVector3D is not used
	
	static float X,Z;
	static GLfloat vdata[12][3];
	static GLint tindices[20][3];
};

#endif

/** @} */
