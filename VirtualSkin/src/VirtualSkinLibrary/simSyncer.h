/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef SIMSYNCER_H
#define SIMSYNCER_H

#include <QThread>
#include <QMatrix4x4>
#include <yarp/os/all.h>
#include "yarpModel.h"

namespace KinematicModel
{
	class CompositeObject;
}

namespace VirtualSkin
{
	class SimSyncer;
	class YarpModel;
}
	
/** \brief Updates VirtualSkin world with iCub Simulator objects
* 
*
 */
class VirtualSkin::SimSyncer : public QThread
{
		
public:

	SimSyncer(VirtualSkin::YarpModel* model, double period); //!< Constructor; set model and update period in seconds
	virtual ~SimSyncer();	                  //!< Destructor
	
	void open( const QString& portName, const QString& simPortName ); //!< open YARP port with PORTNAME, and connect to simulator world port with SIMPORTNAME
	void close(); //!< Stop thread; close port
	
private:
	
	void run();
	bool handler( const yarp::os::Bottle& reply );	//!< Handles incomming RPC commands (expressed as YARP bottles)

	VirtualSkin::YarpModel* model; //!< The model (world) on which this simSyncer operates
	
	yarp::os::Network	yarp;
	yarp::os::RpcClient	port;
	double period;
	bool keepRunning;
	
};
#endif
/** @} */