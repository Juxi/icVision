#ifndef ICUBCONTROLLER_H
#define ICUBCONTROLLER_H

#include <QString>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include "PartController.h"

using namespace yarp::os;
using namespace yarp::dev;

/*! \brief Facilitates Motor Babbling
 */

class ICubController
{
public:
    ICubController();
    ~ICubController();
	
	bool open( const char* _robotName );
	void close();

protected:
	
	bool open();
	
	QString		robotName;
	
private:
	
	PartController	torso, head, leftArm, rightArm;

};

#endif // ICUBCONTROLLER_H
