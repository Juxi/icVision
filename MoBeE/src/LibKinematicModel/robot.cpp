#include "robot.h"
#include "model.h"
#include <time.h>
#include <iostream>

using namespace KinematicModel;

Robot::Robot( Model* m, DT_RespTableHandle robotTable,
                        DT_RespTableHandle fieldTable,
                        DT_ResponseClass robotClass,
                        DT_ResponseClass baseClass,
                        DT_ResponseClass robotField,
                        DT_ResponseClass robotBaseField//,                        bool _openWithField
                                        ) :	model(m),
                                            responseTable(robotTable),
                                            fieldResponseTable(fieldTable),
                                            worldRobotClass(robotClass),
                                            worldBaseClass(baseClass),
                                            worldFieldClass(robotField),
                                            worldBaseFieldClass(robotBaseField),
                                            robotName("unNamedRobot"),
                                            numCompositObjects(0),
                                            isConfigured(false),
                                            //openWithField(_openWithField),
                                            numCollisions(0),
                                            numReflexCollisions(0)
{
	if ( !robotTable ) { throw KinematicModelException("The Robot constructor requires a valid DT_RespTableHandle."); }
	qRegisterMetaType< QVector<qreal> >("QVector<qreal>");
	qRegisterMetaType< RobotObservation >("RobotObservation");
}
Robot::~Robot()
{
	if ( isOpen() ) { close(); }
    DT_DestroyRespTable(getResponseTable());
    DT_DestroyRespTable(getFieldResponseTable());
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
    
	//printf("set up xml parser\n");
	
    if ( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		QString errStr = "failed to open file '";
		errStr.append(fileName);
		errStr.append("'");
		throw KinematicModelException(errStr);
    } //else printf("text file found\n");
	
    QXmlInputSource xmlInputSource( &file );
    if ( !reader.parse( xmlInputSource ) )
	{
		QString errStr = "failed to create robot '";
		errStr.append(getName());
		errStr.append("' from file '");
		errStr.append(fileName);
		errStr.append("'");
		throw KinematicModelException(errStr);
    } //else printf("text file parsed\n");
	
	//printf("Parsed the robot file\n");
	
	ignoreAdjacentPairs();
	home();
	
	printf("Created Robot: %s (%d kinematic tree nodes, %d primitives)\n",getName().toStdString().c_str(), numCompositObjects, getNumPrimitives());

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

void Robot::resetExtTorque()
{
    QVector<Motor*>::iterator j;
    for ( j=motorList.begin(); j!=motorList.end(); ++j ) {
        (*j)->resetTorque();
    }
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
	for (m=0; m<mc; m++) {
		markers[m]->updateTracer();
	}
	
	//emit changedState();
}


/*void setColliding(KinTreeNode* node) { // recursively set the whole node to colliding mode
	const QVector<PrimitiveObject*>& primitives = node->data();
	QVector<PrimitiveObject*>::const_iterator k;
	for (k = primitives.begin(); k!=primitives.end(); ++k) {
		(*k)->setColliding(CONSTRAINT);
	}
	//const QVector<KinTreeNode*>& children = node->childnodes();
	//QVector<KinTreeNode*>::const_iterator c;
	//for (c = children.begin(); c!=children.end(); ++c) {
	//	setColliding((*c));
	//}
}*/

void Robot::evaluateConstraints()
{
	QVector<BodyPart*>::iterator i;
	QVector<Motor*>:: iterator m;
	QVector<Joint*>:: iterator j;
    QVector<PrimitiveObject*>::const_iterator k;
	

    for ( i=partList.begin(); i!=partList.end(); ++i ) {
        if ( !(*i)->evaluateConstraints() ) {
			for (m = (*i)->begin(); m!=(*i)->end(); ++m) {
				for (j = (*m)->begin(); j!=(*m)->end(); ++j) {
                    for (k = (*j)->primitives.begin(); k!=(*j)->primitives.end(); ++k) {
                        (*k)->setColliding(CONSTRAINT);
                    }
					//setColliding((*j));
				}
			}
			addReflexCollision();
		}
    }
}


void Robot::publishState()
{
	emit collisions(numCollisions);
	numCollisions = 0;
	
	emit reflexCollisions( numReflexCollisions );
	numReflexCollisions = 0;
	
    QVector<BodyPart*>::iterator i;
    for ( i=partList.begin(); i!=partList.end(); ++i )
        (*i)->publishState();
    
	//RobotObservation obs = observe();
	//emit observation(obs);
	
	//processState();
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


/*void Robot::appendTreeToModel( KinTreeNode* node )
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
}*/

void Robot::appendMarkersToModel()
{
	//printf("APPENDING MARKERS TO MODEL\n");
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
    QVector<Marker*>::iterator j;
    for ( j=markers.begin(); j!=markers.end(); ++j ) {
        result += (*j)->getTracerObject()->getPrimitives().size();
        result += (*j)->getTracerObject()->getFieldPrimitives().size();
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
