#include "yarpModel.h"
//#include "kintreenode.h"

using namespace VirtualSkin;

YarpModel::YarpModel( bool visualize ) : KinematicModel::Model(visualize)
{
	worldRpcInterface.setModel(this);
}
YarpModel::~YarpModel()
{
	collisionPort.close();
	//worldPort.close();
}

//void YarpModel::onStartUp()
//{
//}

void YarpModel::collisionHandlerAddendum( KinematicModel::PrimitiveObject* prim1, KinematicModel::PrimitiveObject* prim2, const DT_CollData *coll_data )
{	
	QString nameA = prim1->getName();		nameA.prepend("-");		nameA.prepend(prim1->getCompositeObject()->getName());
	QString nameB = prim2->getName();		nameB.prepend("-");		nameB.prepend(prim2->getCompositeObject()->getName());
	
	yarp::os::Bottle& collision = collisionBottle.addList();
	yarp::os::Bottle& part1 = collision.addList();
	yarp::os::Bottle& part2 = collision.addList();
	
	KinematicModel::CompositeObject* comp1 = prim1->getCompositeObject();
	KinematicModel::KinTreeNode* node1 = dynamic_cast<KinematicModel::KinTreeNode*>(comp1);
	if ( node1 )
	{
		//part1.addString(nameA.toStdString().c_str());
		part1.addDouble(coll_data->point1[0]);
		part1.addDouble(coll_data->point1[1]);
		part1.addDouble(coll_data->point1[2]);
	}
	else
	{
		if ( comp1->getResponseClass() == OBSTACLE() ) { part1.addString("OBSTACLE"); }
		else if ( comp1->getResponseClass() == TARGET() ) { part1.addString("TARGET"); }
	}
	
	KinematicModel::CompositeObject* comp2 = prim2->getCompositeObject();
	KinematicModel::KinTreeNode* node2 = dynamic_cast<KinematicModel::KinTreeNode*>(comp2);
	if ( node2 )
	{
		//part2.addString(nameB.toStdString().c_str());
		part2.addDouble(coll_data->point2[0]);
		part2.addDouble(coll_data->point2[1]);
		part2.addDouble(coll_data->point2[2]);
	}
	else
	{
		if ( comp2->getResponseClass() == OBSTACLE() ) { part2.addString("OBSTACLE"); }
		else if ( comp2->getResponseClass() == TARGET() ) { part2.addString("TARGET"); }
	}
	
}

void YarpModel::computePosePrefix()
{
	
	yarp::os::Bottle cmd,response;
	worldPort.read(cmd,true);
	if ( cmd.size() > 0 )
	{
		//if (debug) { showBottle(cmd); }
		worldRpcInterface.handler(cmd,response);
		worldPort.reply(response);
		//if (debug) { printf("reply: %s\n",response.toString().c_str()); }
	}
	
	collisionBottle.clear();
}

void YarpModel::computePoseSuffix()
{
	//if ( collisionPort.isOpen() )
	//{
		if ( collisionBottle.size() == 0 ) { collisionBottle.addInt(0); }
		collisionPort.write(collisionBottle);
	//}
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