#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include <QtGui>
#include "include/SOLID/solid.h"

class PhysObject
{
public:
    PhysObject();
    virtual ~PhysObject();

    int displayList();
    DtObjectRef* solid();
    const QMatrix4x4& txfrToWorld() const;
    void updateTxfrToWorld( const QMatrix4x4& T );
    virtual void makeDisplayList() = 0;

protected:
    DtObjectRef solidObject;
    QMatrix4x4  txfrToLinkCS,
                txfrToWorldCS;
    int         displayListIdx;
    bool        colliding;      // controls when to render stuff red
};

#endif // PHYSOBJECT_H
