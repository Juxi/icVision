#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include <QtGui>
#include "SOLID/solid.h"
#include "displaylist.h"

class Object;

class PrimitiveObject : public DisplayList
{
public:
    PrimitiveObject( Object* parent = 0 );
    virtual ~PrimitiveObject();

    //virtual void makeDisplayList() = 0;

    // set stuff
    void setParent( Object* object );
    void setT();
    void setT( const QMatrix4x4& txfr );
    void setL( const QMatrix4x4& txfr );

protected:
    Object*     parent;
    QMatrix4x4  L;      // transformation from the PrimitiveObjectCS to the ObjectCS

    void doNotCollideWith( Object* object );
};

#endif // PHYSOBJECT_H
