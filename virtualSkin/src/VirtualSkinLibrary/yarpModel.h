#ifndef REFLEXDETECTOR_H
#define REFLEXDETECTOR_H

#include "yarpStreamPort.h"
#include "worldRpcInterface.h"
#include "model.h"

namespace VirtualSkin {

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

class YarpModel : public RobotModel::Model
{

public:
	YarpModel( bool visualize = false );	//!< Constructs an armed collisionDetector object
	~YarpModel();	//!< Just stops the YARP port if its running

	void setCollisionPortName( const QString& aName ) { collisionPort.setName(aName); }	//!< Sets the name of the YARP stream port where collision info will be published
	void openCollisionPort() { collisionPort.start(); }									//!< Opens the YARP stream port
	void closeCollisionPort() { collisionPort.stop(); }									//!< Closes the YARP stream port
	
	void setWorldRpcPortName( const QString& aName ) { worldPort.setName(aName); }	//!< Sets the name of the YARP stream port where collision info will be published
	void openWorldRpcPort() { worldPort.start(); }									//!< Opens the YARP stream port
	void closeWorldRpcPort() { worldPort.stop(); }									//!< Closes the YARP stream port

	void collisionHandlerAddendum( RobotModel::PrimitiveObject*, RobotModel::PrimitiveObject*, const DtCollData* );

private:

	QString				cPortName, wPortName;	//!< The name of the YARP stream port that broadcasts the collision information
	YarpStreamPort		collisionPort;			//!< The YARP stream port itself
	WorldRpcInterface	worldPort;
	yarp::os::Bottle	bottle;				//!< The bottle, containing the collision information, that is broadcast in the stream every time computePose() is called
};
}

#endif
