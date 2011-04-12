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

RobotFilter::RobotFilter( bool visualize ) : model(visualize), isOpen(false), haveControl(false), isColliding(false)
{
	QObject::connect( &model, SIGNAL(collisions(int)),	this, SLOT(takeControl(int)) );
	QObject::connect( this, SIGNAL(responseCompleteReturns()),	this, SLOT(openFilter()) );
	
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
	statusPort.close();
	
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

void RobotFilter::takeControl( int collisions )
{
	if ( collisions > 0 ) { isColliding = true; }
	else { isColliding = false; }
	
	if ( isColliding && !haveControl )
	{
		haveControl = true;
		
		// first take control away from the user
		for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++)
		{
			cbFilters.at(bodyPart)->cutConnection(true);
		}
		
		// inform the user
		printf("*** COLLISION RECOVERY ***\n");
		statusPort.setBottle( yarp::os::Bottle("0") );
		
		// should be a position move ?!?
		collisionResponse();
		
		// wait for the response (a position move) to finish
		start();
	}
}

void RobotFilter::run()
{	
	//wait for the commands issued in collisionResponse() to finish
	responseComplete();
	emit responseCompleteReturns();
}

void RobotFilter::openFilter()
{
	//if ( !isColliding )
	//{
		// reinitialize the pose buffer with the current pose
		for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++ )
		{
			stateObservers.at(bodyPart)->initPoseBuffer( stateObservers.at(bodyPart)->currentPose() );
		}
		
		// reopen the filter... 
		for ( int bodyPart = 0; bodyPart < model.robot.numBodyParts(); bodyPart++ )
		{
			cbFilters.at(bodyPart)->cutConnection(false);
		}
		
		//inform the user
		statusPort.setBottle( yarp::os::Bottle("1") );
		printf("CONTROL RESTORED\n");
		haveControl = false;
	//}
	//else
	//{
	//	printf("No safe pose in the buffer. Consider increasing POSE_BUFFER_SIZE in VirtualSkinLibrary/constants.h \n");
	//}
}
