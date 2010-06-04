#include <iostream>
#include "robotinterface.h"

using namespace std;

RobotInterface::RobotInterface() {}

// construct the robot
bool RobotInterface::configure( const QString& fileName)
{
    ZPHandler handler(&robot);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QFile file(fileName);
    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
        cout << "Failed to open file '" << fileName.toStdString() << "'" << endl;
        return false;
    }
    cout << "Parsing..." << endl;
    QXmlInputSource xmlInputSource( &file );
    if ( reader.parse( xmlInputSource ) ) {
        cout << "Successfully parsed the file!!" << endl;
        return true;
    }
    else { return false; }
}
void RobotInterface::filterAdjacentCollisions(DtResponse response) { robot.filterCollisionPairs(response); }

// query the robot
const QObject* RobotInterface::getQObject() const { return &robot; }
const QString& RobotInterface::getName() const { return robot.robotName; }
int RobotInterface::getNumBranches() const { return robot.branchList.size(); }
const QString* RobotInterface::getBranchName( int branchNum ) const
{
    if ( branchIdxInRange(branchNum) ) { return &robot.branchList.at(branchNum)->name(); }
    else { return 0; }
}
int RobotInterface::getNumMotors( int branchNum ) const
{
    if ( branchIdxInRange(branchNum) ) { return robot.branchList.at(branchNum)->size(); }
    else { return 0; }
}
const QString* RobotInterface::getMotorName( int branchNum, int motorNum ) const
{
    if ( branchIdxInRange(branchNum) && motorIdxInRange(branchNum,motorNum) ) {
        return &robot.branchList.at(branchNum)->at(motorNum)->name(); }
    else { return 0; }
}

// move the robot
void RobotInterface::setPos( qreal pos )                                        // for every motor on the robot (using normalized position)
{
    QVector<Branch*>::iterator i;
    QVector<Motor*>::iterator j;

    cout << "Setting all positions to " << pos << endl;
    for ( i=robot.branchList.begin(); i!=robot.branchList.end(); ++i ) {
        for ( j = (*i)->begin(); j != (*i)->end(); ++j ) {
            (*j)->setEncPos(pos);
        }
    }
}
bool RobotInterface::setPos(int branchNum, const QVector<qreal>& pos)           // for an entire branch (using encoder positions)
{
    if ( branchIdxInRange(branchNum) ) {
        return robot.branchList.at(branchNum)->setEncPos(pos);
    }
    else { return 0; }
}
bool RobotInterface::setPos( int branchNum, int motorNum, qreal pos )           // for only one motor (using encoder position)
{
    if ( branchIdxInRange(branchNum) && motorIdxInRange(branchNum,motorNum) ) {
        robot.branchList.at(branchNum)->at(motorNum)->setEncPos(pos);
        return 1;
    }
    else { return 0; }
}
void RobotInterface::updateTxfrMatrices() { robot.updateTxfrMatrices(); }

// other
void RobotInterface::printJoints()
{
    QVector<Link*>::iterator i;
    for ( i=robot.tree.begin(); i!=robot.tree.end(); ++i ) {
        (*i)->printAll();
    }
}
void RobotInterface::printBranches()
{
    QVector<Branch*>::iterator i;
    QVector<Motor*>::iterator j;
    for ( i=robot.branchList.begin(); i!=robot.branchList.end(); ++i ) {
        cout << "**********************************************" << endl
             << " Branch " << (*i)->idx() << ": " << (*i)->name().toStdString() << " - " << (*i)->size() << " motors" << endl
             << "**********************************************" << endl;
        for ( j=(*i)->begin(); j!=(*i)->end(); ++j ) {
            cout  << "    ======================================" << endl
                  << "      Motor " << (*j)->idx() << ": " << (*j)->name().toStdString() << endl
                  << "    ======================================" << endl;
            (*j)->print();
        }
        cout << "**********************************************" << endl;
    }
}

// private functions
bool RobotInterface::branchIdxInRange( int idx ) const {
    if ( idx >= 0 && idx < robot.branchList.size() ) { return 1; }
    else { return 0; }
}
bool RobotInterface::motorIdxInRange( int branchNum, int idx ) const {
    if ( idx >= 0 && idx < robot.branchList.at(branchNum)->size() ) { return 1; }
    else { return 0; }
}
