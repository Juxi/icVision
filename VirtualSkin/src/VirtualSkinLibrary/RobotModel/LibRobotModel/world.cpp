#include "world.h"
#include <QMatrix4x4>
#include "model.h"
#include "worldhandler.h"

using namespace RobotModel;

World::World( Model* m ) :	model(m),
							numSpheres(0), numCylinders(0), numBoxes(0),
							numSSpheres(0),numSCylinders(0),numSBoxes(0)
{
	if ( !m ) { throw RobotModelException("The World constructor requires a pointer to a valid Model."); }
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

void World::load( const QString& fileName )
{
	if ( fileName != "" )
	{
		WorldHandler handler(this);
		QXmlSimpleReader reader;
		reader.setContentHandler(&handler);
		reader.setErrorHandler(&handler);
		QFile file(fileName);
		
		if ( !file.open(QFile::ReadOnly | QFile::Text) )
		{
			QString errStr = "failed to open file '";
			errStr.append(fileName);
			errStr.append("'");
			throw RobotModelException(errStr);
		}
		
		QXmlInputSource xmlInputSource( &file );
		if ( !reader.parse( xmlInputSource ) )
		{
			QString errStr = "failed to initialize world from file ";
			errStr.append(fileName);
			errStr.append("'");
			throw RobotModelException(errStr);
		}
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
	// render the object as an 'obstacle'
	obj->setObjectType(CompositeObject::OBSTACLE);
	objectList.append( obj );
	
	QVector<PrimitiveObject*>::iterator i;
	for ( i=obj->begin(); i!=obj->end(); ++i ) // these are primitive objects
	{
		emit worldAppendedPrimitive( *i );
		emit requestDisplayList( static_cast<DisplayList*>(*i) );
	}
	emit requestDisplayList( static_cast<DisplayList*>(obj) ); // this is the composite object (which may have the displayList for the local CS
	
	return obj;
}

bool World::appendToObject( CompositeObject* object, PrimitiveObject* primitive )
{
	bool foundObject = false;
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i )
	{
		if ( *i == object )
		{
			foundObject = true;
			break;
		}
	}
	
	if ( foundObject )
	{
		object->append( primitive );
		emit worldAppendedPrimitive( primitive );
	}
	
	return foundObject;
}

bool World::changeResponseClass( CompositeObject* object, DT_ResponseClass respClass)
{
	bool foundObject = false;
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i )
	{
		if ( *i == object )
		{
			foundObject = true;
			break;
		}
	}
	
	if ( foundObject )
	{
		QVector<PrimitiveObject*>::iterator j;
		for ( j=object->begin(); j!=object->end(); ++j )
		{
			emit requestNewResponseClass( *j, respClass );
		}
	}
	
	return foundObject;
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
	QMutexLocker locker(&mutex);

	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i ) {
        (*i)->render();
    }
}

void World::update()
{
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i ) {
        (*i)->update();
    }
}

QVector<QString> World::getList()
{
	//QMutexLocker locker(&mutex);
	
	QVector<QString> list;
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i ) {
        list.append( (*i)->getName() );
    }
	return list;
}

bool World::remove( CompositeObject* obj )
{
	QMutexLocker locker(&mutex);
	
	bool foundObject = false;
	
	QVector<CompositeObject*>::iterator i;
	for ( i=objectList.end(); i!=objectList.begin(); ) {
		--i;
		if ( *i == obj ) {
			objectList.erase(i);
			foundObject = true;
			break;
		}
	}
	
	//locker.unlock();
	
	QVector<PrimitiveObject*>::iterator j;
	for ( j=obj->end(); j!=obj->begin(); )
	{
		--j;
		emit requestRemoveSolidObject( (*j)->getSolidHandle(), (*j)->getShape() );
		emit outdatedDisplayList( (*j)->displayListIdx() );
		//model->destroyObject((*j)->getSolidHandle());
		obj->remove(*j);
	}
	emit outdatedDisplayList( obj->displayListIdx() );
	
	delete(obj);
	
	return foundObject;
}

bool World::removePrimitive( CompositeObject* object, PrimitiveObject* primitive )
{
	bool foundObject = false;
	QVector<CompositeObject*>::iterator i;
    for ( i=objectList.begin(); i!=objectList.end(); ++i )
	{
		if ( *i == object )
		{
			foundObject = true;
			break;
		}
	}
	
	if ( foundObject )
	{
		emit requestRemoveSolidObject( primitive->getSolidHandle(), primitive->getShape() );
		emit outdatedDisplayList( primitive->displayListIdx() );
		object->remove( primitive );
	}
	
	return foundObject;
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