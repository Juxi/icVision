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

/****************************************************************************************************************
 *** this vocab accompanies the name of a marker and a list of 6 operational space forces and torques, like:  ***
 ***                                                 [[opsp] right_hand_marker [ Fx, Fy, Fz, Tx, Ty, Tz ]]    ***
 ***************************************************************************************************************/
 #define VOCAB_OPSPACE VOCAB4('o','p','s','p')

class Controller : public PartController
{
	
public:
    
    typedef QList< QPair<QVector3D, QVector3D> > Jacobian;
	
    Controller( KinematicModel::Robot* robot,
               int partNum,
               int freq );
    
protected:
    
    virtual void handle( yarp::os::Bottle* );
    
    bool getEncoders( double* q );
    QVector<qreal> vectorSum(QVector<qreal>,QVector<qreal>);
    
    bool getMarkerNames( QList<QString>& );
    bool getMarkerPosition( QString, QVector3D& );
    bool project( QString name, yarp::os::Bottle* opSpaceFT, yarp::os::Bottle& jointSpaceF );

    //virtual void computeForces();
    
    KinematicModel::Robot* robot;
    int partNum;
    //KinematicModel::BodyPart* bodyPart;
	
};

#endif

/** @} */
