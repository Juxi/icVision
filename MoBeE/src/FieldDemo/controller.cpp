#include "controller.h"
#include "robot.h"
#include <iostream.h>

Controller::Controller( KinematicModel::Robot* _robot,
                       int _partNum,
                       int freq ) : PartController( _robot->getName().toStdString().c_str(),
                                                    _robot->getPart(_partNum)->name().toStdString().c_str(),
                                                    freq),
                        robot(_robot),
                        partNum(_partNum)
{
    QObject::connect( robot->getPart(partNum), SIGNAL(torques(QVector<qreal>)), this, SLOT(setModelTorque(QVector<qreal>)) );
}

void Controller::setModelTorque(QVector<qreal> t)
{
    if ( t.size() != getNumJoints() )
        printf("Wrong size body part torque vector\n");
    
    
    //printf("setting collision reaction: ");
    for (int i=0; i<getNumJoints(); i++)
    {
        if (i<t.size())
        {
            h[i] = t.at(i);
      //      printf("%f ",g[i]);
        }
        
    }
    //printf("\n");
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
    
    robot->setEncoderPosition(partNum,Q);
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