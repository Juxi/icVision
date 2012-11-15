#include "controller.h"
#include "robot.h"
#include <iostream.h>

Controller::Controller( KinematicModel::Robot* _robot,
                       int _partNum,
                       int freq ) : PartController( _robot->getName().toStdString().c_str(),
                                                    _robot->getPart(_partNum)->name().toStdString().c_str(),
                                                    freq),
                                    robot(_robot),
                                    partNum(_partNum),
                                    cstThresh(20.0)
{
    QObject::connect( this, SIGNAL(setRobotPosition(int,QVector<qreal>)), robot, SLOT(setEncoderPosition(int,QVector<qreal>)) );
    QObject::connect( robot->getPart(partNum), SIGNAL(repulsiveForce(QVector<qreal>)), this, SLOT(setRepulsiveForce(QVector<qreal>)) );
    
    qRegisterMetaType< QVector< QVector< QPair< qreal, QVector<qreal> > > > >("QVector< QVector< QPair< qreal, QVector<qreal> > > >");
    QObject::connect( robot->getPart(partNum), SIGNAL(constraintState(QVector< QVector< QPair< qreal, QVector<qreal> > > >)), this, SLOT(setConstraintSpring(QVector< QVector< QPair< qreal, QVector<qreal> > > >)) );
}

void Controller::setRepulsiveForce(QVector<qreal> t)
{
    if ( t.size() != getNumJoints() )
        printf("Wrong size body part torque vector (%d,%d)\n",t.size(),getNumJoints());
    
    //printf("setting collision reaction: ");
    for (int i=0; i<getNumJoints(); i++)
    {
        if (i<t.size()) {
            fFld[i] = t.at(i);
            //      printf("%f ",g[i]);
        }
    }
    //printf("\n");
}

void Controller::setConstraintSpring(QVector< QVector< QPair< qreal, QVector<qreal> > > > evaluatedConstraints)
{
    QVector<qreal> springDispl;
    springDispl.resize(getNumJoints());
    
    QVector< QVector< QPair< qreal, QVector<qreal> > > >::iterator i;
    QVector< QPair< qreal, QVector<qreal> > >::iterator j;
    for ( i=evaluatedConstraints.begin(); i!=evaluatedConstraints.end(); ++i)
    {
        bool applySpringForce = true;
        QVector<qreal> thisSpringDispl;
        thisSpringDispl.resize(getNumJoints());
        for ( j=(*i).begin(); j!=(*i).end(); ++j) {
            thisSpringDispl = vectorSum(thisSpringDispl, scalarMult(j->first, j->second) );
            if ( j->first > cstThresh ) {
                applySpringForce = false;
                break;
            }
        }
        if (applySpringForce)
            springDispl = vectorSum(springDispl, thisSpringDispl);
    }
    //if (!applySpringForce) {
    //    springDispl.clear();
    //    springDispl.resize(getNumJoints());
    //}
    for (int j=0; j<getNumJoints(); j++) {
        if (j<springDispl.size())
            fCst[j] = springDispl.at(j);
    }
}

void Controller::handler( yarp::os::Bottle* b )
{
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
            setForce( &jointSpaceForce );
            //printf("Set joint-space force from op-space!!! (%s)\n", jointSpaceForce.toString().c_str());
            break;
        default:
            printf("Controller received unknown command! %s", b->toString().c_str());
            break;
    }
}

void Controller::procEncoders( double* q )
{
    QVector<qreal> Q;
    for ( int i=0; i<getNumJoints(); i++ )
		Q.append(q[i]);
    
    //robot->setEncoderPosition(partNum,Q);
    emit setRobotPosition(partNum,Q);
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