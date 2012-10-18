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

/*! \brief Wraps YARP's remote device driver interface neatly
 *  This could be a motor control interface for one part of the iCub for example. see: http://eris.liralab.it/wiki/Motor_control
 */

class Controller : public PartController
{
	
public:
	
    Controller( KinematicModel::Robot* robot,
               int partNum,
               int freq );
    
private:
    
    bool getEncoders( double* q );
    bool computeForces();

    KinematicModel::Robot* robot;
    int partNum;
    //KinematicModel::BodyPart* bodyPart;
	
};

#endif

/** @} */
