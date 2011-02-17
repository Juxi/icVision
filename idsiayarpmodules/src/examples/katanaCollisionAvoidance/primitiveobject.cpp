#include "object.h"
#include "primitiveobject.h"
#include "displmatrix.h"
#include <iostream>

using namespace std;

PrimitiveObject::PrimitiveObject( Object* parentObject )
{
    setParent(parentObject);
}
PrimitiveObject::~PrimitiveObject()
{
}
void PrimitiveObject::setParent( Object* object )
{
    parent = object;
    if (parent) doNotCollideWith(parent);
}
void PrimitiveObject::setL( const QMatrix4x4& txfr )
{
    L = txfr;         // this matrix positions the object relative to the linkCS
    setT();    //
}
void PrimitiveObject::setT()
{
    if (parent) setT(parent->getT());
    else setT(QMatrix4x4());
}
void PrimitiveObject::setT( const QMatrix4x4& txfr )
{
    DisplayList::setT( txfr * L );
    dtSelectObject( this );
    dtLoadIdentity();
    dtMultMatrixd( getT().constData() );
}
void PrimitiveObject::doNotCollideWith( Object* object )
{
    QVector<PrimitiveObject*>::iterator i;
        for ( i=object->begin(); i!=object->end(); ++i ) {
                dtClearPairResponse( *i, this );
        }
}
