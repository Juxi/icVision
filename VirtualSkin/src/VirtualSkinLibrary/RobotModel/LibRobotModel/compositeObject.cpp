#include <qgl.h>
#include <SOLID.h>
#include "compositeObject.h"
#include "displmatrix.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "model.h"

using namespace std;
using namespace RobotModel;

CompositeObject::CompositeObject( const QString& aName ) :	model(NULL),
															responseClass(-1), 
															objectName(aName),
															numSpheres(0),
															numCylinders(0),
															numBoxes(0)
{		
}

CompositeObject::~CompositeObject()
{
	QVector<PrimitiveObject*>::iterator i;
	for ( i=begin(); i!=end(); ++i ) {
		delete(*i);
	}
	resize(0);
}

void CompositeObject::configureSolid( Model* m, DT_ResponseClass c)
{
	model = m;
	if ( c != static_cast<DT_ResponseClass>(-1) ) { responseClass = c; }
	else { responseClass = DT_GenResponseClass( model->getResponseTable() ); }
	
	QVector<PrimitiveObject*>::iterator i;
	for (i=begin(); i!=end(); ++i)
	{
		DT_AddObject( model->getScene(), (*i)->getSolidHandle() );
		DT_SetResponseClass( model->getResponseTable(), (*i)->getSolidHandle(), responseClass );
	}
}

void CompositeObject::append(PrimitiveObject *primitiveObject)
{
	if ( model && responseClass != static_cast<DT_ResponseClass>(-1) )
	{
		DT_AddObject( model->getScene(), primitiveObject->getSolidHandle() );
		DT_SetResponseClass( model->getResponseTable(), primitiveObject->getSolidHandle(), responseClass );
	}
	
    primitiveObject->setParent( this );
	QVector<PrimitiveObject*>::append( primitiveObject );
	update();
}

void CompositeObject::doNotCheckCollision( DT_ResponseClass responseClass2 ) const
{
	//printf("Cleared a pair response %i, %i\n", responseClass, obj->getResponseClass() );
	DT_RemovePairResponse( model->getResponseTable(), responseClass, responseClass2, model->collisionHandler);
}

bool CompositeObject::remove( PrimitiveObject* primitive )
{
	DT_RemoveObject( model->getScene(), primitive->getSolidHandle() );
	
	QVector<PrimitiveObject*>::iterator i;
	for ( i=begin(); i!=end(); ++i )
	{
		if ( *i == primitive )
		{
			erase(i);
			delete(primitive);
			return 1;
		}
	}
	return 0;
}

void CompositeObject::setT( const QMatrix4x4& txfr )
{
    T = txfr;
	update();
}

void CompositeObject::setPosition( const QVector3D& pos )
{
	T.setPosition( pos );
	update();
}

void CompositeObject::translate( const QVector3D& pos )
{
	T.translate( pos );
	update();
}

void CompositeObject::setCartesianOrientation( const QVector3D& rot )
{
	T.setCartesianOrientation( rot );
	update();
}

void CompositeObject::cartesianRotate( const QVector3D& rot )
{
	T.cartesianRotate( rot );
	update();
}

void CompositeObject::setAxisAngleOrientation( const QVector3D& axis, qreal angle )
{
	T.setAxisAngleOrientation(axis, angle);
	update();
}

void CompositeObject::axisAngleRotate( const QVector3D& axis, qreal angle )
{
	T.axisAngleRotate(axis, angle);
	update();
}

void CompositeObject::setSpecialEulerOrientation( const QVector3D& axis, qreal angle )
{
	T.setSpecialEulerOrientation(axis, angle);
	update();
}

PrimitiveObject* CompositeObject::newSphere( double r, const QVector3D& pos )
{
	QString name =  "sph" + QString::number( ++numSpheres );
	
	PrimitiveObject* primitive = new Sphere( r );
	primitive->setName( name );
	primitive->setPosition( pos );
	append( primitive );
	return primitive;
}

PrimitiveObject* CompositeObject::newCylinder( double r, double h, const QVector3D& pos )
{
	QString name =  "cyl" + QString::number( ++numCylinders );
	
	PrimitiveObject* primitive = new Cylinder( r, h );
	primitive->setName( name );
	primitive->setPosition( pos );
	append( primitive );
	return primitive;
}

PrimitiveObject* CompositeObject::newBox( const QVector3D& size, const QVector3D& pos )
{
	QString name =  "box" + QString::number( ++numBoxes );
	
	PrimitiveObject* primitive = new Box( size );
	primitive->setName( name );
	primitive->setPosition( pos );
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
	if (error != GL_NO_ERROR)
	{
		printf("An OpenGL error has occured: %s\n", gluErrorString(error));
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

    // make display lists for primitives
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->makeDisplayList();
    }
}

void CompositeObject::render()
{
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->render();
    }
}
