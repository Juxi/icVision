#include "yarpModel.h"

using namespace VirtualSkin;

YarpModel::YarpModel( bool visualize ) : RobotModel::Model(visualize)
{
	collisionPort.setBottle( bottle );
	worldPort.setWorld( &world );
}
YarpModel::~YarpModel()
{
	if ( collisionPort.isRunning() ) { collisionPort.stop(); }
	if ( worldPort.isRunning() ) { worldPort.stop(); }
}
void YarpModel::collisionHandlerAddendum( RobotModel::PrimitiveObject *object1, RobotModel::PrimitiveObject *object2, const DtCollData *coll_data )
{	
	QString nameA = object1->getName(); 
	nameA.prepend("-");
	nameA.prepend(object1->getParent()->getName());
	QString nameB = object2->getName();
	nameB.prepend("-");
	nameB.prepend(object2->getParent()->getName());
	
	yarp::os::Bottle& collision = this->bottle.addList();
	
	yarp::os::Bottle& part1 = collision.addList();
	part1.addString(nameA.toStdString().c_str());
	part1.addDouble(coll_data->point1[0]);
	part1.addDouble(coll_data->point1[1]);
	part1.addDouble(coll_data->point1[2]);
	
	yarp::os::Bottle& part2 = collision.addList();
	part2.addString(nameB.toStdString().c_str());
	part2.addDouble(coll_data->point2[0]);
	part2.addDouble(coll_data->point2[1]);
	part2.addDouble(coll_data->point2[2]);
}