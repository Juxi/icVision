#include "controller.h"
#include "robot.h"

Controller::Controller( KinematicModel::Robot* _robot,
                       int _partNum,
                       int freq ) : PartController( _robot->getName().toStdString().c_str(),
                                                    _robot->getPart(_partNum)->name().toStdString().c_str(),
                                                    freq),
                        robot(_robot),
                        partNum(_partNum)
{}

bool Controller::getEncoders( double* q )
{
    bool ok = enc->getEncoders(q);
    
    QVector<qreal> Q;
    for ( int i=0; i<getNumJoints(); i++ )
		Q.append(q[i]);
    
    robot->setEncoderPosition(partNum,Q);

    return ok;
}