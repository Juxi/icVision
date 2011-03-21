#ifndef PARTBABBLER_H
#define PARTBABBLER_H

#include <QtCore>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include "partController.h"

using namespace yarp::os;
using namespace yarp::dev;

/*! \brief Facilitates Motor Babbling
 */

class PartBabbler
{
public:
    PartBabbler();
    ~PartBabbler();
	
	bool open( const char* robotName, const char* partName ) { return partController.open(robotName,partName); }
	
	void crackBaby( qreal velocity, bool hands );
	void doTheRobot( qreal velocity, bool hands );
	//void oscillate();
	//void crackBabySafe( int );
	
	bool checkMotionDone( bool* flag ) { return partController.checkMotionDone(flag); } 
	
private:
	PartController partController;
	
	// for the oscillator
	QVector<qreal> minPos,maxPos;
	qreal freq,t;

};

#endif // PARTBABBLER_H
