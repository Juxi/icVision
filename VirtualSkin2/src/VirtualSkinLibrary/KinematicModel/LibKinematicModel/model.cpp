#include "model.h"
#include "robot.h"
#include "worldhandler.h"
#include "modelwindow.h"

using namespace KinematicModel;

Model::Model( bool visualize, bool verb ) : keepRunning(true), col_count(0), encObstacle(false), verbose(verb), modelWindow(NULL)
{
	qRegisterMetaType< DT_ResponseClass >("GL_DisplayList");
	//qRegisterMetaType< DT_ResponseClass >("DT_ObjectHandle");
	//qRegisterMetaType< DT_ResponseClass >("DT_ShapeHandle");
	
	// initialize the SOLID datastructures for managing collision response
	scene = DT_CreateScene();
	responseTables.append(DT_CreateRespTable());
	
	// create SOLID Response classes for the world table
	obstacleClass = DT_GenResponseClass(responseTables.at(0));
	targetClass = DT_GenResponseClass(responseTables.at(0));
	robotClass = DT_GenResponseClass(responseTables.at(0));
	//worldCriticalClass = DT_GenResponseClass(responseTables.at(0));
	//robotCriticalClass = DT_GenResponseClass(responseTables.at(0));

	// define some collision responses for the world table
	DT_AddPairResponse(	responseTables.at(0), robotClass, obstacleClass, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	DT_AddPairResponse(	responseTables.at(0), robotClass, targetClass, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );
	
	// set up the window for OpenGL
	if ( visualize )
	{ 
		printf("CREATING WINDOW AND CONNECTING SIGNALS AND SLOTS\n");
		
		modelWindow = new ModelWindow();
		
		QObject::connect( this, SIGNAL(addedPrimitive(PrimitiveObject*)),	modelWindow->glWidget, SLOT(addDisplayList(PrimitiveObject*)) );
		QObject::connect( this, SIGNAL(removedPrimitive(GL_DisplayList)),	modelWindow->glWidget, SLOT(removeDisplayList(GL_DisplayList)) );
		QObject::connect( this, SIGNAL(computedState(int)),					modelWindow->glWidget, SLOT(update(int)) );
		QObject::connect( modelWindow->glWidget, SIGNAL(renderStuff()),		this, SLOT(renderWorld()), Qt::DirectConnection );

		//QObject::connect( robot, SIGNAL(requestDisplayList(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		//QObject::connect( robot, SIGNAL(outdatedDisplayList(int)),						skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		//QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	this, SLOT(renderRobot()) );
		
		modelWindow->show();
		 
	}
	
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
		
	if ( modelWindow ) { delete(modelWindow); }
	
	QVector<DT_RespTableHandle>::iterator j;
	for ( j=responseTables.begin(); j!=responseTables.end(); ++j ) {
		DT_DestroyRespTable(*j);
	}
}

/****************
*** MODEL I/O ***
****************/
DT_RespTableHandle Model::newRobotTable()
{
	QMutexLocker locker(&mutex);
	printf("Creating Robot Table\n");
	DT_RespTableHandle table = DT_CreateRespTable();
	DT_AddDefaultResponse( table, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	responseTables.append( table );
	return table;
}

void Model::removePairResponse( DT_RespTableHandle t, DT_ResponseClass c1, DT_ResponseClass c2 )
{
	QMutexLocker locker(&mutex);
	DT_RemovePairResponse(t, c1, c2, reflexTrigger);
}

DT_ResponseClass Model::newResponseClass( DT_RespTableHandle table )
{
	QMutexLocker locker(&mutex);
	if (verbose) printf("Creating Robot Response Class\n");
	DT_ResponseClass respClass = DT_GenResponseClass(table);
	return respClass;
}

Robot* Model::loadRobot( const QString& fileName, bool verbose )
{
	DT_RespTableHandle newTable = newRobotTable();
	Robot* robot = new Robot( this, newTable );
	robot->open( fileName, verbose );
	robot->home( verbose );
	
	responseTables.append( newTable );
	robots.append( robot );
	return robot;
}

void Model::loadWorld( const QString& fileName, bool verbose )
{
	printf("Loading world file...\n");
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
}

void Model::appendObject( KinTreeNode* node )
{
	if ( verbose ) printf("  appending robot object.\n");
	QMutexLocker locker(&mutex);
	if ( !node->getResponseClass() || !node->robot()->getResponseTable() )
	{
		throw KinematicModelException("CompositeObject must have a DT_RespTableHandle and a DT_ResponseClass to be appended to the world.");
	}

	const QVector<PrimitiveObject*>& primitives = node->data();
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		if (verbose) printf("appending robot primitive to world\n");
		
		DT_SetResponseClass(	node->robot()->getResponseTable(), (*i)->getSolidObjectHandle(), node->getResponseClass() );
		DT_RemovePairResponse(	node->robot()->getResponseTable(), node->getResponseClass(), node->getResponseClass(), reflexTrigger );
		
		DT_SetResponseClass(	responseTables.at(0), (*i)->getSolidObjectHandle(), robotClass );
		//DT_RemovePairResponse(	responseTables.at(0), robotClass, robotClass, reflexTrigger );
		
		DT_AddObject( scene, (*i)->getSolidObjectHandle() );
		if ( modelWindow ) { (*i)->setListPending(true); }
		emit addedPrimitive(*i);
	}
	node->setInModel(true);
	world.append(node);
}

void Model::appendObject( CompositeObject* object )
{
	if ( verbose ) printf("  appending world object.\n");
	QMutexLocker locker(&mutex);
	if ( !object->getResponseClass() )
	{
		throw KinematicModelException("CompositeObject must have a DT_ResponseClass to be appended to the world.  Use setResponseClass( DT_ResponseClass )");
	}
	
	const QVector<PrimitiveObject*>& primitives = object->data();
	QVector<PrimitiveObject*>::const_iterator i;
	for ( i=primitives.begin(); i!=primitives.end(); ++i )
	{
		if (verbose) printf("appending primitive to world\n");
		
		//if (object->g
		//DT_SetResponseClass( robotTable, (*i)->getSolidObjectHandle(), object->getResponseClass() );
		DT_SetResponseClass( responseTables.at(0), (*i)->getSolidObjectHandle(), object->getResponseClass() );
		//if ( object->getRobotResponseClass() )
		//{
		//	DT_SetResponseClass( robotTable, (*i)->getSolidObjectHandle(), object->getRobotResponseClass() );
		//}
		
		DT_AddObject( scene, (*i)->getSolidObjectHandle() );
		if ( modelWindow ) { (*i)->setListPending(true); }
		emit addedPrimitive(*i);
	}
	object->setInModel(true);
	world.append(object);
}

CompositeObject* Model::removeWorldObject( CompositeObject* object )
{
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
	
	return object;
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
				if (verbose) printf(" no pending display lists\n");
				CompositeObject* dyingObject = *i;	// get a non-iterator ref to the object in question
				removeWorldObject( dyingObject );	// this would f*** up the iterator passed to it because we call QVector::erase( <T> ) inside here
				delete( dyingObject );				// and delete the object from memory
			}
			else
			{
				if (verbose) printf(" waiting to delete object\n");
			}
		}
		
	}
}

/***********************
*** TO RUN THE MODEL ***
***********************/
int Model::computePose()
{
	QMutexLocker locker(&mutex);
	
	cleanTheWorld();		// remove stuff that has been flagged for deletion
	updateWorldState();		// update positions of things in the world

	// Prepare to do collision detection
	col_count = 0;			// reset collision counter
	encObstacle = false;	// this is used to trigger a collision response controller
	computePosePrefix();	// pure virtual function for extra pre-collision-detection computations (like initializing more vars, responding to rpc calls, ect)

	QVector<DT_RespTableHandle>::iterator i;
	for (i=responseTables.begin();i!=responseTables.end();++i)
	{
		DT_Test(scene,*i);
	}
	//DT_Test(scene,worldTable);
	
	computePoseSuffix();
	
	// send the results of DT_TEST as Qt Signals
	if ( encObstacle )
	{
		//printf("EMITTING REFLEX SIGNAL!!! \n");
		emit encounteredObstacle();
	}
	emit computedState(col_count); // this number may not be correct as we stop DT_TEST early in the case of reflexive response
	
	return col_count;
}

void Model::fwdKin()
{
	QVector<Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i )
	{
		(*i)->updatePose();
	}
}

void Model::updateWorldState()
{
	fwdKin();
	
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); ++i )
	{
		(*i)->updateSolid();
	}
}

void Model::run()
{	
	while ( keepRunning )
	{
		computePose();
		msleep( COLLISION_THREAD_PERIOD );
	}
}

void Model::stop()
{
	QMutexLocker locker(&mutex);
	
	keepRunning = false;
	printf("\n*** KinematicModel main thread is shutting down ***\n");
	while ( isRunning() )
	{
		printf(".");
		msleep(1);
	}
	printf("\n\n");
}

/************
 *** SLOTS ***
 ************/
void Model::renderWorld()
{
	QMutexLocker locker(&mutex);
	
	QVector<CompositeObject*>::iterator i;
	for ( i=world.begin(); i!=world.end(); ++i )
	{
		(*i)->render();
	}
}

//void Model::displayListArrived( PrimitiveObject* primitive )
//{
//	printf("displayListArrived()\n");
//	primitive->setListPending(false);
//}
