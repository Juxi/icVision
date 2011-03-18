#ifndef KINTREE_H
#define KINTREE_H

#include "skinWindow.h"
//#include "collisionDetector.h"
#include "renderList.h"
#include "world.h"
#include "bodyPart.h"
#include "motor.h"
#include "link.h"

namespace RobotModel {
	
/*! \brief A kinematic model of a robot.
 *
 * The model consists of a tree of Link objects, some of which are Joint objects, as well as a list of Motor objects
   and a list of Branch objects. Motors are bound to joints and control the joints' position according to a linear mapping.
   Brnaches are groups of motors, accessable via a vector (see RobotInterface::setPos(int,QVector<qreal>)). The Robot class
   is primarily a container, and the tree as well as the lists are populated by the constructors of Branch, Motor and Link,
   which are friend functions of this class.
 */

class Robot : public RenderList
{
	Q_OBJECT

public:
    Robot( bool visualize = 0 );
    ~Robot();
	
	//! To Initialize The Robot
	/*! Parse the robot configuration XML file and call constructors of Link, Joint, Motor, and Branch, to build up the Robot. */
    bool open( const QString& fileName );
	bool isOpen() const	{ return isConfigured; }
	void close();

		// These functions are used by ZPRobotHandler to build up the Robot from an XML description.
		// Should they be private perhaps?
		void setName( const QString& name )		{ robotName = name; }
		void appendBodyPart( BodyPart* part )	{ partList.append(part); }
		void appendMotor( Motor* motor )		{ motorList.append(motor); }
		void resizeMotorList( int size )		{ motorList.resize(size); }
		void appendNode( KinTreeNode* node )	{ tree.append(node); }
		int nextPartIdx() const					{ return partList.size(); }
		int nextMotorIdx() const				{ return motorList.size(); }
		int nextLinkIdx()						{ return numLinks++; }
	
	// These are generic 'get' functions that may be useful
	const QString& getName() const { return robotName; }				//!< Get the name of the Robot
    const QString* getPartName( int partNum ) const;					//!< Get the name of a BodyPart, given its index (usually for printing messages) 
	int getNumMotors( int partNum ) const;								//!< Get the number of motors in a BodyPart, given its index
	BodyPart* getPartByName( const QString& partName );					//!< Get the BodyPart itself, given its name
	const QString* getMotorName( int partNum, int motorNum ) const;		//!< Get the name of a Motor, given its index and the index of its body part
	Motor* getMotorByName( const QString& motorName );					//!< Get the moter itself, given its name
    

	void render();			//!< Recursively calls render() on the link/joint trees, updating the OpenGL display lists
	//virtual void notColliding();	//!
	
    // PRINT STUFF FOR HUMANS TO READ
        //! Print the Link Tree.
        /*! Prints the tree of links with all properties depth first.  */
    void printLinks();
        //! Print the Motor control groups.
        /*! Print a list of the Motor objects in each group. */
    void printBodyParts();
	
	void home();		// Go to home position
	
    void updatePose();	// Do forward kinematics, pushing results down the link/joint trees
	
	//void emitAppended( DisplayList* list ) { emit appendedObject( list ); }
	
public slots:
	//! Set the position of every joint on the robot.
	/*! Usually to zero position. If the value is out of range, the maximum or minimum value will be used accordingly. */
    void setEncoderPosition( qreal pos = 0 );
	//! Set the position of a single Motor.
	/*! If the value is out of range, the maximum or minimum value will be used accordingly.  */
    bool setEncoderPosition( int partNum, int motorNum, qreal pos );
	//! Set the positions of all motors in the group branchNum.
	/*! Returns 0 if branchNum is out of range. If the value is out of range, the maximum or minimum value will be used
	 accordingly. Motors are numbered as they are encountered by the parser (see configure()).
	 To understand this better, try looking at the output of printJoints() and printBranches(). */
    bool setEncoderPosition( int partNum, const QVector<qreal>& pos );
	
private:
    QString					robotName;	// Yarp name of robot
    QVector<BodyPart*>		partList;	// "Body Parts" correspond to Yarp motor control groups such as 'torso' and 'leftArm'
	QVector<Motor*>			motorList;	// "Motors" serve as an interface to set the position of one or more joints
    QVector<KinTreeNode*>	tree;		// root nodes of the link/joint trees
	
    int						numLinks;		// link counter
	bool					isConfigured;	// indicates rediness for setting positions, ect
	
	//SkinWindow			*skinWindow;
	//CollisionDetector	*detector;
	
    // check validity of indices
    bool partIdxInRange( int idx ) const;
    bool motorIdxInRange( int idx, int partNum ) const;

    // to filter out collision pairs we don't need to check
    void filterCollisionPairs();

    /*** FRIEND  ***/
	//friend class CollisionDetector;
	friend class KinTreeNode;	// root nodes of the link/joint trees must be able to request other root nodes to filterCollisionPairs().
								// This may be worth fixing at some point to avoid the need for friendship.
};
	
}

#endif
