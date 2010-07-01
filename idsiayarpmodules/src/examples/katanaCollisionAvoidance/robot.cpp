#include <iostream>
#include "robot.h"

using namespace std;

Robot::Robot() : robotName("katana"), numLinks(0) {}
Robot::~Robot()
{
    QVector<KinTreeNode*>::iterator k;
    for ( k=tree.begin(); k!=tree.end(); ++k ) {
        delete (*k);
    }
    QVector<Motor*>::iterator j;
    for ( j=motorList.begin(); j!=motorList.end(); ++j ) {
        delete (*j);
    }
    QVector<Branch*>::iterator i;
    for ( i=branchList.begin(); i!=branchList.end(); ++i ) {
        delete (*i);
    }
    cout << "delete( robot )..." << endl;
}


/***********************************
 ******* PUBLIC INTERFACE **********
 **********************************/
bool Robot::configure( const QString& fileName)
{
    ZPHandler handler(this);
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
        //world.filterCollisions();
        filterCollisionPairs();
        updatePose();
        return true;
    }
    else { return false; }
}
int Robot::getNumBranches() const { return branchList.size(); }
int Robot::getNumMotors( int branchNum ) const
{
    if ( branchIdxInRange(branchNum) ) { return branchList.at(branchNum)->size(); }
    else { return 0; }
}
const QString* Robot::getBranchName( int branchNum ) const
{
    if ( branchIdxInRange(branchNum) ) { return &branchList.at(branchNum)->name(); }
    else { return 0; }
}
const QString* Robot::getMotorName( int branchNum, int motorNum ) const
{
    if ( branchIdxInRange(branchNum) && motorIdxInRange(branchNum,motorNum) ) {
        return &branchList.at(branchNum)->at(motorNum)->name(); }
    else { return 0; }
}
void Robot::setEncoderPosition( qreal pos )                                        // for every motor on the robot (using normalized position)
{
    QVector<Branch*>::iterator i;
    QVector<Motor*>::iterator j;

    cout << "Setting all positions to " << pos << endl;
    for ( i=branchList.begin(); i!=branchList.end(); ++i ) {
        for ( j = (*i)->begin(); j != (*i)->end(); ++j ) {
            (*j)->setEncPos(pos);
        }
    }
}
bool Robot::setEncoderPosition(int branchNum, const QVector<qreal>& pos)           // for an entire branch (using encoder positions)
{
    if ( branchIdxInRange(branchNum) ) {
        return branchList.at(branchNum)->setEncPos(pos);
    }
    else { return 0; }
}
bool Robot::setEncoderPosition( int branchNum, int motorNum, qreal pos )           // for only one motor (using encoder position)
{
    if ( branchIdxInRange(branchNum) && motorIdxInRange(branchNum,motorNum) ) {
        branchList.at(branchNum)->at(motorNum)->setEncPos(pos);
        return 1;
    }
    else { return 0; }
}
void Robot::printLinks()
{
    cout << "Trying to print nodes" << endl;
    cout << tree.size() << " root nodes" << endl;
    cout << numLinks << " total nodes" << endl;
    cout << "Trying to print nodes" << endl;
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->printAll();
        cout << "   robot printing a link" << endl;
    }
}
void Robot::printBranches()
{
    QVector<Branch*>::iterator i;
    QVector<Motor*>::iterator j;
    for ( i=branchList.begin(); i!=branchList.end(); ++i ) {
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


bool Robot::branchIdxInRange( int idx ) const {
    if ( idx >= 0 && idx < branchList.size() ) { return 1; }
    else { return 0; }
}
bool Robot::motorIdxInRange( int branchNum, int idx ) const {
    if ( idx >= 0 && idx < branchList.at(branchNum)->size() ) { return 1; }
    else { return 0; }
}
Branch* Robot::getBranchByName(const QString &branchName)
{
    QVector<Branch*>::iterator i;
    for ( i=branchList.begin(); i!=branchList.end(); ++i ) {
        if ( (*i)->name() == branchName ) { return (*i); }
    }
    return 0;
}
Motor* Robot::getMotorByName(const QString &motorName)
{
    QVector<Motor*>::iterator i;
    for ( i=motorList.begin(); i!=motorList.end(); ++i ) {
        if ( (*i)->name() == motorName ) { return (*i); }
    }
    return 0;
}
void Robot::filterCollisionPairs()
{
    QVector<KinTreeNode*>::iterator i,j;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        //for ( j=tree.begin(); j!=tree.end(); ++j ) {
            (*i)->serialFilter(*i);
        //}
    }
}
void Robot::updatePose()
{
	cout << "------------------------------------" << endl;
    QMatrix4x4 T;
    T.setToIdentity();
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
		//cout << "Updating position Node" << (*i)->idx() << endl;
        (*i)->updatePosition(T);
    }
}
