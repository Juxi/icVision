#ifndef CYLINDER_H
#define CYLINDER_H

#include "primitiveobject.h"

class Cylinder : public PrimitiveObject
{
public:
    Cylinder( qreal radius, qreal height, int lod = 12);
    ~Cylinder();

    virtual void makeDisplayList();

private:
    qreal radius, height;
    int lod;
};

#endif // CYLINDER_H
