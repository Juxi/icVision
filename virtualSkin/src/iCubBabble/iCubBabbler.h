/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup iCubBabbler
 *	@{
 */

#ifndef ICUBBABBLER
#define ICUBBABBLER

#include <QString>
#include <QThread>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include "partBabbler.h"

using namespace yarp::os;
using namespace yarp::dev;

/*! \brief Facilitates Motor Babbling
 */

class ICubBabbler : private QThread
{
	Q_OBJECT
	
public:
    ICubBabbler();												//!< Nothing special here
    ~ICubBabbler();												//!< Nothing special here
	
	bool configure( const char* _robotName );					//!< Connect to an iCub via a YARP remote device driver

	void crackBaby( qreal period, qreal velocity, bool hands );	//!< For velocity-control-based babbling 
	void doTheRobot( qreal period, qreal velocity, bool hands );//!< For position-control-based babbling
	
private:
	QString robotName;
	Network yarp;
	PartBabbler	torso, head, leftArm, rightArm;
	bool configure();
};
#endif
/** @} */