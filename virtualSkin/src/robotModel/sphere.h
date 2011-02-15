#ifndef SPHERE_H
#define SPHERE_H

#include "primitiveobject.h"

namespace RobotModel {

class Sphere : public PrimitiveObject
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
    void normalize( float* v );
	
	static float X,Z;
	static GLfloat vdata[12][3];
    static GLint tindices[20][3];
};
	
}

#endif
