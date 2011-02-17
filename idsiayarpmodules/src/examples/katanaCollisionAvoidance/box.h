#ifndef BOX_H
#define BOX_H

#include "primitiveobject.h"

class Box : public PrimitiveObject
{
public:
    Box( qreal x,
         qreal y,
         qreal z,
         const QVector3D& pos,
         const QVector3D& axis,
         qreal angle );
    ~Box();

    virtual void makeDisplayList();

private:
    qreal x, y, z;
};

#endif // BOX_H
