#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <QObject>
#include <SOLID/solid.h>
#include "primitiveobject.h"
#include "robot.h"
#include "world.h"
#include "yarpStreamPort.h"
#include "robotmodelexception.h"

/*! \brief Computes the current pose of the Robot and does collision detection using the FreeSOLID library
 *
 * To use this class:
 *   First prepare the object by calling the constructor, setRobot(Robot*) and setWorld(World*).
 *   If you want to publish collision information on the network via YARP, call setPortName(QString&) and openPort().
 *   Call/activate the slot computePose() when you suspect the robot's state has changed.
 *   When the pose has been computed and collision detection is finished, the signal newPoseReady() will be emitted.
 *   If one or more collisions have occurred, the signal crash() will also be emitted, provided that the CollisionDetector is armed.
 *	 Finally, use the slots armDetector() and disarmDetector() to control whether or not to emit the crash() signal if a collision has occurred.
 *   This is useful if the crash() signal triggers some kind of collision handling control code, as you can supress subsequent crash signals until the collision handling is complete.
 *
 *	 NOTE: Collision detection is only carried out between the robot and itself O(n^2) where n=|robot|, as well as between the robot and the world O(n) where n=|world|.
 *	 Thus the computational complexity of the collision detection grows linearly with the size of the world. 
 */

class CollisionDetector : public QObject
{
    Q_OBJECT

public:
	CollisionDetector();	//!< Constructs an armed collisionDetector object
	~CollisionDetector();	//!< Just stops the YARP port if its running

	void setRobot( RobotModel::Robot* aRobot ) { robot = aRobot; }	//!< Binds this collision detector to a robot model
	void setWorld( RobotModel::World* aWorld ) { world = aWorld; }	//!< Binds this collision detector to a world model
	
	void setPortName( const QString& aName ) { solidPort.setName(aName); }	//!< Sets the name of the YARP stream port where collision info will be published
	void openPort();														//!< Opens the YARP stream port
	void closePort() { solidPort.stop(); }									//!< Closes the YARP stream port

	bool detectorIsArmed() { return isArmed; }								//!< Returns whether or not the collisionDetector is armed
	
	/*! This callback is executed whenever FreeSOLID encounters a collision between Objects.
	 It sets a flag that collision(s) has/have occurred, and encodes the collision state into a YARP bottle.
	 */
	static void collisionHandler( void * client_data, DtObjectRef obj1, DtObjectRef obj2, const DtCollData *coll_data )
	{
		RobotModel::PrimitiveObject* object1 = (RobotModel::PrimitiveObject*)obj1;
		RobotModel::PrimitiveObject* object2 = (RobotModel::PrimitiveObject*)obj2;
		object1->setColliding();
		object2->setColliding();
		
		CollisionDetector* detector = (CollisionDetector*)client_data;
		
		QString name1 = object1->getName(); 
				name1.prepend("-");
				name1.prepend(object1->getParent()->getName());
		QString name2 = object2->getName();
				name2.prepend("-");
				name2.prepend(object2->getParent()->getName());
		
		Bottle& collision = detector->bottle.addList();
		
		Bottle& part1 = collision.addList();
		part1.addString(name1.toStdString().c_str());
		part1.addDouble(coll_data->point1[0]);
		part1.addDouble(coll_data->point1[1]);
		part1.addDouble(coll_data->point1[2]);
		
		Bottle& part2 = collision.addList();
		part2.addString(name2.toStdString().c_str());
		part2.addDouble(coll_data->point2[0]);
		part2.addDouble(coll_data->point2[1]);
		part2.addDouble(coll_data->point2[2]);
			
		detector->crashed = true;
		detector->col_count++;
	}

signals:
   
    void newPoseReady(); //!< Signifies that a new pose (in cartesian space) has been computed
	void crash();		 //!< Signifies that a collision(s) has/have occurred
	
public slots:
	void armDetector();							//!< When armed, the collisionDetector will emit the crash() signal if collision(s) occurr(s)
	void disarmDetector() { isArmed = false; }	//!< When disarmed, the collisionDetector will not emit the crash() signal regardless
	bool computePose();							//!< Causes the Robot's pose to be computed in cartesian space and the collision detection to be run using FreeSOLID

private:
    RobotModel::Robot*	robot;			//!< The Robot whose pose we are updating
	RobotModel::World*  world;			//!< All the other Objects not belonging to the Robot's body. (we just need this so we can change colors when things collide or not)
    
	int     col_count;		//!< The number of collisions in the current robot/world configuration
    bool    isArmed;		//!< Whether or not to report collisions
	bool	crashed;		//!< Whether or not the current configuration has at least one pair of colliding objects
	
	QString portName;			//!< The name of the YARP stream port that broadcasts the collision information
	YarpStreamPort solidPort;	//!< The YARP stream port itself
	Bottle bottle;				//!< The bottle, containing the collision information, that is broadcast in the stream every time computePose() is called
};

#endif // COLLISIONDETECTOR_H
