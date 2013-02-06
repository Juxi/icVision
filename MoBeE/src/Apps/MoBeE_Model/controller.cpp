#include "controller.h"
#include "robot.h"
#include <iostream>

Controller::Controller( KinematicModel::Robot* _robot,
                       QString _config_file,
                       int _partNum,
                       int freq ) : PartController( _robot->getName().toStdString().c_str(),
                                                    _robot->getPart(_partNum)->name().toStdString().c_str(),
                                                    _config_file.toStdString().c_str(),
                                                    freq),
                                    //rpcPort(this),
                                    robot(_robot),
                                    partNum(_partNum),
                                    cstThresh(20.0)
{
    QObject::connect( this, SIGNAL(setRobotPosition(int,QVector<qreal>)), robot, SLOT(setEncoderPosition(int,QVector<qreal>)) );
    QObject::connect( robot->getPart(partNum), SIGNAL(repulsiveForce(QVector<qreal>)), this, SLOT(setRepulsiveForce(QVector<qreal>)) );
    
    qRegisterMetaType< QVector< QVector< QPair< qreal, QVector<qreal> > > > >("QVector< QVector< QPair< qreal, QVector<qreal> > > >");
    QObject::connect( robot->getPart(partNum), SIGNAL(constraintState(QVector< QVector< QPair< qreal, QVector<qreal> > > >)), this, SLOT(setConstraintSpring(QVector< QVector< QPair< qreal, QVector<qreal> > > >)) );
}

void Controller::afterStart(bool s)
{
    PartController::afterStart(s);
    if (s) {
        yarp::os::ConstString portName = portPrefix + "/markerState:o";
        markerStatePort.open(portName);
        //rpcPort.open(portName);
        //rpcPort.start();
    }
}
void Controller::threadRelease()
{
    markerStatePort.close();
    //rpcPort.close();
    PartController::threadRelease();
}

void Controller::setRepulsiveForce(QVector<qreal> f)
{
    if ( f.size() != getNumJoints() )
        printf("Wrong size body part torque vector (%d,%d)\n",f.size(),getNumJoints());
    
    f = scalarMult(1000.0, f);

    yarp::os::Bottle b;
    double fMagnitude = 0.0;
    for (QVector<qreal>::iterator i=f.begin(); i!=f.end(); ++i)
        fMagnitude += (*i)*(*i);
    fMagnitude = sqrt(fMagnitude);
    //if (fMagnitude > 0.0) f = scalarMult(1.0/fMagnitude, f);
    for (QVector<qreal>::iterator i=f.begin(); i!=f.end(); ++i)
        b.addDouble(fMagnitude*(*i));
    
    setFFLD(&b);
}

void Controller::setConstraintSpring(QVector< QVector< QPair< qreal, QVector<qreal> > > > evaluatedConstraints)
{
    QVector<qreal> springDispl;
    springDispl.resize(getNumJoints());
    
    QVector< QVector< QPair< qreal, QVector<qreal> > > >::iterator i;
    QVector< QPair< qreal, QVector<qreal> > >::iterator j;
    for ( i=evaluatedConstraints.begin(); i!=evaluatedConstraints.end(); ++i) { // for the list of constraints
        bool applySpringForce = true;
        QVector<qreal> springDisplSum;
        springDisplSum.resize(getNumJoints());
        for ( j=(*i).begin(); j!=(*i).end(); ++j) { // each constraint is a list of constraints in Conjunctive Normal Form (CNF)
            if ( j->first > cstThresh ) { // if one of the constraints in the CNF is satisfied, the set is satisfied
                applySpringForce = false;
                break;
            }
            qreal dx = (cstThresh-j->first)/cstThresh;
            QVector<qreal> thisSpringDispl = scalarMult(dx, j->second);
            springDisplSum = vectorSum(thisSpringDispl, springDisplSum );
        }
        if (applySpringForce) { // if the CNF set is not satisfied, use it to apply a force to the robot
            springDispl = vectorSum(springDispl, springDisplSum);
        }
    }
    
    yarp::os::Bottle b;
    QVector<qreal>::iterator k;
    for (k=springDispl.begin(); k!=springDispl.end(); ++k)
        b.addDouble(*k);
    setFCST(&b);
}

void Controller::handler( yarp::os::Bottle* b )
{
    //if ( isValid() ) {
        //printf("got a bottle: %s\n", b->toString().c_str());
        int cmd = b->get(0).asVocab();
        QString markerName;
        yarp::os::Bottle* opSpaceForceAndTorque;
        yarp::os::Bottle jointSpaceForce;
        
        
        switch (cmd) {
            case VOCAB_OPSPACE:
                markerName = QString(b->get(1).asString());
                opSpaceForceAndTorque = b->get(2).asList();
                if ( !projectToJointSpace( markerName, opSpaceForceAndTorque, jointSpaceForce ) ) {
                    printf("project() failed!!!\n");
                    break;
                }
                setFRPC( &jointSpaceForce );
                //printf("Set joint-space force from op-space!!! (%s)\n", jointSpaceForce.toString().c_str());
                break;
            default:
                printf("Controller received unknown command! %s", b->toString().c_str());
                break;
        }
    //} else printf("Device driver is not valid\n");
}

void Controller::procEncoders( double* q )
{
    QVector<qreal> Q;
    for ( int i=0; i<getNumJoints(); i++ )
		Q.append(q[i]);
    
    //robot->setEncoderPosition(partNum,Q);
    emit setRobotPosition(partNum,Q);
}

void Controller::publishState()
{
    bool sendBottle = true;
    yarp::os::Bottle states;
    QList<QString> markerList;
    if (!getMarkerNames(markerList)) sendBottle = false;
    for (QList<QString>::iterator i=markerList.begin(); i!=markerList.end(); ++i) {
        QVector3D pos,norm;
        if ( getMarkerPosition(*i,pos) && getMarkerNormal(*i,norm) ) {
            yarp::os::Bottle aState;
            aState.addString(i->toStdString().c_str());
            aState.addDouble(pos.x());
            aState.addDouble(pos.y());
            aState.addDouble(pos.z());
            aState.addDouble(norm.x());
            aState.addDouble(norm.y());
            aState.addDouble(norm.z());
            states.addList() = aState;
        } else {
            sendBottle = false;
            break;
        }
    }
    if (sendBottle) markerStatePort.write(states);
}

bool Controller::getMarkerNames( QList<QString>& markerList )
{
    KinematicModel::BodyPart* bodyPart = robot->getPart(partNum);
    if (bodyPart) {
        QVector<KinematicModel::Marker*> markers = bodyPart->getMarkers();
        QVector<KinematicModel::Marker*>::iterator i;
        for ( i=markers.begin(); i!=markers.end(); ++i ) {
            markerList.append((*i)->name());
        }
        return true;
    }
    return false;
}

bool Controller::getMarkerPosition( QString name, QVector3D& pos )
{
    KinematicModel::BodyPart* bodyPart = robot->getPart(partNum);
    if (bodyPart) {
        QVector<KinematicModel::Marker*> markers = bodyPart->getMarkers();
        QVector<KinematicModel::Marker*>::iterator i;
        for ( i=markers.begin(); i!=markers.end(); ++i ) {
            if ( (*i)->name() == name ) {
                pos = (*i)->node()->getPos();
                return true;
            }
        }
    }
    return false;
}

bool Controller::getMarkerNormal( QString name, QVector3D& norm )
{
    KinematicModel::BodyPart* bodyPart = robot->getPart(partNum);
    if (bodyPart) {
        QVector<KinematicModel::Marker*> markers = bodyPart->getMarkers();
        QVector<KinematicModel::Marker*>::iterator i;
        for ( i=markers.begin(); i!=markers.end(); ++i ) {
            if ( (*i)->name() == name ) {
                norm = (*i)->getNorm();
                return true;
            }
        }
    }
    return false;
}

bool Controller::projectToJointSpace( QString name, yarp::os::Bottle* opSpaceFT, yarp::os::Bottle& jointSpaceF )
{
    KinematicModel::BodyPart* bodyPart = robot->getPart(partNum);
    if (bodyPart)
    {
        KinematicModel::KinTreeNode* node = NULL;
        QVector<KinematicModel::Marker*> markers = bodyPart->getMarkers();
        QVector<KinematicModel::Marker*>::iterator i;
        for ( i=markers.begin(); i!=markers.end(); ++i ) {
            if ( (*i)->name() == name ) {
                // we are gonna project some shit
                node = (*i)->node();
                break;
            }
        }
        if (node) {
            jointSpaceF.clear();
            Jacobian jacobian = node->computeJacobian();
            for (size_t j=0; j<jacobian.size(); j++)
            {
                qreal f = 0.0;
                f += jacobian.at(j).first.x()  * opSpaceFT->get(0).asDouble();
                f += jacobian.at(j).first.y()  * opSpaceFT->get(1).asDouble();
                f += jacobian.at(j).first.z()  * opSpaceFT->get(2).asDouble();
                f += jacobian.at(j).second.x() * opSpaceFT->get(3).asDouble();
                f += jacobian.at(j).second.y() * opSpaceFT->get(4).asDouble();
                f += jacobian.at(j).second.z() * opSpaceFT->get(5).asDouble();
                jointSpaceF.addDouble(f);
            }
            return true;
        }
    }
    return false;
}

QVector<qreal> Controller::vectorSum(QVector<qreal> a,QVector<qreal> b) {
    QVector<qreal> ans;
    if (a.size() != b.size()) return ans;
    QVector<qreal>::iterator i,j;
    for (i=a.begin(),j=b.begin(); i!=a.end() && j!=b.end(); ++i,++j)
        ans.append((*i)+(*j));
    return ans;
}

QVector<qreal> Controller::scalarMult(qreal a,QVector<qreal> b){
    QVector<qreal> ans;
    QVector<qreal>::iterator i;
    for (i=b.begin(); i!=b.end(); ++i)
        ans.append(a * (*i));
    return ans;
}