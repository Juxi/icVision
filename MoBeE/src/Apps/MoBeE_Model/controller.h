/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "partController.h"
#include "bodypart.h"
//#include "controllerRpcInterface.h"

/****************************************************************************************************************
 *** this vocab accompanies the name of a marker and a list of 6 operational space forces and torques, like:  ***
 ***                                                 [[opsp] right_hand_marker [ Fx, Fy, Fz, Tx, Ty, Tz ]]    ***
 ***************************************************************************************************************/
#define VOCAB_OPSPACE VOCAB4('o','p','s','p')

typedef QList< QPair<QVector3D, QVector3D> > Jacobian;

class Controller : public QObject, public PartController
{
    Q_OBJECT
	
public:
    
    Controller( KinematicModel::Robot* robot, QString conf_file, int partNum, int freq );
    
public slots:
    
    void setConstraintSpring(QVector< QVector< QPair< qreal, QVector<qreal> > > >);
    void setRepulsiveForce(QVector<qreal>);
    
private:

    void afterStart(bool s);
    void threadRelease();
    
    void handler( yarp::os::Bottle* );
    void procEncoders( double* q );
    void publishState();
    
    bool getMarkerNames( QList<QString>& );
    bool getMarkerPosition( QString, QVector3D& );
    bool getMarkerNormal( QString, QVector3D& );
    bool projectToJointSpace( QString markerName, yarp::os::Bottle* opSpaceFT, yarp::os::Bottle& jointSpaceF );
    
    QVector<qreal> vectorSum(QVector<qreal>,QVector<qreal>);
    QVector<qreal> scalarMult(qreal,QVector<qreal>);

    yarp::os::Port markerStatePort;
    //ControllerRpcInterface rpcPort;
    KinematicModel::Robot* robot;
    int partNum;
    
    double cstThresh;
    
    friend class ControllerRpcInterface;
    
signals:
    void setRobotPosition(int,const QVector<qreal>&);
    //setEncoderPosition( int partNum, const QVector<qreal>& pos )
};

#endif

/** @} */
