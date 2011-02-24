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
    ICubBabbler();
    ~ICubBabbler();
	
	bool configure( const char* _robotName );
	bool configure();

	void crackBaby( qreal period, qreal velocity, bool hands );
	void doTheRobot( qreal period, qreal velocity, bool hands );
	//void oscillate();
	//void crackBabySafe( int period, int magnitude );
	
private:
	QString robotName;
	Network yarp;
	PartBabbler	torso, head, leftArm, rightArm;
};

#endif // ICUBBABBLER
