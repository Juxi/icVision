/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "joint.h"
#include "interval.h"
#include "modelexception.h"

namespace KinematicModel { class Motor; }

/*! \brief Affords control of one or more joints (see Joint, RevoluteJoint and PrismaticJoint)
 *
 * This class can control an arbitrary number of joints via a linear mapping between the motor position and each joint
 * position. The mapping is implicitly defined by the intervals motor.limits and joint.limits, and it works as follows:
 * The motor position is set and stored in motor.encoderPosition. Using the interval motor.limits, the encoder position
 * is normalized to a value on the interval [0,1] and this value is stored in motor.normalPosition. Finally, the normalized
 * motor position is used to interpolate the limits of each joint to which the motor is bound. The motor object inherits
 * QVector<Joint*>, and in this way it is its own list of joints. When a motor position is set, it immediately sets all
 * relevent joint positions.
 */

class KinematicModel::Motor : public QVector<Joint*>
{
	
public:
	
	Motor( Robot* robot, Motor* motor = 0 );				//!< \brief Appends the new motor to the Robot and initializes member vars
															/**< \param robot The robot to which this BodyPart should be appended
																 \param motor Facilitates robot creation via the Qt XML parser and ZPHandler
																 Calls Robot.appendMotor( Motor* this ), initializes the Motor.limits to [-1,1],
																 and initializes encoderPosition = 0.0, normalPosition = 0.5, and homePosition = 0.0
															 */
	~Motor();												//!< Nothing to destroy here

	const QString& name() const { return motorName; }		//!< Returns a human readable name for the motor
	Motor* parent() const { return parentMotor; }			//!< Returns a pointer to the parent Motor
															/**< This is currently used only by ZPHandler for building the robot from a hierarchical
																 XML file. There is nothing inherently heirarchical about a group of Motor objects.
																 It is the underlying KinTreeNode objects (Link, RevoluteJoint and PrismaticJoint) that
																 reflect the hierarchy of the kinematic tree */
	qreal minPos() const { return motorLimits.getMin(); }	//!< Returns the minimum encoderPosition
	qreal maxPos() const { return motorLimits.getMax(); }	//!< Returns the maximum encoderPosition
	qreal encPos() const { return encoderPosition; }		//!< Returns the current encoderPosition
	qreal normPos() const { return normalPosition; }		//!< Returns the current normalPosition
	qreal homePos() const { return homePosition; }			//!< Returns the current homePosition
	qreal normHomePos() const { return (homePosition - minPos()) / (maxPos() - minPos()); }			//!< Returns the current homePosition


	void print();											//!< Prints the Motor's member variables to the terminal

	void setName( const QString& name ) { motorName = name; }
	void setMin( qreal );										//!< Sets the minimum encoderPosition
																/**< If the new minimum is greater than homePosition and/or encoderPosition, then
																	 they will be reset to the new minimum as needed */
	void setMax( qreal );										//!< Sets the maximum encoderPosition
																/**< If the new maximum is less than homePosition and/or encoderPosition, then
																	 they will be reset to the new maximum as needed */
	void setEncPos( qreal );									//!< Sets the current encoderPosition
																/**< If the requested encoderPosition is outside the interval motorLimits, either
																	 either the maximum or minimum value will be used as is appropriate */ 
	void setHomePos( qreal );									//!< Sets the homePosition (the position in which the real robot hardware calibrates)
																/**< If the requested homePosition is outside the interval motorLimits, either
																	 either the maximum or minimum value will be used as is appropriate */ 
	void setNormPos( qreal );									//!< Sets the normalPosition directly
																/**< Values outside the interval [0,1] will be replaced by either 0 or 1 as is appropriate */
	void home(bool verbose = true);								//!< Sets encoderPosition = homePosition


	qreal normToEnc( qreal );										//!< Converts normalized encoder to real encoder value
	qreal encToNorm( qreal );										//!< Converts real encoder to normalized encoder value

    bool isBodyPartRoot() { return bodyPartRoot; }
    
    qreal getTorque() { return torque; }
    void addTorque( qreal t ) { torque += t; }
    void resetTorque() { torque = 0.0; }
    
    qreal getSpring() { return spring; }
    void setSpring( qreal s ) { spring = s; }
    
private:
	
    bool        bodyPartRoot;
    qreal       torque,spring;
	Motor*		parentMotor;		//!< Just for the XML parser
	QString		motorName;			//!< A human readable name 
	Interval	motorLimits;        //!< Encoder position limits
	qreal		encoderPosition,	//!< The position of the motor encoder... is always on the interval 'limits'
				homePosition,		//!< The position in which the real robot (hardware) calibrates... always on the interval 'limits'
				normalPosition;		//!< Normalized encoder position... on the interval [0,1]
	
	void setJointPositions();		//!< Update the positions of the joints belonging to this motor
    
    friend class ZPHandler;	
};

#endif 

/** @} */
