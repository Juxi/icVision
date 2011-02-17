#ifndef KINTREE_H
#define KINTREE_H

#include "glwidget.h"
#include "world.h"
#include "branch.h"
#include "motor.h"
#include "link.h"
#include "kintreenode.h"
#include "zphandler.h"

/*! \brief A kinematic model of a robot.
 *
 * The model consists of a tree of Link objects, some of which are Joint objects, as well as a list of Motor objects
   and a list of Branch objects. Motors are bound to joints and control the joints' position according to a linear mapping.
   Brnaches are groups of motors, accessable via a vector (see RobotInterface::setPos(int,QVector<qreal>)). The Robot class
   is primarily a container, and the tree as well as the lists are populated by the constructors of Branch, Motor and Link,
   which are friend functions of this class.
 */

class Robot : public QObject
{
    Q_OBJECT

public:
    Robot();
    ~Robot();

    // GET AND SET STUFF
    void setName( const QString& name ) { robotName = name; }
        //! Return the name of the Robot.
        /*! May be used to connect to YARP */
    const QString& getName() const { return robotName; }
    Branch* getBranchByName( const QString& branchName );
    Motor* getMotorByName( const QString& motorName );
    void appendBranch( Branch* branch ) { branchList.append(branch); }
    void appendMotor( Motor* motor ) { motorList.append(motor); }
    void appendLink( KinTreeNode* link ) { tree.append(link); }
    int nextBranchIdx() const { return branchList.size(); }
    int nextMotorIdx() const { return motorList.size(); }
    int nextLinkIdx() { return numLinks++; }

        //! Return the name of a Motor.
        /*!   */
    const QString* getMotorName( int branchNum, int motorNum ) const;
        //! Return the name of the Motor control group.
        /*! If connecting to YARP, this is the name of the target control group. For the iCub, 'torso', 'arm', 'head', ect. */
    const QString* getBranchName( int branchNum ) const;
        //! Return the number of motors in the group branchNum.
        /*! Returns 0 if branchNum is out of range.  */
    int getNumMotors( int branchNum ) const;
        //! Return the number of Motor groups.
        /*! In YARP these groups correspond to ports such as 'torso' and 'head'. */
    int getNumBranches() const;

    // PRINT STUFF FOR HUMANS TO READ
        //! Print the Link Tree.
        /*! Prints the tree of links with all properties depth first.  */
    void printLinks();
        //! Print the Motor control groups.
        /*! Print a list of the Motor objects in each group. */
    void printBranches();

    // TO CONSTRUCT AND USE THE ROBOT
        //! Initialize the Robot
        /*! Parse the robot configuration XML file and call constructors of Link, Joint, Motor, and Branch, to build up the Robot.
        ZPRobotHandler, which inherits QXmlDefaultHandler, calls the constructors. */
    bool configure( const QString& fileName );
        //! Set the position of every joint on the robot.
        /*! Usually to zero position. If the value is out of range, the maximum or minimum value will be used accordingly. */
    void setEncoderPosition( qreal pos = 0 );
        //! Set the positions of all motors in the group branchNum.
        /*! Returns 0 if branchNum is out of range. If the value is out of range, the maximum or minimum value will be used
        accordingly. Motors are numbered as they are encountered by the parser (see configure()).
        To understand this better, try looking at the output of printJoints() and printBranches(). */
    bool setEncoderPosition( int branchNum, const QVector<qreal>& pos );
        //! Set the position of a single Motor.
        /*! If the value is out of range, the maximum or minimum value will be used accordingly.  */
    bool setEncoderPosition( int branchNum, int motorNum, qreal pos );
        //! Update the pose of the Robot.
        /*! This recursive function carries out the matrix multiplication of forward kinematics. As the position and orientation
        of each link in the tree is calculated, the Link sends a signal via the Robot, Robot::NewJoint(int).  */
    void updatePose();

    World world;

signals:
        //! The signal that a Link has been created.
        /*! Currently heard by GLWidget only, this signal signifies that we can define the bounding cylinder. */
    void finishLink( DisplayList* object );
	void collision();

private:
    QString          robotName;     // Yarp name of robot
    QVector<Branch*> branchList;    // "Branches" correspond to Yarp motor control groups such as 'torso' and 'leftArm'
                                        /* A branch contains a list of motors, and a motor contains a list of joints.
                                           Motors are bound to their joints by linear functions. Therefore, joints should
                                           always be moved via a branch.setPos(QVector<qreal>) */
    QVector<Motor*>       motorList;
    QVector<KinTreeNode*> tree;     // root nodes of the link/joint trees
    int                   numLinks;     // link counter

    // to help move the robot
    bool branchIdxInRange( int idx ) const;
    bool motorIdxInRange( int idx, int branchNum ) const;

    // to filter out collision pairs we don't need to check
    void filterCollisionPairs();

    /*** FRIEND FUNCTIONS ***/
	friend class CollisionDetector;
    friend void KinTreeNode::filterCollisionPairs(); // allow links to read the link tree
    friend void KinTreeNode::serialFilter(KinTreeNode *node, bool link, bool joint); // allow links to read the link tree
    friend void KinTreeNode::emitFinished();         // allow links to send signals (to OpenGL)
};

#endif // KINTREE_H
