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

	statusPort.setBottle("1");
}

RobotFilter::~RobotFilter()
{
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
	if ( numReflexCollisions > 0 ) { isColliding = true; }
	else { isColliding = false; }
	
	if ( isColliding && !haveControl )
	{
		haveControl = true;
		
		// first take control away from the user
		for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++)
		{
			cbFilters.at(bodyPart)->cutConnection(true);
		}
		
		// inform the user
		printf("*** COLLISION RECOVERY ***\n");
		statusPort.setBottle( yarp::os::Bottle("0") );
		
		// do some control in response
		collisionResponse();
		
		// wait for the response to finish
		start();
	}
}

void RobotFilter::run()
{	
	//wait for the commands issued in collisionResponse() to finish
	responseComplete();
	
	if ( isColliding )
	{
		printf("No safe pose in the buffer. Consider increasing POSE_BUFFER_SIZE and/or ALL_CLEAR_WAIT in VirtualSkinLibrary/constants.h. Please resolve the situation manually, and the RobotFilter will re-open auto-magically! \n");

		/* */
		yarp::os::Bottle torso,head,arm;
		torso.addVocab(VOCAB_SET);
		torso.addVocab(VOCAB_POSITION_MOVES);
		yarp::os::Bottle& pTorso = torso.addList();
		pTorso.addDouble(0.0);
		pTorso.addDouble(0.0);
		pTorso.addDouble(0.0);
		
		head.addVocab(VOCAB_SET);
		head.addVocab(VOCAB_POSITION_MOVES);
		yarp::os::Bottle& pHead = head.addList();
		pHead.addDouble(0.0);
		pHead.addDouble(0.0);
		pHead.addDouble(0.0);
		pHead.addDouble(0.0);
		pHead.addDouble(0.0);
		pHead.addDouble(0.0);
		
		arm.addVocab(VOCAB_SET);
		arm.addVocab(VOCAB_POSITION_MOVES);
		yarp::os::Bottle& pArm = arm.addList();
		pArm.addDouble(-26.0);
		pArm.addDouble(20.0);
		pArm.addDouble(0.0);
		pArm.addDouble(50.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);
		pArm.addDouble(0.0);

		cbFilters.at(0)->injectCall(torso);
		cbFilters.at(1)->injectCall(head);
		cbFilters.at(2)->injectCall(arm);
		cbFilters.at(3)->injectCall(arm);
		
		printf("sent 'home' command\n");
		
	}
	while ( isColliding )
	{
		msleep(YARP_PERIOD_ms);
	}
	
	emit reflexDone();
}

void RobotFilter::openFilter()
{	
	// reinitialize the pose buffer with the current pose
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
	{
		stateObservers.at(bodyPart)->initPoseBuffer( stateObservers.at(bodyPart)->currentPose() );
	}
	
	// reopen the filter... 
	for ( int bodyPart = 0; bodyPart < robot->numBodyParts(); bodyPart++ )
	{
		cbFilters.at(bodyPart)->cutConnection(false);
	}
	
	//inform the user
	statusPort.setBottle( yarp::os::Bottle("1") );
	printf("CONTROL RESTORED\n");
	haveControl = false;
}
