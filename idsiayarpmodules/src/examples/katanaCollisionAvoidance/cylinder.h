#ifndef CYLINDER_H
#define CYLINDER_H

#include "primitiveobject.h"

class Cylinder : public PrimitiveObject
{
public:
    Cylinder( qreal radius,
              qreal height,
              const QVector3D& cylPos,
              const QVector3D& cylAxis,
              int lod = 0);
    ~Cylinder();

    virtual void makeDisplayList();

private:
    qreal radius, height;
    int lod;
};

#endif // CYLINDER_H
