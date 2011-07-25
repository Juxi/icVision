//#include <SOLID.h>
#include "compositeobject.h"
#include "primitiveobject.h"
#include "modelexception.h"

using namespace std;
using namespace KinematicModel;

CompositeObject::CompositeObject( DT_ResponseClass c, DT_RespTableHandle t ) : responseClass(c),
																			   inModel(false), 
																			   deathWish(false),
																			   numSpheres(0),
																			   numCylinders(0),
																			   numBoxes(0)
{
}

CompositeObject::~CompositeObject()
{
	//QMutexLocker locker(&mutex);
	
	//printf("CompositeObject destructor called on: %s\n", getName().toStdString().c_str());
	QVector<PrimitiveObject*>::iterator i;
	for ( i=primitives.end(); i!=primitives.begin(); )
	{
		--i;
		delete(*i);
	}
	//printf("deleted CompositeObject\n");
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

//void CompositeObject::setListPending()
//{
//    QVector<PrimitiveObject*>::iterator i;
//    for (i=primitives.begin(); i!=primitives.end(); ++i)
//	{
//        (*i)->setListPending(true);
//    }
//}

void CompositeObject::updateSolid()
{
    QVector<PrimitiveObject*>::iterator i;
    for (i=primitives.begin(); i!=primitives.end(); ++i)
	{
        (*i)->updateSolid(T);
    }
}

void CompositeObject::render()
{
	//QMutexLocker locker(&mutex);
	
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
	//QVector<PrimitiveObject*>::iterator i;
	//for ( i=primitives.end(); i!=primitives.begin(); )
	//{
	//	(*i)->kill();
	//}
	deathWish = true;
}
/*QVector<Garbage> CompositeObject::collectGarbage()
{
	Garbage garbage;
	QVector<Garbage> theTrash;
	QVector<PrimitiveObject*>::iterator i;
	for ( i=primitives.end(); i!=primitives.begin(); )
	{
		--i;
		if ( (*i)->deleteMe() && !(*i)->listIsPending() )
		{
			if ( (*i)->getObjectHandle() )
			garbage.solidObject = (*i)->getObjectHandle();
			garbage.displayList = (*i)->getDisplayList();
			theTrash.append(garbage);
			primitives.erase(i);
			delete(*i);
		}
	}
	return theTrash;
}*/
