#include "primitiveobject.h"
#include "compositeObject.h"

using namespace RobotModel;

PrimitiveObject::PrimitiveObject() : parent(0), shape(NULL), solidObject(NULL)
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
	DT_SetMatrixd( solidObject , T.constData() );
}

/*void PrimitiveObject::doNotCollideWith( CompositeObject* object )
{
    QVector<PrimitiveObject*>::iterator i;
        for ( i=object->begin(); i!=object->end(); ++i ) {
                dtClearPairResponse( *i, this );
        }
}*/

CompositeObject::ObjType PrimitiveObject::getObjectType()
{
	if (parent) { return parent->getObjectType(); }
	else { return CompositeObject::NO_TYPE; }
}

void PrimitiveObject::render()
{
	if ( glIsList(index) )
	{
		GLfloat* color = gray;
		switch ( getObjectType() ) {
			case CompositeObject::OBSTACLE: 
				if ( isColliding() ) { color = blue; }
				else { color = transpBlue; }
				break;
			case CompositeObject::TARGET: 
				if ( isColliding() ) { color = green; }
				else { color = transpGreen; }
				break;
			case CompositeObject::BODY_PART: 
				if ( isColliding() ) { color = red; }
				else { color = gray; }
				break;
			default:
				if ( isColliding() ) { color = transpRed; }
				else { color = transpGray; }
				break;
		}
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, color);
		
		glPushMatrix();
		glMultMatrixd( T.constData() );
		glCallList( index );
		glPopMatrix();
	}
}
