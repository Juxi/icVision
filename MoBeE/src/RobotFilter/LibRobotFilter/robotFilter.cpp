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

#include "robotFilter.h"
#include "filterRpcInterface.h"
#include "constants.h"

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <time.h>

using namespace MoBeE;

RobotFilter::RobotFilter(	KinematicModel::Robot* r,
							//KinematicModel::Model* m,
							bool visualize				) : robot(r),
															isOpen(false),
															haveControl(false),
															isColliding(false)
{
	//QObject::connect( m, SIGNAL(computedState(int)),	this, SLOT(collisionStatus(int)) );
	QObject::connect( robot, SIGNAL(reflexCollisions(int)),	this, SLOT(takeControl(int)) );
	QObject::connect( this, SIGNAL(reflexDone()),			this, SLOT(openFilter()) );
	
	stop_command.addVocab(VOCAB_SET);
	stop_command.addVocab(VOCAB_STOPS);

	active = true;
	filterRpcInterface.setFilter(this);
	
	statusPort.setBottle("1");
}

RobotFilter::~RobotFilter()
{
	filterRpcInterface.close();
	if ( isOpen ) { close(); }
}

void RobotFilter::close()
{
	//model.stop();
	//robot->close();
	//model.world->clear();
	//statusPort.close();
	
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

	isOpen = false;
}

/*void RobotFilter::collisionStatus( int collisions )
{
	if ( collisions > 0 ) { isColliding = true; }
	else { isColliding = false; }
}*/

void RobotFilter::takeControl( int numReflexCollisions )
{
    //return;
    
	if (!active) {isColliding = false; return; };

	if ( numReflexCollisions > 0 ) { isColliding = true; }
	else { isColliding = false; }

	if ( isColliding && !haveControl )
	{
		haveControl = true;
		
		// do some control in response
		start();
	}
}

void RobotFilter::run()
{	
	collisionResponse();
	
	if ( isColliding )
	{
		printf("No safe pose in the buffer. Please resolve the situation manually, and the RobotFilter will re-open auto-magically! \n");
	}
	
	while ( isColliding )
	{
		printf(".");
		msleep(YARP_PERIOD_ms);
	}
	
	printf("\n");
	
	emit reflexDone();
}

void RobotFilter::openFilter()
{	
	// reinitialize the pose buffer with the current pose
	setWaypoint();
	
	// reopen the proxy (if necessary)... 
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
	{
		cbFilters.at(bodyPart)->cutConnection(false);
	}
	
	//inform the user
	statusPort.setBottle( yarp::os::Bottle("1") );
	printf("*** CONTROL RESTORED ***\n");
	haveControl = false;
}

void RobotFilter::setWaypoint()
{
	//printf("entering for loop! %d body parts\n",robot->numBodyParts());
	
	for (int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
	{
		//printf("RobotFilter setWayPoint(%d) called\n",bodyPart);
		stateObservers.at(bodyPart)->setWaypoint();
	}
}