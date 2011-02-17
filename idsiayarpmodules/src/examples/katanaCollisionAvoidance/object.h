#ifndef OBJECT_H
#define OBJECT_H

#include <QMatrix4x4>
#include "displaylist.h"

class PrimitiveObject;

class Object : public QVector<PrimitiveObject*>, public DisplayList
{
public:
    Object( Object* parent = 0 );
    ~Object();


    void makeDisplayList();
    void render();
    void setT( const QMatrix4x4& txfr );
    void append( PrimitiveObject* primitiveObject );
    void doNotCheckCollision( const Object* a );
    void doNotCheckCollision( const Object* a, const Object* b );
    Object* parent() { return parentObject; }

private:
    Object* parentObject;
};

#endif // OBJECT_H
