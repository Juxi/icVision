#include "primitiveobject.h"
#include "compositeObject.h"

using namespace RobotModel;

PrimitiveObject::PrimitiveObject() : parent(0)
{
}
PrimitiveObject::~PrimitiveObject()
{
	if ( solidObject ) { DT_DestroyObject(solidObject); }
	if ( shape ) { DT_DeleteShape(shape); }
}
void PrimitiveObject::setParent( CompositeObject* object )
{
	if (object) {
		parent = object;
		//doNotCollideWith(parent);
	}
}
void PrimitiveObject::setL( const QMatrix4x4& txfr )
{
    L = txfr;  // this matrix positions the object relative to the linkCS
    if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setPosition( const QVector3D& pos )
{
	L.setPosition( pos );
	if (parent) { update(parent->getT()); }
}
void PrimitiveObject::translate( const QVector3D& pos )
{
	L.translate( pos );
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setCartesianOrientation( const QVector3D& rot )
{
	L.setCartesianOrientation( rot );
	if (parent) { update(parent->getT()); }
}
void PrimitiveObject::cartesianRotate( const QVector3D& rot )
{
	L.cartesianRotate( rot );
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setAxisAngleOrientation( const QVector3D& axis, qreal angle )
{
	L.setAxisAngleOrientation(axis, angle);
	if (parent) { update(parent->getT()); }
}
void PrimitiveObject::axisAngleRotate( const QVector3D& axis, qreal angle )
{
	L.axisAngleRotate(axis, angle);
	if (parent) { update(parent->getT()); }
}

void PrimitiveObject::setSpecialEulerOrientation( const QVector3D& axis, qreal angle )
{
	L.setSpecialEulerOrientation(axis, angle);
	if (parent) { update(parent->getT()); }
}

//void PrimitiveObject::specialEulerRotate( const QVector3D& axis, qreal angle )
//{
//	L.specialEulerRotate(axis, angle);
//	if (parent) { update(parent->getT()); }
//}

void PrimitiveObject::update( const QMatrix4x4& txfr )
{
    T = txfr * L;
    //dtSelectObject( this );
    //dtLoadIdentity();
    //dtMultMatrixd( T.constData() );
	DT_SetMatrixd( solidObject , T.constData() );
}
/*void PrimitiveObject::doNotCollideWith( CompositeObject* object )
{
    QVector<PrimitiveObject*>::iterator i;
        for ( i=object->begin(); i!=object->end(); ++i ) {
                dtClearPairResponse( *i, this );
        }
}*/
