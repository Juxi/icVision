#ifndef CYLINDER_H
#define CYLINDER_H

#include "physobject.h"

class Cylinder : public PhysObject
{
public:
    Cylinder();
    Cylinder( const qreal& radius,
              const qreal& height,
              const QVector3D& cylPos,
              const QVector3D& cylAxis );
    ~Cylinder();

    virtual void makeDisplayList();

private:
    qreal radius, height;
};

#endif // CYLINDER_H
