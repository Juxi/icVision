/*
 * Copyright (C) 2010 Gregor Kaufmann
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/** @file RobotFilter.cpp Implementation file for the RobotFilter class.
 *
 * Version: $Rev$
 *
 * $Date$
 *
 */

#include "RobotFilter.h"
#include "constants.h"

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <iostream>
#include <time.h>

RobotFilter::RobotFilter() : isOpen(false), /*emitCollisions(true),*/ robot(NULL), cbFilters(), stateObservers(), callObservers(), responseObservers() {
	// No special action to take for construction
}

RobotFilter::~RobotFilter() {
	if (true == isOpen) {
		close();
	}
}

bool RobotFilter::open() {
	if (true == isOpen) {
		close();
	}
	std::cout << "Starting robot filter." << std::endl;

    // check whether a network is available
    if (false == yarp::os::Network::checkNetwork()) {
		std::cout << "network unavailable..." << std::endl;
    	return false;
    }
	
	// check whether there is a robotModel
	if ( !robot ) {
		std::cout << "no robot model has been set...  you must call setRobot(Robot&) before open()" << std::endl;
		return false;
	}

	// Get the name of the robot. It is used as the base of the YARP port
	// name of the ports the ControlBoardFilter are going to connect to
	const QString deviceBaseName(robot->getName());
	const QString filterBaseName(robot->getName() + "F");
	
	// TODO: create observers for commands
	RobotModel::PartController *p_ctrl;					// create a controller to move the robot 
	yarp::os::ControlBoardFilter *p_cbf;	// create the port filter
	StateObserver *p_so;					// create an observer for the encoder readings
	CallObserver *p_co;
	ResponseObserver *p_ro;					// create an observer for the rpc responses
	QString targetName;
	QString filterName;
	int yarpNumAxes,modelNumAxes;

	// Check for consistency in the number of controllable axes on the robotModel and the robot on the network
	const QString robotName = robot->getName();
	for (int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++) {
		std::cout << std::endl << "----------------------------------------------------------------" << std::endl;
		
		const QString* partName = robot->getPartName(bodyPart);
		std::cout << "connecting to " << robotName.toStdString() << ":" << partName->toStdString() << std::endl;
		
		p_ctrl = new RobotModel::PartController();
		if ( p_ctrl->open( robotName.toStdString().c_str(), partName->toStdString().c_str() ) )
		{
			yarpNumAxes = p_ctrl->getNumJoints();
			modelNumAxes = robot->getNumMotors(bodyPart);
			
			if (modelNumAxes == yarpNumAxes)
			{
				printf("Creating ControlBoardFilter for %s\n", robot->getPartName(bodyPart)->toStdString().c_str());
				
				partControllers.append(p_ctrl);
				p_cbf = new yarp::os::ControlBoardFilter();
				
				filterName = "/" + robot->getName() + "F/" + *(robot->getPartName(bodyPart));
				targetName = "/" + robot->getName() + "/" + *(robot->getPartName(bodyPart));
				
				if (true == p_cbf->open(filterName.toStdString().c_str(), targetName.toStdString().c_str()))
				{

					cbFilters.append(p_cbf);

					// create and set state observer
					p_so = new StateObserver(this, bodyPart);
					
					//p_so = new StateObserver(robot, bodyPart);
					p_cbf->setStateObserver(p_so);
					stateObservers.append(p_so);

					// create and set call observer
					p_co = new CallObserver(robot, bodyPart);
					p_cbf->setCallObserver(p_co);
					callObservers.append(p_co);

					// create and set response observer
					p_ro = new ResponseObserver(robot, bodyPart);
					p_co->setResponseObsever(p_ro);
					p_cbf->setResponseObserver(p_ro);
					responseObservers.append(p_ro);
					
				} else {
					printf("Failed to find YARP port: %s\n", targetName.toStdString().c_str());
					delete p_cbf;
					close();
					return false;
				}
				 
			} else {
				printf("ERROR: NUMBER OF AXES DOES NOT MATCH!\n");
				printf("     modelNumAxes: %d\n", modelNumAxes);
				printf("     yarpNumAxes: %d\n", yarpNumAxes);
				close();
				return false;
			}
		} else { 
			delete p_ctrl;
			close();
			return false; 
		}
	}
	
	isOpen = true;
	targetPose.resize(robot->getNumBodyParts());
	statusPort.setBottle( Bottle("1") );
	statusPort.start();
	
	return true;
}

void RobotFilter::close() {
	
	statusPort.stop();
	
	// remove all observers from the ControlBoardFilters, close the filters and delete them
	QVector<yarp::os::ControlBoardFilter*>::const_iterator cbfIter;
	for (cbfIter = cbFilters.begin(); cbfIter != cbFilters.end(); ++cbfIter) {
		(*cbfIter)->setResponseObserver(NULL);
		(*cbfIter)->setCallObserver(NULL);
		(*cbfIter)->setCommandObserver(NULL);
		(*cbfIter)->setStateObserver(NULL);
		(*cbfIter)->close();
		yarp::os::ControlBoardFilter *cbf = *cbfIter;
		delete cbf;
	}
	cbFilters.clear();

	// delete all observers
	QVector<StateObserver*>::const_iterator i;
	for (i = stateObservers.begin(); i != stateObservers.end(); ++i) {
		delete (*i);
	}
	stateObservers.clear();
	QVector<CallObserver*>::const_iterator j;
	for (j = callObservers.begin(); j != callObservers.end(); ++j) {
		delete (*j);
	}
	callObservers.clear();
	QVector<ResponseObserver*>::const_iterator k;
	for (k = responseObservers.begin(); k != responseObservers.end(); ++k) {
		delete (*k);
	}
	responseObservers.clear();
	
	// delete controllers
	QVector<RobotModel::PartController*>::const_iterator n;
	for (n = partControllers.begin(); n != partControllers.end(); ++n) {
		delete (*n);
	}
	partControllers.clear();

	isOpen = false;
}

void RobotFilter::takeControl() {
	
	printf("*** COLLISION RECOVERY - Stopping the robot ***\n");

	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++)
	{
		cbFilters.at(bodyPart)->cutConnection(true);		// take control away from the user
		partControllers.at(bodyPart)->stop();				// stop the robot
	}
	
	// update the command filter status on the network
	statusPort.setBottle( Bottle("0") );
	
	// turn off response to collision (so we can peacefully move out of the colliding state)
	emit haveControl();
	
	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++)
	{
		// get the desired pose from the pose buffer
		targetPose.replace(bodyPart, stateObservers.at(bodyPart)->nonCollidingPose());
		
		// set velocities and do the position move
		partControllers.at(bodyPart)->setRefSpeeds(REFLEX_SPEED);
		partControllers.at(bodyPart)->positionMove(targetPose.at(bodyPart));
	}
	
	// wait for the robot to reach the specified position... then return control to the user
	start();
}

void RobotFilter::run()
{
	bool motionDone = false;
	time_t startTime = time(NULL);
	int sleepPeriod = 3*YARP_PERIOD;
	
	// wait for the position move to finish
	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++)
	{
		while ( !motionDone )
		{
			partControllers.at(bodyPart)->checkMotionDone(&motionDone);
			usleep(sleepPeriod);	
		}

		if ( time(NULL) - startTime >= POSITION_MOVE_TIMEOUT )
		{
			printf("Position move timed out!\n");
			break;
		}
	}
	
	// re-initialize pose buffer using the non-colliding pose
	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++)
	{
		stateObservers.at(bodyPart)->initPoseBuffer( targetPose.at(bodyPart) );
	}

	// reopen the filter... 
	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++ ) {
		cbFilters.at(bodyPart)->cutConnection(false);
	}
	
	emit returnControl();
	statusPort.setBottle( Bottle("1") );
	printf("CONTROL RESTORED\n");
}
