#include <iostream>
#include "robot.h"

using namespace std;

Robot::Robot() : robotName("iCub_SIM"), numLinks(0) {}
Robot::~Robot()
{
    QVector<Link*>::iterator k;
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

// private functions to help construct the robot
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
void Robot::filterCollisionPairs(DtResponse response)
{
    QVector<Link*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->filterCollisionPairs(response);
    }
}
void Robot::updateTxfrMatrices()
{
    QMatrix4x4 T;
    T.setToIdentity();
    QVector<Link*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->updateTxfrMatrix(T);
    }
}
