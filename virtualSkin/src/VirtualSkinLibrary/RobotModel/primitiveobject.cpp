#include "primitiveobject.h"
#include "object.h"

using namespace RobotModel;

PrimitiveObject::PrimitiveObject() : parent(0)
{
}
PrimitiveObject::~PrimitiveObject()
{
}
void PrimitiveObject::setParent( CompositeObject* object )
{
	if (object) {
		parent = object;
		doNotCollideWith(parent);
	}
}
void PrimitiveObject::setL( const QMatrix4x4& txfr )
{
    L = txfr;  // this matrix positions the object relative to the linkCS
    if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setTranslation( const QVector3D& pos )
{
	L.setTranslation( pos );
	if (parent) { update(parent->getT()); }
}
void PrimitiveObject::translate( const QVector3D& pos )
{
	L.translate( pos );
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setCartesianRotation( const QVector3D& rot )
{
	L.setCartesianRotation( rot );
	if (parent) { update(parent->getT()); }
}
void PrimitiveObject::cartesianRotate( const QVector3D& rot )
{
	L.cartesianRotate( rot );
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setAxisAngleRotation( const QVector3D& axis, qreal angle )
{
	L.setAxisAngleRotation(axis, angle);
	if (parent) { update(parent->getT()); }
}
void PrimitiveObject::axisAngleRotate( const QVector3D& axis, qreal angle )
{
	L.axisAngleRotate(axis, angle);
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::specialRotate( const QVector3D& axis, qreal angle )
{
	L.specialRotate(axis, angle);
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::update( const QMatrix4x4& txfr )
{
    T = txfr * L;
    dtSelectObject( this );
    dtLoadIdentity();
    dtMultMatrixd( T.constData() );
}
void PrimitiveObject::doNotCollideWith( CompositeObject* object )
{
    QVector<PrimitiveObject*>::iterator i;
        for ( i=object->begin(); i!=object->end(); ++i ) {
                dtClearPairResponse( *i, this );
        }
}
