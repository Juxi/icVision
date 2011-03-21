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

class RobotModel::Sphere : public PrimitiveObject
{
	
public:
	
	Sphere( qreal radius, int lod = 3 );
	virtual ~Sphere();

	void makeDisplayList();

private:
	
	int lod;
	qreal radius;

	void drawTriangle( float* v1, float* v2, float* v3 );
	void subdivide( float* v1, float* v2, float* v3, int lod );
	void normalize( float v[3] );
	
	static float X,Z;
	static GLfloat vdata[12][3];
	static GLint tindices[20][3];
};

#endif

/** @} */
