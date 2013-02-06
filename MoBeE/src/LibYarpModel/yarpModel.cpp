#include "yarpModel.h"
#include "yarprobot.h"

using namespace MoBeE;

YarpModel::YarpModel( bool visualize ) : KinematicModel::Model(visualize)//, rpcIsOpen(false)
{
	worldRpcInterface.setModel(this);
	simSyncer.setModel(this);
	//worldPort.setReader(rpcReader);
}
YarpModel::~YarpModel()
{
	//worldRpcInterface.close();
	//simSyncer.close();
}

//void YarpModel::onStartUp()
//{
//}

YarpRobot* YarpModel::loadYarpRobot( const QString& fileName, bool verbose )
{
    mutexData.lockForWrite();
    
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
    YarpRobot* robot = new YarpRobot( this,
                                     newTable,
                                     newFieldTable,
                                     newRobotClass,
                                     newBaseClass,
                                     newFieldClass,
                                     newBaseFieldClass );
	mutexData.unlock();

    robot->open( fileName, verbose ); // open calls appendObject, which locks the mutex by itself
	
	mutexData.lockForWrite();
    robots.append( robot );
	mutexData.unlock();
		
	// these are normal objects not KintreeNodes.  Appending them locks the model mutex
	robot->appendMarkersToModel();
	
	return robot;
}

void YarpModel::collisionHandlerAddendum( KinematicModel::PrimitiveObject* prim1, KinematicModel::PrimitiveObject* prim2, const DT_CollData *coll_data )
{	
	//QString nameA = prim1->getName();		nameA.prepend("-");		nameA.prepend(prim1->getCompositeObject()->getName());
	//QString nameB = prim2->getName();		nameB.prepend("-");		nameB.prepend(prim2->getCompositeObject()->getName());
	
	//printf("called collision handler addendum\n");
	
	KinematicModel::CompositeObject* comp1 = prim1->getCompositeObject();
	KinematicModel::CompositeObject* comp2 = prim2->getCompositeObject();
	KinematicModel::KinTreeNode* node1 = dynamic_cast<KinematicModel::KinTreeNode*>(comp1);
	KinematicModel::KinTreeNode* node2 = dynamic_cast<KinematicModel::KinTreeNode*>(comp2);
	
	if ( node1 && node2 )
	{
		if ( node1->robot() == node2->robot() )
		{
			YarpRobot* r = dynamic_cast<YarpRobot*> (node1->robot());
			if (r) r->addCollisionData( prim1->idx() , coll_data->point1[0], coll_data->point1[1], coll_data->point1[2],
										prim2->idx(), coll_data->point2[0], coll_data->point2[1], coll_data->point2[2]  );
		}
		else
		{
			YarpRobot* r1 = dynamic_cast<YarpRobot*> (node1->robot());
			if (r1) r1->addCollisionData( prim1->idx(), coll_data->point1[0], coll_data->point1[1], coll_data->point1[2], "ROBOT" );
			
			YarpRobot* r2 = dynamic_cast<YarpRobot*> (node2->robot());
			if (r2) r2->addCollisionData( prim2->idx(), coll_data->point2[0], coll_data->point2[1], coll_data->point2[2], "ROBOT" );
		}
	}
	else if ( node1 )
	{
		if ( comp2->getResponseClass() == OBSTACLE() )
		{
			YarpRobot* r = dynamic_cast<YarpRobot*> (node1->robot());
			if (r) r->addCollisionData( prim1->idx(), coll_data->point1[0], coll_data->point1[1], coll_data->point1[2], "OBSTACLE" );
		}
		else if ( comp2->getResponseClass() == TARGET() )
		{
			YarpRobot* r = dynamic_cast<YarpRobot*> (node1->robot());
			if (r) r->addCollisionData( prim1->idx(), coll_data->point1[0], coll_data->point1[1], coll_data->point1[2], "TARGET" );
		}
	}
	else if ( node2 )
	{
		if ( comp1->getResponseClass() == OBSTACLE() )
		{
			YarpRobot* r = dynamic_cast<YarpRobot*> (node2->robot());
			if (r) r->addCollisionData( prim2->idx(), coll_data->point2[0], coll_data->point2[1], coll_data->point2[2], "OBSTACLE" );
		}
		else if ( comp1->getResponseClass() == TARGET() )
		{
			YarpRobot* r = dynamic_cast<YarpRobot*> (node2->robot());
			if (r) r->addCollisionData( prim2->idx(), coll_data->point2[0], coll_data->point2[1], coll_data->point2[2], "TARGET" );
		}
	}
}

void YarpModel::computePosePrefix()
{
	// clear the yarp::bottles of residual observations and collisions
	QVector<KinematicModel::Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i )
	{
		YarpRobot* r = dynamic_cast<YarpRobot*>(*i);
		if (r) { r->clearStateBottles(); }
	}
}

void YarpModel::computePoseSuffix()
{
	//KinematicModel::computePoseSuffix();
	QVector<KinematicModel::Robot*>::iterator i;
	for ( i=robots.begin(); i!=robots.end(); ++i )
	{
		(*i)->publishState();
	}
}

void YarpModel::showBottle( yarp::os::Bottle& anUnknownBottle, int indentation)
{
    for (int i=0; i<anUnknownBottle.size(); i++) {
        for (int j=0; j<indentation; j++) { printf(" "); }
        printf("[%d]: ", i);
        yarp::os::Value& element = anUnknownBottle.get(i);
        switch (element.getCode()) {
			case BOTTLE_TAG_INT:
				printf("int %d\n", element.asInt());
				break;
			case BOTTLE_TAG_DOUBLE:
				printf("float %g\n", element.asDouble());
				break;
			case BOTTLE_TAG_STRING:
				printf("string \"%s\"\n", element.asString().c_str());
				break;
			case BOTTLE_TAG_BLOB:
				printf("binary blob of length %d\n", element.asBlobLength());
				break;
			case BOTTLE_TAG_VOCAB:
				printf("vocab [%s]\n", yarp::os::Vocab::decode(element.asVocab()).c_str());
				break;
			default:
				if (element.isList()) {
					yarp::os::Bottle *lst = element.asList();
					printf("list of %d elements\n", lst->size());
					showBottle(*lst,indentation+2);
				} else {
					printf("unrecognized type\n");
				}
				break;
        }
    }
}
