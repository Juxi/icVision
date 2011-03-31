/*******************************************************************
 ***              Copyright (C) 2010 Gregor Kaufmann             ***
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup VirtualSkin
 *	@{
 */

#ifndef ROBOTFILTER_H_
#define ROBOTFILTER_H_

//#include <QMutex>
#include <QThread>
#include <QVector>
#include <yarp/os/ControlBoardFilter.h>

#include "robot.h"
#include "callObserver.h"
#include "stateObserver.h"
#include "responseObserver.h"
#include "yarpModel.h"
#include "yarpStreamPort.h"

namespace VirtualSkin {

/** \brief This class provides a safety mechanism to protect a YARP device from unwanted collisions
 *
 *	 The RobotFilter is intended to protect the robot by filtering motor control commands that may lead to unwanted collisions. It duplicates the motor
 *	 control ports for your robot, and forwards commands from the duplicates to the real ones as long as the filter is connected. The filter is controlled
 *	 by a kinematic model of the robot (see YarpModel and RobotModel::Model) that reads the streams of encoder positions from the real robot, computes the
 *	 forward kinematics and does collision detection. If unwanted collisions are found, the filter cuts its connection between the duplicate ports and the real
 *	 ones, thereby effectively ignoring the user until a pure virutal handler function (collisionResponse()) returns.
 *
 *	 The user is free to implement collisionResponse() in any way they like. An example of a simple reflexive response to collision is implemented in ReflexFilter.
 *	 The implementation of RobotFilter is built on YarpFilter. In order to empower the user to gather the data needed to handle collisions in arbitrary ways, the RobotFilter supports the extension
 *	 helper-classes of YarpFilter (StateObserver, CallObserver and ResponseObserver) can be extended, and the resulting sub-classes passed to the RobotFilter
 *	 via the templated open<aStateObserver,aCallObserver,aResponseObserver>( const QString& ) function.
 */
class RobotFilter : public QThread
{
	Q_OBJECT

public:
	RobotFilter( bool visualize = false );	//!< Connects the RobotModel::Model to the RobotFilter, and initializes some members
	virtual ~RobotFilter();					//!< Calls close()

	/** \brief Builds a RobotFilter for the RobotModel::Model described in the requested config file
	 *
	 *	The filter consists of a yarp::os::ControlBoardFilter for each RobotModel::BodyPart specified in the config file. The ControlBoardFilters
	 *	require some helper classes (StateObserver, CallObserver and ResponseObserver), which can be extended to provide additional functionality.
	 *	To open a 'standard' RobotFilter full of 'standard' ControlBoardFilters, call open<StateObserver,CallObserver,ResponseObserver>( "configFile.xml" ).
	 *	For an example of this, see PATH_TO_YOUR_GIT_REPOSITORY/virtualSkin/ReflexDemo/main.cpp. If you need a custom ControlBoardFilter, extend StateObserver,
	 *	CallObserver and ResponseObserver, and pass the sub-types to the open<aStateObserver,aCallObserver,aResponseObserver>(const QString&) method.
	 *
	 * \note aStateObserver,aCallObserver,aResponseObserver MUST be sub-classes of StateObserver,CallObserver,ResponseObserver respectively
	 */
	template <class aStateObserver, class aCallObserver, class aResponseObserver>
	bool open( const QString& fileName )
	{
		if ( isOpen ) { close(); }
		
		if ( !yarp::os::Network::checkNetwork() )
		{
			printf("ROBOT FILTER ERROR: yarp network unavailable...\n");
			return false;
		}
		
		model.robot.open(fileName);
		model.start();
		
		const QString deviceBaseName( model.robot.getName() );
		const QString filterBaseName( model.robot.getName() + "F" );
		
		yarp::os::ControlBoardFilter *p_cbf;	// create the port filter
		aStateObserver *p_so;					// create an observer for the encoder readings
		aCallObserver *p_co;					// create an observer for RPC calls
		aResponseObserver *p_ro;				// create an observer for the rpc responses
												// TODO: create CommandObserver
												// TODO: create Replier
		QString targetName;
		QString filterName;
		
		for (int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
		{
			p_cbf = new yarp::os::ControlBoardFilter();
			
			filterName = "/" + model.robot.getName() + "F/" + *(model.robot.getPartName(bodyPart));
			targetName = "/" + model.robot.getName() + "/" + *(model.robot.getPartName(bodyPart));
			
			printf("----------------------------------------------------------------\n");
			printf( "connecting to %s:%s\n", model.robot.getName().toStdString().c_str(), model.robot.getPartName(bodyPart)->toStdString().c_str() );
			
			if ( p_cbf->open(filterName.toStdString().c_str(), targetName.toStdString().c_str()) )
			{
				cbFilters.append(p_cbf);
				
				p_so = new aStateObserver(this, bodyPart);		// create and set state observer
				p_cbf->setStateObserver(p_so);
				stateObservers.append(p_so);
				
				p_co = new aCallObserver(this, bodyPart);		// create and set call observer
				p_cbf->setCallObserver(p_co);
				callObservers.append(p_co);
				
				p_ro = new aResponseObserver(this, bodyPart);	// create and set response observer
				p_co->setResponseObsever(p_ro);
				p_cbf->setResponseObserver(p_ro);
				responseObservers.append(p_ro);
				
				QObject::connect(p_so, SIGNAL(setPosition(int,const QVector<qreal>&)),	&model.robot, SLOT(setEncoderPosition(int,const QVector<qreal>&)) );
				QObject::connect(p_ro, SIGNAL(setPosition(int,int,qreal)),				&model.robot, SLOT(setEncoderPosition(int,int,qreal)) );
			}
			else
			{
				printf("Failed to find YARP port: %s\n", targetName.toStdString().c_str());
				delete p_cbf;
				close();
				return false;
			}
		} 
		
		extraOpenStuff();

		isOpen = true;
		statusPort.setBottle("1");
		
		return true;
	}
	
	void close();							//!< Deletes all ControlBoardFilters and IObservers, returning the RobotFilter to the state it was in just after construction
	virtual void extraOpenStuff() {}		//!< This is called shortly before open<>(const QString&) returns
											/**< In your sub-classes, replace the empty implementation with any initialization code required.
												 For an example of this, see ReflexFilter. */ 
	virtual void collisionResponse() = 0;	//!< Provides a mechanism to respond to collision events by injecting control code. See the implementation in ReflexFilter.
											/**< This is executed once the RobotFilter has detected collisions and cut its connection. Control
												 is not restored to the user until this function returns. */
	void run();								//!< Collision response is handled in a separate thread so as not to interrupt anything
	void openStatusPort( const QString& name ) { statusPort.open(name); }	//!< Open a YARP port that streams a boolean indicating the status of the RobotFilter
																			/**< A 1 indicates the filter is connected and motor commands are being forwarded, 
																				 whereas a 0 indicates that the filter has been cut and motor commands are being ignored. */
	void closeStatusPort()		{ statusPort.close(); }						//!< Closes the Yarp stream port indicating whether or not the filter is connected
	
	YarpModel			model;	//!< The model that controls the opening and closing of the filter
								/**< It is public so that the user has access to the information that was read out of the robot configuration file, 
									 such as the name of the robot and the number of controllable axes for example */
	
public slots:
	
	void takeControl();			//!< Provides a thread-safe mechanism to cut the filter connection and begin collision recovery

protected:
	
	YarpStreamPort		statusPort;				//!< Published the (open/closed) status of the filter
												//! TODO: Make this not iCub specific	
	yarp::os::Bottle	stop_command;			//!< Stores an RPC command to stop the iCub robot
												/**< \note This is iCub specific, and it should be done differently in the future */
	bool				isOpen,					//!< Indicates whether or not filter is forwarding commands
						haveControl;			//!< Indicates whether or not the filter currently has control of the robot

	QVector<yarp::os::ControlBoardFilter *> cbFilters;	//!< A QVector of ControlBoardFilters - one for each of the robot's BodyParts
														/**< These filters have a 1-to-1 correspondence to the YARP
															 (server) control board drivers of the robot. */
	QVector<StateObserver*>		stateObservers;			//!< A QVector of IObservers. Each is bound to the state port of a BodyPart, and logs its history in a CircularBuffer
	QVector<CallObserver*>		callObservers;			//!< A QVector of IObservers that monitor RPC calls
	QVector<ResponseObserver*>	responseObservers;		//!< A QVector of IObservers that monitor RPC responses
	
};
}
#endif
/** @} */