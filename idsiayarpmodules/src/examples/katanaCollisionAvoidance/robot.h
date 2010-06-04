#ifndef KINTREE_H
#define KINTREE_H

#include "glwidget.h"
#include "branch.h"
#include "motor.h"
#include "link.h"
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

signals:
    //! The signal that a Link has been created.
    /*! Currently heard by GLWidget only, this signal signifies that we can define the bounding cylinder. */
    void finishLink( Link* link );

private:
    QString          robotName;     // Yarp name of robot
    QVector<Branch*> branchList;    // "Branches" correspond to Yarp motor control groups such as 'torso' and 'leftArm'
                                        /* A branch contains a list of motors, and a motor contains a list of joints.
                                           Motors are bound to their joints by linear functions. Therefore, joints should
                                           always be moved via a branch.setPos(QVector<qreal>) */
    QVector<Motor*>  motorList;
    QVector<Link*>   tree;     // root nodes of the link/joint trees
    int              numLinks;     // joint counter

    // to help construct the robot
    Branch* getBranchByName( const QString& branchName );
    Motor* getMotorByName( const QString& motorName );

    // to filter out collision pairs we don't need to check
    void filterCollisionPairs(DtResponse response);

    // to calculate the new pose after motors/joints have been moved
    void updateTxfrMatrices();

    /*** FRIEND FUNCTIONS ***/
    // to initialize the robot in Robot::configure()
    friend Branch::Branch( Robot*, Branch* );
    friend Motor::Motor( Robot*, Motor* );
    friend Link::Link( Robot* );
    friend Link::Link( Link* );
    //friend Joint::Joint( Robot*, Motor* );
    //friend Joint::Joint( Link*, Motor* );
    friend bool ZPHandler::startElement( const QString&, const QString&, const QString&, const QXmlAttributes& );

    // allow links to send signals (to OpenGL)
    friend void Link::emitFinished();
    friend void Link::updateTxfrMatrix( const QMatrix4x4& );

    // a wrapper for the user
    friend class RobotInterface;

};

#endif // KINTREE_H
