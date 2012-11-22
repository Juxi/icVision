/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup MoBeE
 *	@{
 */

#ifndef SIMSYNCER_H
#define SIMSYNCER_H

#include <QThread>
#include <QMutex>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <yarp/os/all.h>

namespace KinematicModel
{
	class CompositeObject;
}

namespace MoBeE
{
	class YarpModel;
	class SimSyncer;
}
	
/** \brief Updates MoBeE world with iCub Simulator objects
* 
*
 */
class MoBeE::SimSyncer : public QThread
{
		
public:

	SimSyncer();			//!< Constructor
	virtual ~SimSyncer();	//!< Destructor
	
	void setModel(MoBeE::YarpModel* m) { model = m; };			//!< Set model on which the iCubSim synchronization operates
	void setRefreshPeriod(double p) { period = (qint64) (p*1000.);};	//!< Set thread update period in seconds
	void open( const QString& portName, const QString& simPortName );	//!< open YARP port with PORTNAME, and connect to simulator world port with SIMPORTNAME
	void close();														//!< Stop thread; close port
	void stop();														//!< Stop thread
	void step();														//!< One sync step
	
private:
	
	void run();
	bool handler( const yarp::os::Bottle& reply );	//!< Handles incomming RPC commands (expressed as YARP bottles)
	void interruptableSleeper(qint64 time); //<! Interruptable sleeper, time in millis

	MoBeE::YarpModel* model; //!< The model (world) on which this simSyncer operates
	 QMutex mutex;
	yarp::os::Bottle cmd, response;
	yarp::os::Network	yarp;
	yarp::os::RpcClient	port;
	qint64 period;
	bool keepRunning;
	QElapsedTimer sleepTimer;
};
#endif
/** @} */