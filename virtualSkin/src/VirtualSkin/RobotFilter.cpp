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
#include <time.h>

using namespace VirtualSkin;

RobotFilter::RobotFilter( bool visualize ) : /*emitCollisions(true),*/ robot(NULL), isOpen(false), cbFilters(), stateObservers(), callObservers(), responseObservers()
{
	stop_command.addVocab(VOCAB_SET);
	stop_command.addVocab(VOCAB_STOPS);
	
	if ( visualize )
	{ 
		skinWindow = new SkinWindow();
		skinWindow->show();
	}
}

RobotFilter::~RobotFilter()
{
	if ( skinWindow ) { delete skinWindow; }
	if ( isOpen ) { close(); }
}

void RobotFilter::setRobot( RobotModel::Robot* r )
{
	robot = r;
	collisionDetector.setRobot(r);
	
	if ( robot && skinWindow )
	{
		QObject::connect( robot, SIGNAL(appendedObject(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( robot, SIGNAL(outdatedDisplayList(int)),					skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		QObject::connect( robot, SIGNAL(changedState()),							skinWindow->glWidget, SLOT(update()) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),				robot, SLOT(callLists()) );
	}
}
void RobotFilter::setWorld( RobotModel::World *w )
{
	world = w;
	collisionDetector.setWorld(w);
	
	if ( world && skinWindow )
	{
		QObject::connect( world, SIGNAL(appendedObject(RobotModel::DisplayList*)),	skinWindow->glWidget, SLOT(addDisplayList(RobotModel::DisplayList*)) );
		QObject::connect( world, SIGNAL(outdatedDisplayList(int)),					skinWindow->glWidget, SLOT(removeDisplayList(int)) );
		QObject::connect( world, SIGNAL(changedState()),							skinWindow->glWidget, SLOT(update()) );
		QObject::connect( skinWindow->glWidget, SIGNAL(renderStuff()),				world, SLOT(callLists()) );
	}
}

void RobotFilter::close()
{
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
	//QVector<RobotModel::PartController*>::const_iterator n;
	//for (n = partControllers.begin(); n != partControllers.end(); ++n) {
	//	delete (*n);
	//}
	//partControllers.clear();

	isOpen = false;
}

void RobotFilter::takeControl()
{
	// first stop the robot
	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++)
	{
		cbFilters.at(bodyPart)->cutConnection(true);		// take control away from the user
		cbFilters.at(bodyPart)->injectCall(stop_command);	// stop the robot
	}

	// inform the user
	printf("*** COLLISION RECOVERY ***\n");
	statusPort.setBottle( yarp::os::Bottle("0") );
	
	// disarm the collision detector (so it doesn't complain during recovery)
	collisionDetector.disarm();
	
	// pure virtual handler
	collisionHandler();
	
	// rearm the collision detector
	collisionDetector.arm();
	
	// reopen the filter... 
	for ( int bodyPart = 0; bodyPart < robot->getNumBodyParts(); bodyPart++ ) {
		cbFilters.at(bodyPart)->cutConnection(false);
	}
	
	//inform the user
	statusPort.setBottle( yarp::os::Bottle("1") );
	printf("CONTROL RESTORED\n");
	
}

