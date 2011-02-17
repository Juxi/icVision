#include <qgl.h>
#include "solid.h"
#include "object.h"
#include "displmatrix.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

using namespace std;
using namespace RobotModel;

CompositeObject::CompositeObject( const QString& aName ) : objectName(aName), numSpheres(0), numCylinders(0), numBoxes(0)
{
}

CompositeObject::~CompositeObject()
{
	//printf("\n entering delete object");
	QVector<PrimitiveObject*>::iterator i;
	for ( i=begin(); i!=end(); ++i ) {
		delete(*i);
	}
	resize(0);
	//printf("\n exiting delete object");
}

void CompositeObject::setT( const QMatrix4x4& txfr )
{
    T = txfr;
	update();
}

void CompositeObject::setTranslation( const QVector3D& pos )
{
	T.setTranslation( pos );
	update();
}

void CompositeObject::translate( const QVector3D& pos )
{
	T.translate( pos );
	update();
}

void CompositeObject::setCartesianRotation( const QVector3D& rot )
{
	T.setCartesianRotation( rot );
	update();
}

void CompositeObject::cartesianRotate( const QVector3D& rot )
{
	T.cartesianRotate( rot );
	update();
}

void CompositeObject::setAxisAngleRotation( const QVector3D& axis, qreal angle )
{
	T.setAxisAngleRotation(axis, angle);
	update();
}

void CompositeObject::axisAngleRotate( const QVector3D& axis, qreal angle )
{
	T.axisAngleRotate(axis, angle);
	update();
}

void CompositeObject::specialRotate( const QVector3D& axis, qreal angle )
{
	T.specialRotate(axis, angle);
	update();
}

PrimitiveObject* CompositeObject::newSphere( double r, const QVector3D& pos )
{
	QString name =  "sph" + QString::number( ++numSpheres );
	
	PrimitiveObject* primitive = new Sphere( r );
	primitive->setName( name );
	primitive->setTranslation( pos );
	append( primitive );
	return primitive;
}

PrimitiveObject* CompositeObject::newCylinder( double r, double h, const QVector3D& pos )
{
	QString name =  "cyl" + QString::number( ++numCylinders );
	
	PrimitiveObject* primitive = new Cylinder( r, h );
	primitive->setName( name );
	primitive->setTranslation( pos );
	append( primitive );
	return primitive;
}

PrimitiveObject* CompositeObject::newBox( const QVector3D& size, const QVector3D& pos )
{
	QString name =  "box" + QString::number( ++numBoxes );
	
	PrimitiveObject* primitive = new Box( size );
	primitive->setName( name );
	primitive->setTranslation( pos );
	append( primitive );
	return primitive;
}

void CompositeObject::update()
{
    QVector<PrimitiveObject*>::iterator i;
    for (i=begin(); i!=end(); ++i) {
        (*i)->update(T);
    }
}

void CompositeObject::append(PrimitiveObject *primitiveObject)
{
    primitiveObject->setParent( this );
	doNotCheckCollision( primitiveObject );
	QVector<PrimitiveObject*>::append( primitiveObject );
	update();
	//return primitiveObject;
}

bool CompositeObject::remove( PrimitiveObject* primitive )
{
	QVector<PrimitiveObject*>::iterator i;
	for ( i=begin(); i!=end(); ++i ) {
		if ( *i == primitive ) {
			erase(i);
			delete(primitive);
			return 1;
		}
	}
	return 0;
}
void CompositeObject::doNotCheckCollision( const CompositeObject* obj ) const {
    QVector<PrimitiveObject*>::const_iterator i,j;
	for ( i=begin(); i!=end(); ++i ) {
		for ( j=obj->begin(); j!=obj->end(); ++j ) {
			dtClearPairResponse( *i, *j );
		}
	}
}
void CompositeObject::doNotCheckCollision( PrimitiveObject* prim ) const {
    QVector<PrimitiveObject*>::const_iterator i;
	for ( i=begin(); i!=end(); ++i ) {
			dtClearPairResponse( *i, prim );
	}
}

PrimitiveObject* CompositeObject::getPrimitiveByName( const QString& name ) const
{
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=begin(); i!=end(); ++i ) {
		if ( (*i)->getName() == name ) return *i;
	}
	return NULL;
}

void CompositeObject::makeDisplayList()
{
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "An OpenGL error has occured: " << gluErrorString(error) << std::endl;
	}
	
    // get a unique display list index and define the list
    setDisplayListIdx(glGenLists(1));

    glNewList( displayListIdx(), GL_COMPILE );

        //qreal csLen = 1.5*radius;
        GLfloat red[] = {1,0,0,1};
        GLfloat green[] = {0,1,0,1};
        GLfloat blue[] = {0,0,1,1};
        glBegin(GL_LINES);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
            glVertex3f(0,0,0); glVertex3f(1,0,0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, green);
            glVertex3f(0,0,0); glVertex3f(0,1,0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, blue);
            glVertex3f(0,0,0); glVertex3f(0,0,1);
        glEnd();

    glEndList();
    //cout << "made object display list #" << displayListIdx() << endl;

    // make display lists for primitives
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->makeDisplayList();
    }
}

void CompositeObject::notColliding()
{
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i )
	{
        (*i)->unSetColliding();
    }
}

void CompositeObject::render()
{
    //DisplayList::render();
    //cout << "called render() on object" << endl;
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->render();
        //cout << "called render() on primitive" << endl;
    }
}
