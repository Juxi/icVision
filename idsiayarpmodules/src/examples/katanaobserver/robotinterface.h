#ifndef ROBOTINTERFACE_H
#define ROBOTINTERFACE_H

#include <QtGui>
#include "robot.h"

class RobotInterface
{
    /*
     *** NOTES ***
     SETTING MOTOR POSITIONS: if the requested position is outside the acceptable interval (see motor.h)
                              then the maximum or minimum allowable encoder position will be used accordingly
   */

public:
    RobotInterface();

    // robot
    bool configure();                   // read the xml file and initialize the robot
    const QObject* getQObject() const;  // a pointer to the robot's QObject for connecting signals/slots
    const QString& getName() const;     // (YARP) name of the robot
    int getNumBranches() const;         // get the number of branches (YARP control groups, such as 'torso')
    void setPos( qreal pos = 0.0 );     // set the position of every joint on the robot (usually to zero)
    void printJoints();                 // a list of joints with their properties
    void printBranches();               // a hierarchy of branches/motors/joints (without joint properties)

    // branch
    const QString* getBranchName( int branchNum ) const;        // name of the branch (YARP control group, such as 'torso')
    int getNumMotors( int branchNum ) const;                    // number of motors in this control group (returns 0 if branchNum is invalid)
    bool setPos( int branchNum, const QVector<qreal>& pos );    // set the positions of all the motors in this group (returns 0 if branchNum is invalid)
                                                                    /* Motors are numbered as encountered by the XML parser,
                                                                       which builds a tree using a traditional left traversal.
                                                                       Motors appear in their branch vector ordered according
                                                                       to ascending (if not consecutive) tree indices. For help
                                                                       understanding this convention, look at the output of
                                                                       printBranches() above. */
    
    // motor
    const QString* getMotorName( int branchNum, int motorNum ) const; // name of the motor (useful for humans if not for YARP)
    bool setPos( int branchNum, int motorNum, qreal pos );            // set position of a single motor (returns 0 if the branchNum/motorNum pair is invalid)

private:
    Robot robot;
    bool branchIdxInRange( int idx ) const;
    bool motorIdxInRange( int idx, int branchNum ) const;

    double lastPoss[6];
};

#endif // ROBOTINTERFACE_H
