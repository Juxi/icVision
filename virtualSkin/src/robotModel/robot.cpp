#include <iostream>
#include "robot.h"

using namespace std;
using namespace RobotModel;

Robot::Robot() : robotName("unNamedRobot"), numLinks(0)
{
    srand ( time(0) ); // for oscillators
}
Robot::~Robot()
{
	printf("deleting the robot\n");
	
    QVector<KinTreeNode*>::iterator k;
    for ( k=tree.begin(); k!=tree.end(); ++k ) {
        delete (*k);
    }
    QVector<Motor*>::iterator j;
    for ( j=motorList.begin(); j!=motorList.end(); ++j ) {
        delete (*j);
    }
    QVector<BodyPart*>::iterator i;
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        delete (*i);
    }
    //cout << "delete( robot )..." << endl;
	printf("deleted the robot\n");
}

bool Robot::configure( const QString& fileName)
{
	//mutex.lock();
    ZPHandler handler(this);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QFile file(fileName);
    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
        cout << "Failed to open file '" << fileName.toStdString() << "'" << endl;
        return false;
    }
    QXmlInputSource xmlInputSource( &file );
    if ( reader.parse( xmlInputSource ) ) {
        cout << "Created Robot: " << getName().toStdString().c_str() << endl;
        filterCollisionPairs();
        home();
		//setEncoderPosition(0);
		//updatePose();
        return true;
    }
    else
	{
		cout << "Failed to create Robot: " << getName().toStdString().c_str() << endl;
		return false;
	}
	//mutex.unlock();
}

void Robot::filterCollisionPairs()
{
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i )
	{
        (*i)->filterCollisionPairs();
    }
}

void Robot::home()
{
	//mutex.lock();
	printf("Going to home position.\n");
	QVector<Motor*>::iterator j;
    for ( j=motorList.begin(); j!=motorList.end(); ++j ) {
        (*j)->home();
    }
	//mutex.unlock();
	updatePose();
}

void Robot::setEncoderPosition( qreal pos )                                        // for every motor on the robot
{
	//mutex.lock();
    QVector<BodyPart*>::iterator i;
    QVector<Motor*>::iterator j;

    cout << "Setting all positions to " << pos << endl;
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        for ( j = (*i)->begin(); j != (*i)->end(); ++j ) {
            (*j)->setEncPos(pos);
        }
    }
	//mutex.unlock();
}

bool Robot::setEncoderPosition(int partNum, const QVector<qreal>& pos)           // for an entire branch (using encoder positions)
{
	//mutex.lock();
    if ( partIdxInRange(partNum) ) {
        return partList.at(partNum)->setEncPos(pos);
    }
    else { return 0; }
	//mutex.unlock();
}

bool Robot::setEncoderPosition( int partNum, int motorNum, qreal pos )           // for only one motor (using encoder position)
{
	//mutex.lock();
    if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
        partList.at(partNum)->at(motorNum)->setEncPos(pos);
        return 1;
    }
    else { return 0; }
	//mutex.unlock();
}

void Robot::updatePose()
{
	//mutex.lock();
    QMatrix4x4 T;
    T.setToIdentity();
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->update(T);
    }
	//mutex.unlock();
	emit changedState();
}

void Robot::render()
{
	//mutex.lock();
	QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->render();
    }
	//mutex.unlock();
}

void Robot::notColliding()
{
	//mutex.lock();
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->notColliding();
    }
	//mutex.unlock();
}

/**********************
 ***	GET STUFF	***
 **********************/
bool Robot::partIdxInRange( int idx ) const {
    if ( idx >= 0 && idx < partList.size() ) { return 1; }
    else { return 0; }
}

bool Robot::motorIdxInRange( int partNum, int idx ) const {
    if ( idx >= 0 && idx < partList.at(partNum)->size() ) { return 1; }
    else { return 0; }
}

int Robot::getNumMotors( int partNum ) const
{
    if ( partIdxInRange(partNum) ) { return partList.at(partNum)->size(); }
    else { return 0; }
}

const QString* Robot::getPartName( int partNum ) const
{
    if ( partIdxInRange(partNum) ) { return &partList.at(partNum)->name(); }
    else { return 0; }
}

const QString* Robot::getMotorName( int partNum, int motorNum ) const
{
    if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
	return &partList.at(partNum)->at(motorNum)->name(); }
    else { return 0; }
}

BodyPart* Robot::getPartByName(const QString &partName)
{
    QVector<BodyPart*>::iterator i;
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        if ( (*i)->name() == partName )
		{
			return (*i);
		}
    }
    return 0;
}

Motor* Robot::getMotorByName(const QString &motorName)
{
    QVector<Motor*>::iterator i;
    for ( i=motorList.begin(); i!=motorList.end(); ++i ) {
        if ( (*i) && (*i)->name() == motorName )
		{ 
			return (*i);
		}
    }
    return 0;
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
void Robot::printBodyParts()
{
    QVector<BodyPart*>::iterator i;
    QVector<Motor*>::iterator j;
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        cout << "**********************************************" << endl
             << " BodyPart " << (*i)->name().toStdString() << ": " <<  (*i)->size() << " motors" << endl
             << "**********************************************" << endl;
        for ( j=(*i)->begin(); j!=(*i)->end(); ++j ) {
            cout  << "    ======================================" << endl
                  << "      Motor " << (*j)->name().toStdString() << endl
                  << "    ======================================" << endl;
            (*j)->print();
        }
        cout << "**********************************************" << endl;
    }
}
