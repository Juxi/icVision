#ifndef PARTCONTROLLER_H
#define PARTCONTROLLER_H

#include <QtCore>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>

namespace RobotModel {

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
	
	yarp::os::Network network;
	yarp::dev::PolyDriver *dd;
	yarp::dev::IPositionControl *pos;
	yarp::dev::IVelocityControl *vel;
	yarp::dev::IEncoders *enc;
	yarp::dev::IPidControl *pid;
	yarp::dev::IAmplifierControl *amp;
	yarp::dev::IControlLimits *lim;
};

}

#endif
