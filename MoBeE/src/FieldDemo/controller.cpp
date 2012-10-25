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

void Controller::computeForces()
{
        printf("Called computeForces()\n");
        
        // this should be a slot that is triggered by the signal robot.observation()
        KinematicModel::RobotObservation obs = robot->observe();
        printf(" Observing %d markers\n",obs.getNumMarkers());
        
        //gravity
        QVector3D F(100.0,0.0,100.0);
        QVector3D T(0.0,0.0,0.0);
        
        QVector<qreal> netForce(0.0);
        for (size_t i=0; i<obs.getNumMarkers(); i++)
        {
            QVector<qreal> thisForce;
            for (size_t j=3; j<obs.markerJacobian(i).size(); j++)
            {
                qreal f = 0.0;
                f += obs.markerJacobian(i).at(j).first.x()  * F.x();
                f += obs.markerJacobian(i).at(j).first.y()  * F.y();
                f += obs.markerJacobian(i).at(j).first.z()  * F.z();
                f += obs.markerJacobian(i).at(j).second.x() * T.x();
                f += obs.markerJacobian(i).at(j).second.y() * T.y();
                f += obs.markerJacobian(i).at(j).second.z() * T.z();
                thisForce.append(f);
            }
            netForce = vectorSum(netForce, thisForce);
        }
        
        yarp::os::Bottle b;
        //std::cout <<" this_f: ";
        for ( size_t k=0; k!=netForce.size(); k++ )
        {
            b.addDouble(netForce.at(k));
            //std::cout << netForce.at(k) << " ";
        }
        //std::cout << std::endl;
        
        set(&f,&b);
}

QVector<qreal> Controller::vectorSum(QVector<qreal> a,QVector<qreal> b)
{
    QVector<qreal> ans;
    if ( a.size() > b.size() ) {
        ans = a;
        for (size_t i=0; i<b.size(); i++)
            a.replace(i, a.at(i) + b.at(i));
    }
    else {
        ans = b;
        for (size_t i=0; i<a.size(); i++)
            b.replace(i, a.at(i) + b.at(i));
    }
    return ans;
}