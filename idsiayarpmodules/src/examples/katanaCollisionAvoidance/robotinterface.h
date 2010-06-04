#ifndef ROBOTINTERFACE_H
#define ROBOTINTERFACE_H

#include "robot.h"

/*! \brief An interface for the Robot class.
 *
 * This class defines the interface to the Robot class by exposing a subset of its members.
 */

class RobotInterface
{
public:
    RobotInterface();

    //! Initialize the Robot
    /*! Parse the robot configuration XML file and call constructors of Link, Joint, Motor, and Branch, to build up the Robot.
        ZPRobotHandler, which inherits QXmlDefaultHandler, calls the constructors. */
    bool configure( const QString& fileName );

    // to filter out collision pairs we don't need to check
    void filterAdjacentCollisions( DtResponse response );

    //! Return a pointer to the Robot.
    /*! Helpful fo connecting signals and slots. (see main.cpp) */
    const QObject* getQObject() const;

    //! Return the name of the Robot.
    /*! May be used to connect to YARP */
    const QString& getName() const;

    //! Return the number of Motor groups.
    /*! In YARP these groups correspond to ports such as 'torso' and 'head'. */
    int getNumBranches() const;

    //! Print the Link Tree.
    /*! Prints the tree of links with all properties depth first.  */
    void printJoints();

    //! Print the Motor control groups.
    /*! Print a list of the Motor objects in each group. */
    void printBranches();

    //! Set the position of every joint on the robot.
    /*! Usually to zero position. If the value is out of range, the maximum or minimum value will be used accordingly. */
    void setPos( qreal pos = 0 );

    // branch
    //! Return the name of the Motor control group.
    /*! If connecting to YARP, this is the name of the target control group. For the iCub, 'torso', 'arm', 'head', ect. */
    const QString* getBranchName( int branchNum ) const;

    //! Return the number of motors in the group branchNum.
    /*! Returns 0 if branchNum is out of range.  */
    int getNumMotors( int branchNum ) const;

    //! Set the positions of all motors in the group branchNum.
    /*! Returns 0 if branchNum is out of range. If the value is out of range, the maximum or minimum value will be used
        accordingly. Motors are numbered as they are encountered by the parser (see configure()).
        To understand this better, try looking at the output of printJoints() and printBranches(). */
    bool setPos( int branchNum, const QVector<qreal>& pos );

    // motor
    //! Return the name of a Motor.
    /*!   */
    const QString* getMotorName( int branchNum, int motorNum ) const;

    //! Set the position of a single Motor.
    /*! If the value is out of range, the maximum or minimum value will be used accordingly.  */
    bool setPos( int branchNum, int motorNum, qreal pos );            // set position of a single motor (returns 0 if the branchNum/motorNum pair is invalid)

    // other
    //! Update the pose of the Robot.
    /*! This recursive function carries out the matrix multiplication of forward kinematics. As the position and orientation
        of each link in the tree is calculated, the Link sends a signal via the Robot, Robot::NewJoint(int).  */
    void updateTxfrMatrices();

private:
    Robot robot;
    bool branchIdxInRange( int idx ) const;
    bool motorIdxInRange( int idx, int branchNum ) const;
};

#endif // ROBOTINTERFACE_H
