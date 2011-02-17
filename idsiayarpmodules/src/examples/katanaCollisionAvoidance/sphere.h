#ifndef SPHERE_H
#define SPHERE_H

#include "primitiveobject.h"

class Sphere : public PrimitiveObject
{
public:
    Sphere( qreal radius,
            const QVector3D& pos,
            int lod = 0 );
    ~Sphere();

    void makeDisplayList();

private:
    int lod;
    qreal radius;

    void drawTriangle( float* v1, float* v2, float* v3 );
    void subdivide( float* v1, float* v2, float* v3, int lod );
    void normalize( float* v );
};

#endif // SPHERE_H
