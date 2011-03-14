#include "robot.h"
#include <time.h>

using namespace RobotModel;

Robot::Robot( bool visualize ) : robotName("unNamedRobot"), numLinks(0), isConfigured(false)
{
	qRegisterMetaType< QVector<qreal> >("QVector<qreal>");
}
Robot::~Robot()
{
	if ( isOpen() ) { close(); }
}

void Robot::close()
{	
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
}

bool Robot::open( const QString& fileName)
{
    ZPHandler handler(this);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QFile file(fileName);
	
    if ( !file.open(QFile::ReadOnly | QFile::Text) )
	{
        printf("Failed to open file: %s\n",fileName.toStdString().c_str());
        return false;
    }
	
    QXmlInputSource xmlInputSource( &file );
	
    if ( !reader.parse( xmlInputSource ) )
	{
		printf("Failed to create Robot: %s", getName().toStdString().c_str());
		return false;
	}
	
	printf("Created Robot: %s\n",getName().toStdString().c_str());
	
	filterCollisionPairs();
	home();
	
	return isConfigured = true;
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
    QVector<BodyPart*>::iterator i;
    QVector<Motor*>::iterator j;

	printf("Setting all positions to %f", pos);
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        for ( j = (*i)->begin(); j != (*i)->end(); ++j ) {
            (*j)->setEncPos(pos);
        }
    }
}

bool Robot::setEncoderPosition(int partNum, const QVector<qreal>& pos)           // for an entire branch (using encoder positions)
{
	//printf("called setEncoderPosition() - size %i \n", pos.size() );
    if ( partIdxInRange(partNum) ) {
        return partList.at(partNum)->setEncPos(pos);
    }
    else { return 0; }
}

bool Robot::setEncoderPosition( int partNum, int motorNum, qreal pos )           // for only one motor (using encoder position)
{
    if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
        partList.at(partNum)->at(motorNum)->setEncPos(pos);
        return 1;
    }
    else { return 0; }
}

void Robot::updatePose()
{
    QMatrix4x4 T;
    T.setToIdentity();
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->update(T);
    }
	emit changedState();
}

void Robot::render()
{
	QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->render();
    }
}

//void Robot::notColliding()
//{
//    QVector<KinTreeNode*>::iterator i;
//    for ( i=tree.begin(); i!=tree.end(); ++i ) {
//        (*i)->notColliding();
//    }
//}

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
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->printAll();
    }
}
void Robot::printBodyParts()
{
    QVector<BodyPart*>::iterator i;
    QVector<Motor*>::iterator j;
    for ( i=partList.begin(); i!=partList.end(); ++i )
	{
		printf("**********************************************\n");
        printf(" BodyPart: %s - %d motors\n", (*i)->name().toStdString().c_str(), (*i)->size());
		printf("**********************************************\n");
		
        for ( j=(*i)->begin(); j!=(*i)->end(); ++j )
		{
			printf("    ======================================\n");
			printf("      motor: %s\n", (*j)->name().toStdString().c_str());
			printf("    ======================================\n");
            (*j)->print();
        }
		printf("**********************************************\n");
    }
}
