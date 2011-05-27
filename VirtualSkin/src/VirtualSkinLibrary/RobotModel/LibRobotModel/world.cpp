#include "world.h"
#include <QMatrix4x4>
#include "model.h"

using namespace RobotModel;

World::World( Model* m ) :	model(m),
							numSpheres(0), numCylinders(0), numBoxes(0),
							numSSpheres(0),numSCylinders(0),numSBoxes(0)
{
	if ( !m ) { throw RobotModelException("The World constructor requires a pointer to a valid Model."); }
	worldResponseClass = DT_GenResponseClass( model->getResponseTable() );
	DT_RemovePairResponse( model->getResponseTable(), worldResponseClass, worldResponseClass, model->collisionHandler );
}
World::~World()
{
	//closeRpcInterface();
	
	QVector<CompositeObject*>::iterator i;
	QVector<PrimitiveObject*>::iterator j;
	for ( i=objectList.begin(); i!=objectList.end(); ++i )
	{
		for ( j=(*i)->begin(); j!=(*i)->end(); ++j )
		{
			emit outdatedDisplayList( (*j)->displayListIdx() );
			delete(*j);
		}
		emit outdatedDisplayList( (*i)->displayListIdx() );
		delete(*i);
	}
}
CompositeObject* World::newSphere( double r, const QVector3D& pos )
{
	QString name =  "sph" + QString::number( ++numSpheres );
	
	CompositeObject* object = new CompositeObject( name );	
	object->newSphere(r);
	object->setPosition(pos);
	return append( object );
}
CompositeObject* World::newSSphere( double r, const QVector3D& pos )
{
	QString name =  "ssph" + QString::number( ++numSSpheres );
	
	CompositeObject* object = new CompositeObject( name );		
	object->newSphere(r);
	object->setPosition(pos);
	return append( object );
}
CompositeObject* World::newCylinder( double r, double h, const QVector3D& pos )
{
	QString name =  "cyl" + QString::number( ++numCylinders );

	CompositeObject* object = new CompositeObject( name );		
	object->newCylinder(r, h);
	object->setPosition(pos);
	return append( object );
}
CompositeObject* World::newSCylinder( double r, double h, const QVector3D& pos )
{
	QString name =  "scyl" + QString::number( ++numSCylinders );
	
	CompositeObject* object = new CompositeObject( name );		
	object->newCylinder(r, h);
	object->setPosition(pos);
	return append( object );
}
CompositeObject* World::newBox( const QVector3D& size, const QVector3D& pos )
{
	QString name =  "box" + QString::number( ++numBoxes );

	CompositeObject* object = new CompositeObject( name );		
	object->newBox(size);
	object->setPosition(pos);
	return append( object );
}
CompositeObject* World::newSBox( const QVector3D& size, const QVector3D& pos )
{
	QString name =  "sbox" + QString::number( ++numSBoxes );
	
	CompositeObject* object = new CompositeObject( name );		
	object->newBox(size);
	object->setPosition(pos);
	return append( object );
}
CompositeObject* World::newObject( const QString& name, const QVector3D& pos )
{
	CompositeObject* object = new CompositeObject( name );
	object->setPosition(pos);
	return append( object );	// this copies the object into the list
}
CompositeObject* World::append( CompositeObject* obj )
{
	obj->configureSolid( model, worldResponseClass );
	objectList.append( obj );
	
	QVector<PrimitiveObject*>::iterator i;
	for ( i=obj->begin(); i!=obj->end(); ++i ) // these are primitive objects
	{
		emit appendedObject( static_cast<DisplayList*>(*i) );
	}
	emit appendedObject( static_cast<DisplayList*>(obj) ); // this is the composite object (which may have the displayList for the local CS
	
	return obj;
}

CompositeObject* World::getObjectByName( const QString& name )
{
	CompositeObject* object = NULL;
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i ) {
        if ( name == (*i)->getName() ) { 
			printf("FoundName: %s\n",(*i)->getName().toStdString().c_str());
			object =  *i;
		}
    }
	return object;
}

void World::render()
{
	mutex.lock();
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i ) {
        (*i)->render();
    }
	mutex.unlock();
}

bool World::remove( CompositeObject* obj )
{
	bool foundObject = false;
	
	mutex.lock();
		QVector<CompositeObject*>::iterator i;
		for ( i=objectList.begin(); i!=objectList.end(); ++i ) {
			if ( *i == obj ) {
				objectList.erase(i);
				foundObject = true;
				break;
			}
		}
	mutex.unlock();
	
	QVector<PrimitiveObject*>::iterator j;
	for ( j=obj->end(); j!=obj->begin(); )
	{
		--j;
		emit outdatedDisplayList( (*j)->displayListIdx() );
		obj->remove(*j);
	}
	emit outdatedDisplayList( obj->displayListIdx() );
	
	delete(obj);
	
	return foundObject;
}

bool World::removePrimitive( CompositeObject* object, PrimitiveObject* primitive )
{
	mutex.lock();
		emit outdatedDisplayList( primitive->displayListIdx() );
		object->remove( primitive );
	mutex.unlock();
	return 0;
}

void World::clear()
{
	QVector<CompositeObject*>::iterator i;
	for ( i=objectList.end(); i!=objectList.begin(); )
	{ 
		--i;
		remove(*i);
	}
	
	numBoxes = 0;		numSBoxes = 0;
	numCylinders = 0;	numSCylinders = 0;
	numSpheres = 0;		numSSpheres = 0;
}