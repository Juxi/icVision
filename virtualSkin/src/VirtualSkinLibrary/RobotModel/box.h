#ifndef BOX_H
#define BOX_H

#include "primitiveobject.h"

namespace RobotModel {

class Box : public PrimitiveObject
{
public:
    Box( const QVector3D& size );
    ~Box();

    virtual void makeDisplayList();

private:
    qreal x, y, z;
};
	
}

#endif
