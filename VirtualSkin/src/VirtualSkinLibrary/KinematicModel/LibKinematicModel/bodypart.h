/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef BODYPART_H
#define BODYPART_H
 
#include "zphandler.h"
#include "motor.h"

/** \brief Contains classes relevant to the robot model.
 *
 * The CMake project RobotModel in YOUR_GIT_REPOSITORY/virtualSkin/src/VirtualSkinLibrary/RobotModel can be built independantly to produce the static library libRobotModel.a. The project requires:
 *		- Qt (4.x)
 *		- OpenGL
 *		- SOLID
 * YARP is not required. A robot model wrapped in YARP ports is implemented in YarpModel.
 */
namespace KinematicModel
{
	class BodyPart;
	class Robot;
}
	
/** \brief Allows motors to be controlled via groups such as 'torso', 'head' and 'arm'
 *
 * This class facilitates grouping of the motors (and therefore joints) into subsystems, as in the YARP interface to the iCub. 
 * An instance of BodyPart therefore inherits a QVector of motors and provides a method setEncPos( const QVector<qreal>& x ), to set
 * the encoder positions of the motors.
 */
class KinematicModel::BodyPart : public QVector<Motor*>
{
	
public:
	
    BodyPart( Robot* aRobot, BodyPart* aParentBodyPart = 0 );	//!< \brief The constructor immediately appends the new object to the Robot
																/**< ...by calling Robot.appendBodyPart( BodyPart* this )
																	 \param aRobot The robot to which this BodyPart should be appended
																	 \param aParentBodyPart Facilitates robot creation via the Qt XML parser and ZPHandler */
    ~BodyPart();												//!< Nothing special to destroy here
    
    void setName( const QString& name ) { partName = name; }	//!< Sets a human readable name for the BodyPart
	const QString& name() const { return partName; }			//!< Returns the human readable name of the BodyPart
    BodyPart* parent() const { return parentPart; }				//!< Returns a pointer to the parent BodyPart
																/**< This is currently used only by ZPHandler for building the robot from a hierarchical
																	 XML file. There is nothing inherently heirarchical about a group of BodyPart objects.
																	 It is the underlying KinTreeNode objects (Link, RevoluteJoint and PrismaticJoint) that
																	 reflect the hierarchy of the kinematic tree */
    bool setEncPos( const QVector<qreal>& pos );				//!< Sets all motor positions by calling Motor.setEncPos( qreal ) on each motor
																/**< \param pos A vector of encoder positions. If there are too few or too many
																	 positions in the vector, a warning will be printed to the console, and all
																	 valid motor positions will be set. */
	bool verify();												//!< Returns 'true' if there are no void pointers in the QVector and 'false' otherwise
	
	void addConstraint( QStringList _a, QStringList _q, qreal _b );
	bool evaluateConstraints();
	
private:
    BodyPart* parentPart;	//!< Requires a parent object for compatibility with the hierarchical ZPHandler
    QString   partName;		//!< A human readable name
	
	// linear constraints C*q > d
	
	class LinearConstraint {
	public: 
		LinearConstraint(){}
		LinearConstraint( BodyPart *_parent, QStringList _a, QStringList _q, qreal _b) : parent(_parent), b(_b) {
			QStringList::iterator i,j;
			for ( i=_a.begin(), j=_q.begin(); i!=_a.end() && j!=_q.end(); ++i, ++j)
			{
				a.append( (*i).toDouble() );
				q.append( (*j).toInt() );
			}
		}
		~LinearConstraint(){}
		
		bool evaluate()
		{
			if ( !parent || a.begin()==a.end() || q.begin()==q.end() ) {
				printf("Bad constraint encountered!\n");
				return false;
			}
			
			double AdotQ = 0.0;
			QList<qreal>::iterator i;
			QList<int>::iterator j;
			for ( i=a.begin(), j=q.begin(); i!=a.end() && j!=q.end(); ++i, ++j) {
				if ( parent->at(*j) ) {
					AdotQ += (*i) * parent->at(*j)->encPos();
					//if ( i!=a.begin() ) printf(" + ");
					//printf("%f * %f", *i, parent->at(*j)->encPos() );
				}
			} 
			
			double ans = AdotQ + b;
			//printf(" + %f = %f >? 0", b, ans);
			return ans > 0;
		}
	private:
		BodyPart		*parent;
		QList<qreal>	a;
		QList<int>		q;
		qreal			b;
	};
	QVector<LinearConstraint> constraints;
};

#endif

/** @} */
