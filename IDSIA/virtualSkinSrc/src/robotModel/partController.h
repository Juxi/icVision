#ifndef PARTCONTROLLER_H
#define PARTCONTROLLER_H

#include <QtGui>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::dev;

/*! \brief Provides a remote device driver for a YARP port
 *  This could be a motor control interface for one part of the iCub for example. see: http://eris.liralab.it/wiki/Motor_control
 */

class PartController
{
public:
    PartController();
    ~PartController();
	
	bool open( const char* _robotName, const char* _partName );
	void close();
	
	bool isValid();
	int	getNumJoints() { return numJoints; }
	bool checkMotionDone( bool* );
	
	bool stop() { return vel->stop(); }
	bool setRefSpeeds( const qreal );
	bool setRefSpeeds( const QVector<qreal>& speeds) { return pos->setRefSpeeds(speeds.constData()); }
	bool positionMove( const QVector<qreal>& );
	bool velocityMove( int i, qreal v);
	
	bool randomPosMove( qreal maxSpeed, bool hands );
	
	//bool getLimits( QVector<qreal>* min, QVector<qreal>* max );
	
	//bool normPosition( int, qreal norm, qreal* enc );

private:
	
	bool open();
	
	QString robotName;
	QString partName;
	int numJoints;
	
	PolyDriver *dd;
	IPositionControl *pos;
	IVelocityControl *vel;
	IEncoders *enc;
	IPidControl *pid;
	IAmplifierControl *amp;
	IControlLimits *lim;
};

#endif // PARTCONTROLLER_H
