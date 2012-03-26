#include "yarprobot.h"

using namespace VirtualSkin;

YarpRobot::YarpRobot( KinematicModel::Model* m, DT_RespTableHandle t, DT_ResponseClass c, DT_ResponseClass b ) : KinematicModel::Robot(m,t,c,b)
{
}
YarpRobot::~YarpRobot()
{
	observationPort.close();
	collisionPort.close();
}

void YarpRobot::clearStateBottles()
{
	collisionBottle.clear();
	observationBottle.clear();
}

void YarpRobot::publishState()
{
	KinematicModel::Robot::publishState();
	
	//if ( collisionPort.isOpen() )
	//{
		if ( collisionBottle.size() == 0 ) { collisionBottle.addInt(0); }
		//collisionBottle.addString("***");
		collisionPort.write(collisionBottle);
	//}
	
	//if ( observationPort.isOpen() )
	//{
		KinematicModel::RobotObservation obs = observe();
		for ( uint i = 0; i < obs.getNumMarkers(); i++ )
		{
			yarp::os::Bottle& thisObs = observationBottle.addList();
			thisObs.addString( obs.markerName(i).toStdString().c_str() );
			for ( int j = 0; j < 16; j++ )
			{
				thisObs.addDouble( obs.markerConfiguration(i).data()[j] );
			}
		}
		observationPort.write(observationBottle);
	//}
}

void YarpRobot::addCollisionData( int idx1, double x1, double y1, double z1,
								   int idx2, double x2, double y2, double z2 )
{
	// this is a self collision
	yarp::os::Bottle& thisColl = collisionBottle.addList();
	yarp::os::Bottle& a = thisColl.addList();
	yarp::os::Bottle& b = thisColl.addList();
	a.addInt(idx1);		b.addInt(idx2);
	a.addDouble(x1);	b.addDouble(x2);
	a.addDouble(y1);	b.addDouble(y2);
	a.addDouble(z1);	b.addDouble(z2);
}

void YarpRobot::addCollisionData( int idx, double x, double y, double z, char* s )
{
	// this is a self collision
	yarp::os::Bottle& thisColl = collisionBottle.addList();
	thisColl.addInt(idx);
	thisColl.addDouble(x);
	thisColl.addDouble(y);
	thisColl.addDouble(z);
	thisColl.addString(s);
}