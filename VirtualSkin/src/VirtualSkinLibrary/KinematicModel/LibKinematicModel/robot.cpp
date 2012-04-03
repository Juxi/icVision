#include "robot.h"
#include "model.h"
#include <time.h>

using namespace KinematicModel;

Robot::Robot( Model* m, DT_RespTableHandle t,
						DT_ResponseClass robotClass, 
						DT_ResponseClass baseClass ) :	model(m),
														responseTable(t),
														worldRobotClass(robotClass),
														worldBaseClass(baseClass),
														robotName("unNamedRobot"),
														numLinks(0),
														isConfigured(false),
														numCollisions(0),
														numReflexCollisions(0)
{
	if ( !t ) { throw KinematicModelException("The Robot constructor requires a valid DT_RespTableHandle."); }
	qRegisterMetaType< QVector<qreal> >("QVector<qreal>");
	qRegisterMetaType< RobotObservation >("RobotObservation");
}
Robot::~Robot()
{
	if ( isOpen() ) { close(); }
}

void Robot::appendNode( KinTreeNode* node )
{
	//printf("Called Append KinTreeNode to Robot\n");
	tree.append(node);
}

void Robot::close()
{	
	isConfigured = false;
	
	QVector<Motor*>::iterator j;
    for ( j=motorList.end(); j!=motorList.begin(); ) {
		--j;
        delete (*j);
    }
    QVector<BodyPart*>::iterator i;
    for ( i=partList.end(); i!=partList.begin(); )
	{
		--i;
        delete (*i);
    }
	
	kill();
}

void Robot::kill()
{
	QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i )
	{
        (*i)->kill();
    }
}

void Robot::open(const QString& fileName, bool verbose) throw(KinematicModelException)
{	
	//printf("Robot.Open()\n");
    ZPHandler handler( model, this );
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    QFile file(fileName);
	
    if ( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		QString errStr = "failed to open file '";
		errStr.append(fileName);
		errStr.append("'");
		throw KinematicModelException(errStr);
    }
	
    QXmlInputSource xmlInputSource( &file );
    if ( !reader.parse( xmlInputSource ) )
	{
		QString errStr = "failed to create robot '";
		errStr.append(getName());
		errStr.append("' from file '");
		errStr.append(fileName);
		errStr.append("'");
		throw KinematicModelException(errStr);
    }
	
	ignoreAdjacentPairs();
	//ignoreAdjacentPairs();
	home();
	
	printf("Created Robot: %s with %d primitives\n",getName().toStdString().c_str(), getNumPrimitives());
	
	//home(verbose);
	
	isConfigured = true;
}

/*void Robot::removeCollisionResponse( DT_ResponseClass c, DT_RespTableHandle t );
{
	QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i )
	{
        (*i)->removeCollisionResponse( c, t );
    }
}*/

void Robot::ignoreAdjacentPairs()
{
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i )
	{
        (*i)->ignoreAdjacentPairs();
    }
}

void Robot::home(bool verbose)
{
	if (verbose) printf("Going to home position.\n");
	QVector<Motor*>::iterator j;
    for ( j=motorList.begin(); j!=motorList.end(); ++j ) {
        (*j)->home(verbose);
    }
	//updatePose();
}

void Robot::setNormalPosition( qreal pos )
{
	//QMutexLocker locker(&mutex);
	
    QVector<BodyPart*>::iterator i;
    QVector<Motor*>::iterator j;
	
	//printf("Setting all positions to %f\n", pos);
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        for ( j = (*i)->begin(); j != (*i)->end(); ++j ) {
            (*j)->setNormPos(pos);
        }
    }
}
void Robot::setEncoderPosition( qreal pos )                                        // for every motor on the robot
{
	QMutexLocker locker(&mutex);
	
    QVector<BodyPart*>::iterator i;
    QVector<Motor*>::iterator j;

	//printf("Setting all positions to %f\n", pos);
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        for ( j = (*i)->begin(); j != (*i)->end(); ++j ) {
            (*j)->setEncPos(pos);
        }
    }
}

bool Robot::setEncoderPosition(int partNum, const QVector<qreal>& pos)           // for an entire branch (using encoder positions)
{
	//printf("called setEncoderPosition() - size %i \n", pos.size() );

	QMutexLocker locker(&mutex);
	
    if ( partIdxInRange(partNum) ) {
        return partList.at(partNum)->setEncPos(pos);
    }
    else { return 0; }
}

bool Robot::setNormalPosition( int partNum, int motorNum, qreal pos )           // for only one motor (using normal position)
{
	QMutexLocker locker(&mutex);
	
    if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
        partList.at(partNum)->at(motorNum)->setNormPos(pos);
        return 1;
    }
    else { return 0; }
}

bool Robot::setEncoderPosition( int partNum, int motorNum, qreal pos )           // for only one motor (using encoder position)
{
	QMutexLocker locker(&mutex);
	
    if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
        partList.at(partNum)->at(motorNum)->setEncPos(pos);
        return 1;
    }
    else { return 0; }
}

qreal Robot::EncoderToNormalPosition( int partNum, int motorNum, qreal pos ) {
    if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
		return partList.at(partNum)->at(motorNum)->encToNorm(pos);
    }
    else
    	return 0.0;
}

qreal Robot::NormalToEncoderPosition( int partNum, int motorNum, qreal pos ) {
	if ( partIdxInRange(partNum) && motorIdxInRange(partNum,motorNum) ) {
		return partList.at(partNum)->at(motorNum)->normToEnc(pos);
	}
	else
		return 0.0;
}

void Robot::updatePose()
{
	//QMutexLocker locker(&mutex);
	
    QMatrix4x4 T;
    T.setToIdentity();
    QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->update(T);
    }
	
	unsigned int m, mc = markers.size();
	if (mc > 0)
	{
		for (m=0; m<mc; m++)
		{
			markers[m]->updateTracer();
		}
	}
	//emit changedState();
}


void Robot::evaluateConstraints()
{
	QVector<BodyPart*>::iterator i;
    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        if ( !(*i)->evaluateConstraints() )
			addReflexCollision();
    }
}

void Robot::publishState()
{
	emit collisions(numCollisions);
	numCollisions = 0;
	
	emit reflexCollisions( numReflexCollisions );
	numReflexCollisions = 0;
	
	RobotObservation obs = observe();
	emit observation(obs);
	
	//processState();
}

RobotObservation Robot::observe()
{
	RobotObservation obs;
	unsigned int m, mc = markers.size();
	if (mc > 0)
	{
		obs.m_markerName.resize(mc);
		obs.m_markerConfiguration.resize(mc);
		for (m=0; m<mc; m++)
		{
			obs.m_markerName[m] = markers[m]->name();
			obs.m_markerConfiguration[m] = markers[m]->object()->getT();
		}
	}
	return obs;
}
/*void Robot::render()
{
	QMutexLocker locker(&mutex);
	
	QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        (*i)->render();
    }
	
}*/

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


void Robot::appendTreeToModel( KinTreeNode* node )
{
	QVector<KinTreeNode*>::iterator i;
	if ( node == NULL ) {
		for ( i=tree.begin(); i!=tree.end(); ++i )
			appendTreeToModel(*i);
	} else {
		for ( i=node->children.begin(); i!=node->children.end(); ++i ) {
			appendTreeToModel(*i);
		}
		model->appendObject(node);
	}
}

void Robot::appendMarkersToModel()
{
	QVector<Marker*>::iterator i;
	for ( i=markers.begin(); i!=markers.end(); ++i ) {
			model->appendObject( (*i)->getTracerObject() );
	}
}

int Robot::getNumPrimitives()
{
	//printf("Robot.getNumPrimitives()\n");
	int result = 0;
	QVector<KinTreeNode*>::iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i ) {
        result += (*i)->getNumPrimitives();
    }
	//printf(" num primitives: %d\n", result );
	return result;
}


/*bool Robot::isColliding() const
{
	QVector<KinTreeNode*>::const_iterator i;
    for ( i=tree.begin(); i!=tree.end(); ++i )
	{
        if ( (*i)->isColliding() )
		{
			return true;
		}
    }
	return false;
}*/

/*
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
*/
