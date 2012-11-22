/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef JOINT_H
#define JOINT_H

#include "interval.h"
#include "kintreenode.h"

namespace KinematicModel
{
	class Joint;
	class Motor;
}

/** \brief Defines a kinematic 'joint'
 *
 * This joint class is intended to be a building block for a kinematic model that is embedded in a tree structure, and therefore inherits KinTreeNode.
 */
class KinematicModel::Joint : public KinTreeNode
{
public:
    Joint( /*DT_ResponseClass nodeResponseClass,*/ Robot* robot, int part, KinTreeNode* parent, Motor* motor, NodeType type );	//!< Simply initializes member variables and calls KinTreeNode constructor
																			//!< \param robot The robot to which this joint belongs
																			//!< \param parent The parent KinTreeNode in the kinematic tree structure
																			//!< \param motor The motor that controls this joint
																			//!< \param type Indicates whether the Joint is RevoluteJoint or PrismaticJoint
    virtual ~Joint();														//!< Nothing special to do here (hierarchy is handled by KinTreeNode)

    void setMin( qreal pos );	//!< Sets the minimum physical joint position (radians)
								/**< The joint limits are stored in an Interval, which ensures that min <= max */
    void setMax( qreal pos );	//!< Sets the maximum physical joint position (radians)
								/**< The joint limits are stored in an Interval, which ensures that min <= max */
    void setPos();				//!< Sets the physical position of the joint.
								/**< The position is computed as a function of the position of the Motor. If the Joint's Motor is null, 
									 position is set to 0. If 0 is out of the range of the joint limits, the position is set to 
									 limits.getMin() or limits.getMax() accordingly. */
    bool isBodyPartRoot();
    
    Motor* parentMotor() { return motor; }
    
protected:
	
    Motor*   motor;				//!< The motor that controls this joint
    qreal    position;			//!< The current joint position
    Interval limits;			//!< The physical limits of the joint position (radians)

    virtual void setM() = 0;	//!< Set the transformation matrix from the next CS in the kinematic tree (that of the child KinTreeNode) to this CS
};

#endif

/** @} */
