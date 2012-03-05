//#include <SOLID.h>
#include "compositeobject.h"
#include "primitiveobject.h"
#include "modelexception.h"

using namespace std;
using namespace KinematicModel;

CompositeObject::CompositeObject( DT_ResponseClass c ) :	index(0),
															responseClass(c),
															inModel(false), 
															deathWish(false),
															numSpheres(0),
															numCylinders(0),
															numBoxes(0)
{
	collidingColor[0] = 0.7; collidingColor[1] = 0.0; collidingColor[2] = 0.0; collidingColor[3] = 0.5;
    //collidingColor[0] = 0.3; collidingColor[1] = 0.3; collidingColor[2] = 0.3; collidingColor[3] = 0.2;
	
    freeColor[0] = 0.3;	  freeColor[1] = 0.3;	   freeColor[2] = 0.3;	    freeColor[3] = 1.0; 
	//black[0] = 0.0;		  black[0] = 0.0;		   black[0] = 0.0;			black[0] = 1.0;
}

CompositeObject::~CompositeObject()
{
	QVector<PrimitiveObject*>::iterator i;
	for ( i=primitives.end(); i!=primitives.begin(); )
	{
		--i;
		delete(*i);
	}
}

QString	CompositeObject::getName() const
{
	//printf( "index: %s", QString(index).toStdString().c_str() );
	if ( objectName != "" ) { return objectName; }
	else { return QString("object") + QString::number(index); }
}				

void CompositeObject::append( PrimitiveObject* primitive )
{
	if ( inModel ) 
	{
		throw KinematicModelException("Tried to append primitives to a CompositeObject already in the Model. Remove the CompositeObject, edit it, and then re-append to the model.");
	}
	else if ( primitive->getGeomType() != SPHERE &&
			  primitive->getGeomType() != CYLINDER &&
			  primitive->getGeomType() != BOX )
	{
		throw KinematicModelException("CompositeObject::append(PrimitiveObject*) encountered unknown GeomType. Use SPHERE, CYLINDER, or BOX");
	}
	else if ( primitive->getName() == "" )
	{
		QString name;
		switch (primitive->getGeomType())
		{
			case SPHERE:	name = "sph";	name.append(++numSpheres);		break;
			case CYLINDER:	name = "cyl";	name.append(++numCylinders);	break;
			case BOX:		name = "box";	name.append(++numBoxes);		break;
		}
		primitive->setName(name);
	}
	QColor fc,cc;
	fc.setRedF(freeColor[0]); fc.setGreenF(freeColor[1]); fc.setBlueF(freeColor[2]); fc.setAlphaF(freeColor[3]);
	cc.setRedF(collidingColor[0]); cc.setGreenF(collidingColor[1]); cc.setBlueF(collidingColor[2]);	cc.setAlphaF(collidingColor[3]);
	
	primitive->setCompositeObject(this);
	primitive->setFreeColor(fc);
	primitive->setCollidingColor(cc);
	
	primitives.append(primitive);
}

bool CompositeObject::remove( PrimitiveObject* primitive )
{
	if ( inModel ) 
	{
		throw KinematicModelException("Tried to remove primitives from a CompositeObject already in the Model. Remove the CompositeObject, edit it, and then re-append to the model.");
	}
	
	QVector<PrimitiveObject*>::iterator i;
	for ( i=primitives.end(); i!=primitives.begin(); )
	{
		--i;
		if ( *i == primitive )
		{
			primitives.erase(i);
			return 1;
		}
	}
	return 0;
}

PrimitiveObject* CompositeObject::get( const QString& name ) const
{
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		if ( (*i)->getName() == name ) { return *i; }
	}
	return NULL;
}

void CompositeObject::updateSolid()
{
    QVector<PrimitiveObject*>::iterator i;
    for (i=primitives.begin(); i!=primitives.end(); ++i)
	{
        (*i)->updateSolid(T);
    }
}

void CompositeObject::setCollidingColor( QColor c )
{ 
	collidingColor[0] = c.redF();
	collidingColor[1] = c.greenF();
	collidingColor[2] = c.blueF();
	collidingColor[3] = c.alphaF();
	
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		(*i)->setCollidingColor(c);
	}
}

void CompositeObject::setFreeColor( QColor c )
{ 
	//printf("CALLED SET OBJECT>SETFREECOLOR()\n");
	freeColor[0] = c.redF();
	freeColor[1] = c.greenF();
	freeColor[2] = c.blueF();
	freeColor[3] = c.alphaF();
	
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		(*i)->setFreeColor(c);
	}
}

void CompositeObject::render()
{
	glPushMatrix();
	glMultMatrixd( T.constData() );
		QVector<PrimitiveObject*>::const_iterator i;
		for ( i=primitives.begin(); i!=primitives.end(); ++i )
		{
			(*i)->render();
		}
	glPopMatrix();
}

void CompositeObject::kill()
{
	deathWish = true;
}
