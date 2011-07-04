#include "model.h"

using namespace RobotModel;

Model::Model( bool visualize ) : robot(NULL), world(NULL), skinWindow(NULL), keepRunning(true), col_count(0), reflexTriggered(false)
{
	qRegisterMetaType< DT_ResponseClass >("DT_ResponseClass");
	
	// initialize the SOLID datastructures for managing collision response
	scene = DT_CreateScene();
	robotTable =  DT_CreateRespTable();  // setup of this response table is handled by KinTreeNode
	worldTable =  DT_CreateRespTable();
	
	// create SOLID Response classes for the world table
	OBSTACLE = DT_GenResponseClass(worldTable);
	TARGET = DT_GenResponseClass(worldTable);
	BODY_PART = DT_GenResponseClass(worldTable);
	
	// FOR SELF_COLLISION
	DT_AddDefaultResponse( robotTable, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	
	// FOR INTERACTION WITH THE ENVIRONMENT
	// anything that hits an 'OBSTACLE' causes the reflexTrigger signal to be emitted
	DT_AddPairResponse( worldTable, BODY_PART, OBSTACLE, reflexTrigger, DT_WITNESSED_RESPONSE, (void*) this );
	
	// anything that hits a 'TARGET' does not (but still computes the witness point)
	DT_AddPairResponse( worldTable, BODY_PART, TARGET, collisionHandler, DT_WITNESSED_RESPONSE, (void*) this );

	// collisions between objects in the robot's environment are ignored completely
	//DT_RemovePairResponse( worldTable,	OBSTACLE,	OBSTACLE,	collisionHandler);
	//DT_RemovePairResponse( worldTable,	TARGET,		TARGET,		collisionHandler);
	//DT_RemovePairResponse( worldTable,	TARGET,		OBSTACLE,	collisionHandler);
	// the robot's self-collisions are also ignored, as they are handled in another response table
	//DT_RemovePairResponse( worldTable,	BODY_PART,	BODY_PART,	collisionHandler);

	// initialize the data-structures that hold the geometries in the model
	robot = new Robot(this);
	QObject::connect( robot, SIGNAL(appendedObject(KinTreeNode*)), this, SLOT(newBodyPartResponseClass(KinTreeNode*)) );
	QObject::connect( robot, SIGNAL(appendedPrimitive(PrimitiveObject*)), this, SLOT(newBodyPart(PrimitiveObject*)) );
	QObject::connect( robot, SIGNAL(removeSelfCollisionPair(DT_ResponseClass,DT_ResponseClass)), this, SLOT(removeRobotTablePair(DT_ResponseClass,DT_ResponseClass)) );
	
	world = new World(this);
	QObject::connect( world, SIGNAL(worldAppendedPrimitive(PrimitiveObject*)), this, SLOT(newObstacle(PrimitiveObject*)) );
	QObject::connect( world, SIGNAL(requestNewResponseClass(PrimitiveObject*,DT_ResponseClass)), this, SLOT(changeResponseClass(PrimitiveObject*,DT_ResponseClass)) );
	QObject::connect( world, SIGNAL(requestRemoveSolidObject(PrimitiveObject*)), this, SLOT(removeObject(PrimitiveObject*)) );
	
	// set up the window for OpenGL
	if ( visualize )
	{ 
		skinWindow = new SkinWindow();
		
		QObject::connect( robot, SIGNAL(requestDisplayList(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( robot, SIGNAL(outdatedDisplayList(int)),						skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		
		QObject::connect( world, SIGNAL(requestDisplayList(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( world, SIGNAL(outdatedDisplayList(int)),						skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		
		QObject::connect( this, SIGNAL(collisions(int)),				skinWindow->glWidget, SLOT(update(int)) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	robot, SLOT(callLists()) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),	world, SLOT(callLists()) );
		
		skinWindow->show();
	}
	
}

Model::~Model()
{
	if ( skinWindow ) { delete skinWindow; }
	DT_DestroyRespTable(worldTable);
	DT_DestroyRespTable(robotTable);
}

void Model::newBodyPartResponseClass( KinTreeNode* node )
{
	//QMutexLocker locker(&mutex);
	node->setSolidResponseClass( DT_GenResponseClass(robotTable) );
}

DT_ShapeHandle Model::createSolidShape( PrimitiveObject* primitive )
{
	// create the appropriate shape object for the primitive at hand
	switch (primitive->getGeomType()) {
		case PrimitiveObject::SPHERE: {
			Sphere* sphere = static_cast<Sphere*>(primitive);
			return DT_NewSphere(sphere->r());
		}
		case PrimitiveObject::CYLINDER: {
			Cylinder* cylinder = static_cast<Cylinder*>(primitive);
			return DT_NewCylinder(cylinder->r(), cylinder->h());
		}
		case PrimitiveObject::BOX: {
			Box* box = static_cast<Box*>(primitive);
			return DT_NewBox(box->getX(), box->getY(), box->getZ());
		}
		default:
			return NULL;
	}
}
void Model::newBodyPart( PrimitiveObject* primitive )
{
	//QMutexLocker locker(&mutex);
	
	primitive->setShape( createSolidShape( primitive ) );
	primitive->setObject( DT_CreateObject( primitive, primitive->getShape()) );
	DT_AddObject( scene, primitive->getSolidHandle() );
	DT_SetResponseClass( robotTable, primitive->getSolidHandle(), primitive->getParent()->getSolidResponseClass() );
	DT_SetResponseClass( worldTable, primitive->getSolidHandle(), body_partResponseClass() );
}

void Model::newObstacle( PrimitiveObject* primitive )
{
	//QMutexLocker locker(&mutex);
	
	primitive->setShape( createSolidShape( primitive ) );
	primitive->setObject( DT_CreateObject( primitive, primitive->getShape()) );
	DT_AddObject( scene, primitive->getSolidHandle() );
	DT_SetResponseClass( worldTable, primitive->getSolidHandle(), obstacleResponseClass() );
}

void Model::changeResponseClass( PrimitiveObject* primitive, DT_ResponseClass responseClass )
{
	//QMutexLocker locker(&mutex);
	DT_SetResponseClass( worldTable, primitive->getSolidHandle(), responseClass );
}

void Model::removeRobotTablePair( DT_ResponseClass objType1, DT_ResponseClass objType2 )
{
	//QMutexLocker locker(&mutex);
	DT_RemovePairResponse( robotTable, objType1, objType2, reflexTrigger);
}

void Model::removeObject( PrimitiveObject* primitive)
{
	QMutexLocker locker(&mutex);
	DT_RemoveObject( scene, primitive->getObject() );
	DT_DestroyObject( primitive->getObject() );
	DT_DeleteShape( primitive->getShape() );
}

int Model::computePose()
{
	QMutexLocker locker(&mutex);
	
	/**/if ( !robot->isOpen() )
	{
		printf("COLLISION DETECTOR ERROR: the robot has not been configured...  call robot.open(QString) before computePose()...\n *** DO YOU HAVE THE RIGHT ROBOT NAME?!? ***\n");
		return 0;
	}
	
	// Prepare to do collision detection
	col_count = 0;			// reset collision counter
	reflexTriggered = false;
	computePosePrefix();	// pure virtual function for extra pre-collision-detection computations
	
	robot->updatePose();		// do forward kinematics
	world->update();

	DT_Test(scene,robotTable);
	DT_Test(scene,worldTable);
	
	// send the results of DT_TEST as Qt Signals
	if ( reflexTriggered )
	{
		//printf("EMITTING REFLEX SIGNAL!!! \n");
		emit reflexResponse();
	}
	emit collisions(col_count); // this number may not be correct as we stop DT_TEST early in the case of reflexive response
	
	//if ( col_count ) { printf("%i Collisions!\n", col_count); }
	//printf("called compute pose \n");
	
	computePoseSuffix();

	return col_count;
}

void Model::run()
{	
	while ( keepRunning )
	{
		computePose();
		msleep( period );
	}
}

void Model::stop()
{
	keepRunning = false;
	while ( isRunning() ) {}
}
