#include "model.h"
#include "robot.h"
#include "worldhandler.h"
#include "modelwindow.h"
#include <iostream>

using namespace KinematicModel;

Model::Model( bool visualize, bool verb ) : keepRunning(true),
											col_count(0),
											reflex_col_count(0),
											encObstacle(false), 
											verbose(verb), 
											modelWindow(NULL), 
											numObjects(0),
											numPrimitives(0),
											stopOnFirstCollision(DT_CONTINUE),
											syncGraphics(true)

{
	qRegisterMetaType< DT_ResponseClass >("GL_DisplayList");
	
	// initialize the SOLID datastructures for managing collision response
	scene = DT_CreateScene();
    worldTable = DT_CreateRespTable();
	//responseTables.append(DT_CreateRespTable());                // the first response table is for managing the world
                                                                // (with multiple robots and other objects)
	//printf("Created World Response table %p\n",worldTable);
	
	// create SOLID Response classes for the world table
	obstacleClass = DT_GenResponseClass(worldTable);
	targetClass = DT_GenResponseClass(worldTable);
	ghostClass = DT_GenResponseClass(worldTable);
    fieldClass = DT_GenResponseClass(worldTable);
	
	// set up the window for OpenGL
	if ( visualize )
	{ 
		//printf("CREATING WINDOW AND CONNECTING SIGNALS AND SLOTS\n");
		
		modelWindow = new ModelWindow();
		
		QObject::connect( this, SIGNAL(addedPrimitive(PrimitiveObject*)),	modelWindow->glWidget, SLOT(addDisplayList(PrimitiveObject*)) );
		QObject::connect( this, SIGNAL(removedPrimitive(GL_DisplayList)),	modelWindow->glWidget, SLOT(removeDisplayList(GL_DisplayList)) );
		QObject::connect( this, SIGNAL(computedState(int)),					modelWindow->glWidget, SLOT(update(int)) );
		QObject::connect( modelWindow->glWidget, SIGNAL(renderStuff()),		this, SLOT(renderModel()), Qt::DirectConnection );

		//printf("showing model window\n");
		modelWindow->show();
		 
	}
	
	objectMover = new ObjectMover(this, &robots);

	//printf("model constructor returns\n");
}

Model::~Model()
{
	if ( isRunning() ) { stop(); }
	
	QVector<Robot*>::iterator k;
	for ( k=robots.end(); k!=robots.begin(); )
	{
		--k;
		delete(*k);
	}
	
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); ++i )
	{
		(*i)->kill();
	}
	
	cleanTheWorld();
	
	delete objectMover;

	if ( modelWindow ) { delete(modelWindow); }
	
	QVector<Robot*>::iterator j;
	for ( j=robots.begin(); j!=robots.end(); ++j ) {
		DT_DestroyRespTable((*j)->getResponseTable());
		DT_DestroyRespTable((*j)->getFieldResponseTable());
	}
    DT_DestroyRespTable(worldTable);
}

/****************
*** MODEL I/O ***
****************/
DT_RespTableHandle Model::newRobotTable()
{
	DT_RespTableHandle table = DT_CreateRespTable();
	//printf("Created Robot Response table %p\n",table);
	DT_AddDefaultResponse( table, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
    DT_AddDefaultResponse( table, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );
	//responseTables.append( table );
	return table;
}

DT_RespTableHandle Model::newRobotFieldTable()
{
	DT_RespTableHandle table = DT_CreateRespTable();
    DT_AddDefaultResponse( table, repel, DT_DEPTH_RESPONSE, (void*) this );
    DT_AddDefaultResponse( table, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );
	//responseTables.append( table );
	return table;
}

void Model::removeAllResponses( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 )
{
	removeReflexResponse(t,c1,c2);
	removeVisualResponse(t,c1,c2);
    removeForceResponse(t,c1,c2);
}

void Model::removeReflexResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 )
{
	//printf("REMOVE_REFLEX");
	DT_RemovePairResponse(t, c1, c2, reflexTrigger);
	//printf(" removed response '%p' from collision pair (%d,%d) in table %p\n", reflexTrigger, c1, c2, t );
}

void Model::removeVisualResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 )
{
	//printf("REMOVE_VISUAL");
	DT_RemovePairResponse(t, c1, c2, collisionHandler);
	//printf(" removed response '%p' from collision pair (%d,%d) in table %p\n", collisionHandler, c1, c2, t );
}

void Model::removeForceResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 )
{
	//printf("REMOVE_VISUAL");
	DT_RemovePairResponse(t, c1, c2, repel);
	//printf(" removed response '%p' from collision pair (%d,%d) in table %p\n", collisionHandler, c1, c2, t );
}

/*void Model::setVisualResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 )
{
	//printf("SET_VISUAL_RESPONSE\n");
	//DT_RemovePairResponse(t, c1, c2, reflexTrigger);
	//printf("  removed response '%p' from collision pair (%d,%d) in table %p\n", reflexTrigger, c1, c2, t );
	
	DT_AddPairResponse(	t, c1, c2, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );
	//printf("  added   response '%p' from collision pair (%d,%d) in table %p\n", collisionHandler, c1, c2, t );
}*/

DT_ResponseClass Model::newResponseClass( DT_RespTableHandle table )
{
	//if (verbose) printf("Creating Robot Response Class\n");
	DT_ResponseClass respClass = DT_GenResponseClass(table);
	return respClass;
}

Robot* Model::loadRobot( const QString& fileName, bool verbose)
{	
	mutex.lockForWrite();

	DT_ResponseClass newRobotClass     = newResponseClass( worldTable );
	DT_ResponseClass newBaseClass      = newResponseClass( worldTable );
	DT_ResponseClass newFieldClass     = newResponseClass( worldTable );
	DT_ResponseClass newBaseFieldClass = newResponseClass( worldTable );

	DT_AddPairResponse(	worldTable, newRobotClass, obstacleClass, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	DT_AddPairResponse(	worldTable, newRobotClass, obstacleClass, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );
	DT_AddPairResponse(	worldTable, newRobotClass, targetClass,   collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );
	DT_AddPairResponse(	worldTable, newFieldClass, obstacleClass, repel, DT_DEPTH_RESPONSE, (void*) this );
	DT_AddPairResponse(	worldTable, newFieldClass, obstacleClass, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );

	QVector<DT_ResponseClass>::iterator i;
	for ( i = robotResponseClasses.begin(); i != robotResponseClasses.end(); ++i )
	{
		DT_AddPairResponse(	worldTable, newRobotClass,        *i, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
		DT_AddPairResponse(	worldTable, newBaseClass,         *i, reflexTrigger,  DT_WITNESSED_RESPONSE, (void*) this );
		DT_AddPairResponse(	worldTable, newFieldClass,        *i, repel,         DT_DEPTH_RESPONSE, (void*) this );
		DT_AddPairResponse(	worldTable, newBaseFieldClass,    *i, repel,     DT_DEPTH_RESPONSE, (void*) this );
	}

	robotResponseClasses.append( newRobotClass );
	robotResponseClasses.append( newFieldClass );
	//fieldResponseClasses.append( newFieldClass );
	//robotBaseClasses.append( newBaseClass );

	//printf("Loading non-yarp robot.\n");
	DT_RespTableHandle newTable = newRobotTable();              // a table for handling self collisions
	DT_RespTableHandle newFieldTable = newRobotFieldTable();	// a table for handling self repulsion
	Robot* robot = new Robot( this,
		newTable,
		newFieldTable,
		newRobotClass,
		newBaseClass,
		newFieldClass,
		newBaseFieldClass );
	mutex.unlock();

	robot->open( fileName, verbose ); // open calls appendObject, which locks the mutex by itself

	mutex.lockForWrite();
	robots.append( robot );
	mutex.unlock();

	robot->appendMarkersToModel();

	return robot;
}

void Model::loadWorld( const QString& fileName, bool verbose )
{
	//printf("Loading world file...\n");
    WorldHandler handler( this );
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QFile file(fileName);
	
    if ( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		QString errStr = "failed to open file '";
		errStr.append(fileName);
		errStr.append("'");
		throw KinematicModelException(errStr);
    }
	
    QXmlInputSource xmlInputSource( &file );
    if ( !reader.parse( xmlInputSource ) )
	{
		QString errStr = "failed to create world from file '";
		errStr.append(fileName);
		errStr.append("'");
		throw KinematicModelException(errStr);
    }
	//printf("success!\n");
}

void Model::appendObject( KinTreeNode* node )
{
	QWriteLocker locker(&mutex);

	//if ( verbose ) printf("  appending robot object.\n");
	if ( !node->getResponseClass() || !node->robot()->getResponseTable() || !node->robot()->getWorldRobotClass() )
	{
		throw KinematicModelException("CompositeObject must have a DT_RespTableHandle and a DT_ResponseClass to be appended to the world.");
	}

	node->setIdx( ++numObjects );
	QVector<PrimitiveObject*>::const_iterator i;
    const QVector<PrimitiveObject*>& primitives = node->data();
    const QVector<PrimitiveObject*>& fieldPrimitives = node-> getFieldPrimitives();
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		//if (verbose) printf("appending robot primitive to world\n");
		DT_SetResponseClass( node->robot()->getResponseTable(), (*i)->getSolidObjectHandle(), node->getResponseClass() );
		DT_RemovePairResponse( node->robot()->getResponseTable(), node->getResponseClass(), node->getResponseClass(), reflexTrigger );
		DT_RemovePairResponse( node->robot()->getResponseTable(), node->getResponseClass(), node->getResponseClass(), collisionHandler );
		
		if ( node->isNearRoot() )
			DT_SetResponseClass( worldTable, (*i)->getSolidObjectHandle(), node->robot()->getWorldBaseClass() );
		else
			DT_SetResponseClass( worldTable, (*i)->getSolidObjectHandle(), node->robot()->getWorldRobotClass() );
		
		DT_AddObject( scene, (*i)->getSolidObjectHandle() );
		if ( modelWindow ) { (*i)->setListPending(true); }
		(*i)->setIdx( ++numPrimitives );
		emit addedPrimitive(*i);
	}
    for ( i=fieldPrimitives.begin(); i!=fieldPrimitives.end(); ++i )
	{
		//if (verbose) printf("appending robot primitive to world\n");
		DT_SetResponseClass( node->robot()->getFieldResponseTable(), (*i)->getSolidObjectHandle(), node->getFieldResponseClass() );
        
		DT_RemovePairResponse( node->robot()->getFieldResponseTable(), node->getFieldResponseClass(), node->getFieldResponseClass(), repel );
		DT_RemovePairResponse( node->robot()->getFieldResponseTable(), node->getFieldResponseClass(), node->getFieldResponseClass(), collisionHandler );
		
		if ( node->isNearRoot() )
			DT_SetResponseClass( worldTable, (*i)->getSolidObjectHandle(), node->robot()->getWorldBaseFieldClass() );
		else
			DT_SetResponseClass( worldTable, (*i)->getSolidObjectHandle(), node->robot()->getWorldFieldClass() );
		
		DT_AddObject( scene, (*i)->getSolidObjectHandle() );
		if ( modelWindow ) { (*i)->setListPending(true); }
		(*i)->setIdx( ++numPrimitives );
		emit addedPrimitive(*i);
	}
	node->setInModel(true);
	world.append(node);
	//printf("Appended KinTreeNode to non-yarp world Model!!!\n");
}

void Model::appendObject( CompositeObject* object )
{
	//if ( verbose ) printf("  appending world object.\n");
	QWriteLocker locker(&mutex);

	if ( !object->getResponseClass() )
	{
		throw KinematicModelException("CompositeObject must have a DT_ResponseClass to be appended to the world.  Use setResponseClass( DT_ResponseClass )");
	}
	
	object->setIdx( ++numObjects );
	const QVector<PrimitiveObject*>& primitives = object->data();
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		//if (verbose) printf("appending primitive to world\n");
		if (object->getResponseClass() != TARGET() && object->getResponseClass() != GHOST()) {
			DT_SetResponseClass( worldTable, (*i)->getSolidObjectHandle(), object->getResponseClass() );
			DT_AddObject( scene, (*i)->getSolidObjectHandle() );
		}
		if ( modelWindow ) { (*i)->setListPending(true); }
		(*i)->setIdx( ++numPrimitives );
		emit addedPrimitive(*i);
	}
	object->setInModel(true);
	world.append(object);
}

bool Model::removeWorldObject( CompositeObject* object )
{
	QWriteLocker locker(&mutex);

	// remove the pointer to the object from the world vector
	QVector<CompositeObject*>::iterator i;
	for ( i=world.end(); i!=world.begin(); )
	{
		--i;
		if ( object == *i )
		{
			if (verbose) printf(" removing object from world vector\n");
			world.erase(i);
			break;
		}
	}
	
	// remove the solid objects from the model
	const QVector<PrimitiveObject*>& primitives = object->data();
	QVector<PrimitiveObject*>::const_iterator j;
	for ( j=primitives.end(); j!=primitives.begin(); )
	{
		--j;
		if (verbose) printf(" removing primitive from the SOLID world\n");
		DT_RemoveObject( scene, (*j)->getSolidObjectHandle() );
		emit removedPrimitive( (*j)->getDisplayList() );
	}
	
	object->setInModel(false); // allow primitives to be edited once again
	
	return true;
}

QVector< QString > Model::listWorldObjects()
{
	QReadLocker locker(&mutex);

	//uint count = 0;
	QVector< QString > list;
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); i++ )
	{
		//printf("world item: %d", count++ );
		KinematicModel::KinTreeNode* n = dynamic_cast<KinematicModel::KinTreeNode*> (*i);
		if( !n )
		{
			//printf(" - appending name '%s'", (*i)->getName().toStdString().c_str() );
			
			list.append( (*i)->getName() );
			//const QVector<PrimitiveObject*>& primitives = (*i)->data();
			//QVector<PrimitiveObject*>::const_iterator j;
			//for ( j=primitives.begin(); j!=primitives.end(); ++j )
			//{
			//	list.append( (*j)->getName() );
			//}
		}
		//printf("\n");
	}
	return list;
}


void Model::grabObject( CompositeObject* object, Robot* robot, int markerIndex ) {
	QReadLocker locker(&mutex);
	objectMover->grabObject( object, robot, markerIndex );
}


CompositeObject* Model::getObject( const QString& _name )
{
	QReadLocker locker(&mutex);
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); i++ )
	{
		if( (*i)->getName().compare( _name ) == 0 ) // if there's a match
		{
			return *i;
		}
	}
	return NULL;
}


Robot* Model::getRobot( const QString& _name )
{
	QReadLocker locker(&mutex);
	QVector<Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); i++ )
	{
		if( (*i)->getName().compare( _name ) == 0 ) // if there's a match
		{
			return *i;
		}
	}
	return NULL;
}


void Model::cleanTheWorld()
{
	QVector<CompositeObject*>::iterator i;
	for ( i=world.end(); i!=world.begin(); )
	{
		--i;
		if ( (*i)->isDead() )
		{
			if (verbose) printf("found dead object: %s\n", (*i)->getName().toStdString().c_str());
			
			// check whether OpenGL is about to access this CompositeObject
			bool displayListPending = false;
			const QVector<PrimitiveObject*>& primitives = (*i)->data();
			QVector<PrimitiveObject*>::const_iterator j;
			for ( j=primitives.begin(); j!=primitives.end(); ++j )
			{
				if ( (*j)->listIsPending() )
				{
					if (verbose) printf(" primitive has pending display list\n");
					displayListPending = true;
					break;
				}
			}
			
			if ( !displayListPending )
			{
				//if (verbose) printf(" no pending display lists\n");
				CompositeObject* dyingObject = *i;	// get a non-iterator ref to the object in question
				removeWorldObject( dyingObject );	// this would f*** up the iterator passed to it because we call QVector::erase( <T> ) inside here
				objectMover->check();				// check objectMover lists and remove object if necessary
				delete( dyingObject );				// and delete the object from memory
			}
			//else
			//{
			//	if (verbose) printf(" waiting to delete object\n");
			//}
		}
	}
}

void Model::clearTheWorld()
{
	QReadLocker locker(&mutex);
	QVector<CompositeObject*>::iterator i;
	for ( i=world.end(); i!=world.begin(); )
	{
		--i;
		KinTreeNode* node = dynamic_cast<KinTreeNode*>(*i);
		if ( !node ) { (*i)->kill(); }
	}
}


void Model::clearWorldObject(CompositeObject* object)
{
	QReadLocker locker(&mutex);
	KinTreeNode* node = dynamic_cast<KinTreeNode*>(object);
		if ( !node ) { object->kill(); }
}


/***********************
*** TO RUN THE MODEL ***
***********************/
int Model::computePose()
{
	cleanTheWorld();		// remove stuff that has been flagged for deletion (this will lock the mutex itself)
	
	// Prepare to do collision detection
	col_count = 0;			// reset collision counter
	reflex_col_count = 0;
	encObstacle = false;	// this is used to trigger a collision response controller
	
	computePosePrefix();	// pure virtual function for extra pre-collision-detection computations (like initializing more vars, responding to rpc calls, ect)
	updateWorldState();		// update positions of things in the world
	updateSolid();

    
	evaluateRobotConstraints();
	computeCollisions();
	
	computePoseSuffix();
	
	emit computedState(col_count);
	
	//printf("COMPUTED KINEMATICS AND COLLISION DETECTION: %d collisions, %d reflex collisions\n",col_count, reflex_col_count);
	
	return reflex_col_count;
}

void Model::computePoseSuffix()
{
	QReadLocker locker(&mutex);
	QVector<Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i ) {
		(*i)->publishState();
	}
}

void Model::evaluateRobotConstraints()
{
	//QWriteLocker locker(&mutex);
	if (syncGraphics)
		mutex.lockForWrite(); // the model has to wait untile the GL thread finishes, and the GL thread has to wait until we are finished updating the model
	else
		mutex.lockForRead(); // The model thread doesn't have the wait for the GL thread to finish drawing, and the GL thread reads whatever values we have written so far
	QVector<Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i ) {
		(*i)->evaluateConstraints();
	}
	mutex.unlock();
}

void Model::computeCollisions()
{
	QReadLocker locker(&mutex);
    
    QVector<Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i ) {
		(*i)->resetExtTorque();
	}
    
    DT_Test(scene,worldTable);
    
    QVector<Robot*>::iterator j;
	for ( j=robots.begin(); j!=robots.end(); ++j ) {
		DT_Test(scene,(*j)->getResponseTable());
		DT_Test(scene,(*j)->getFieldResponseTable());
	}
    
	//QVector<DT_RespTableHandle>::iterator j;
	//uint num = 0;
	//for (j=responseTables.begin();j!=responseTables.end();++j) {
	//	DT_Test(scene,*j);
	//}
}

void Model::updateWorldState()
{
	//QWriteLocker locker(&mutex);
	if (syncGraphics)
		mutex.lockForWrite(); // the model has to wait untile the GL thread finishes, and the GL thread has to wait until we are finished updating the model
	else
		mutex.lockForRead(); // The model thread doesn't have the wait for the GL thread to finish drawing, and the GL thread reads whatever values we have written so far
	
	// forward kinematics
	QVector<Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i ) {
		(*i)->updatePose();
	}
	objectMover->update();  // update object positions that are attached to the robots' markers
	mutex.unlock();
}


void Model::updateSolid()
{
	QReadLocker locker(&mutex);
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); ++i ) {
		(*i)->updateSolid();
	}
}


void Model::run()
{
	printf("Starting collision detection thread.\n");
	while ( keepRunning )
	{
		computePose();
		//if ( robots.at(0) && robots.at(0)->isOpen() ) printf("OPEN\n");
		//else printf("CLOSED\n");
		msleep( COLLISION_THREAD_PERIOD ); //#TODO: sleep for COLLISION_THREAD_PERIOD - time_spent_in(computePose())
	}
}

void Model::stop()
{
	QReadLocker locker(&mutex);
	
	keepRunning = false;
	printf("KinematicModel main thread is shutting down ");
	while ( isRunning() )
	{
		printf(".");
		msleep(1);
	}
	printf("\n");
}

/************
 *** SLOTS ***
 ************/
void Model::renderModel()
{
	QReadLocker locker(&mutex);
	
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); ++i )
	{
		(*i)->render();
	}
}
