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
#include "constants.h"

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <time.h>

using namespace VirtualSkin;

RobotFilter::RobotFilter( bool visualize ) : model(visualize), isOpen(false), haveControl(false)
{
	QObject::connect( &model, SIGNAL(collision()),	this, SLOT(takeControl()) );
	
	stop_command.addVocab(VOCAB_SET);
	stop_command.addVocab(VOCAB_STOPS);

	statusPort.setBottle("0");
}

RobotFilter::~RobotFilter()
{
	if ( isOpen ) { close(); }
}

void RobotFilter::close()
{
	model.stop();
	model.robot.close();
	model.world.clear();
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

	isOpen = false;
}

void RobotFilter::takeControl()
{
	if ( !haveControl )
	{
		haveControl = true;
		start();
	}
}

void RobotFilter::run()
{
	// first stop the robot
	for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
	{
		cbFilters.at(bodyPart)->cutConnection(true);		// take control away from the user
	}
	
	// inform the user
	printf("*** COLLISION RECOVERY ***\n");
	statusPort.setBottle( yarp::os::Bottle("0") );
	
	// pure virtual response to collision
	collisionResponse();
	
	// reopen the filter... 
	for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++ ) {
		cbFilters.at(bodyPart)->cutConnection(false);
	}
	
	//inform the user
	statusPort.setBottle( yarp::os::Bottle("1") );
	printf("CONTROL RESTORED\n");
	haveControl = false;
}
