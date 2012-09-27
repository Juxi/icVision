/*
 *  simSyncer.cpp
 *  MoBeE
 *
 *  Created by Leo Pape on 16/08/12.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <QColor>
#include "simSyncer.h"
#include "modelexception.h"
#include "exception.h"
#include "yarpModel.h"
#include "constants.h"
#include "compositeobject.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

using namespace MoBeE;

SimSyncer::SimSyncer() : keepRunning(false), model(NULL), period(YARP_PERIOD_ms)
{
	cmd.fromString("world get all"); // command for getting all objects from iCubSim
}


SimSyncer::~SimSyncer()
{
}


void SimSyncer::open( const QString& portName, const QString& simPortName )
{
	//if ( isOpen ) { close(); }
	if ( !model ) { throw MoBeEException("SimSyncer must have a valid YarpModel. Use setModel(YarpModel*)"); }
	if ( !yarp::os::Network::checkNetwork() ) { throw( MoBeEException("yarp network unavailable...") ); }
	
	port.open(portName.toStdString().c_str());
	if(!port.addOutput(simPortName.toStdString().c_str())) {
		printf("\n\nWarning: could not connect to simulator world port for syncing\n\n");
	}
	start();
	printf("\n\nSimSyncer successfully started\n\n");
}


void SimSyncer::close()
{
	stop();
	port.close();
}


void SimSyncer::stop() {
	printf("Stopping SimSyncer ");
	keepRunning = false;
	port.interrupt();
	while ( isRunning() ) { 
		printf("."); 
		msleep(YARP_PERIOD_ms); 
	}
	printf("\n");
}


void SimSyncer::run() {
	if (keepRunning) { printf("\n\n\niCubSim synchronization thread already running!\n\n\n"); return; }
	port.resume();
	keepRunning = true;
	QElapsedTimer t; t.start();
	while ( keepRunning ) {
		step();
		interruptableSleeper(period - t.elapsed()); t.start();
	}
}


void SimSyncer::interruptableSleeper(qint64 time) {
	if (time <=0) { return; }
	sleepTimer.start();
	while ( keepRunning && !sleepTimer.hasExpired(time)) {
		msleep(YARP_PERIOD_ms);
	}
}


void SimSyncer::step() {
	QMutexLocker locker(&mutex); // multiple threads may call step(), so protect it with a mutex
	 
	bool moved = false, doUpdate = false, rtok = false, success = false;
	int n = 0;	// identifier of the current bottle element
	yarp::os::Bottle empty;
	yarp::os::Bottle* line;
	KinematicModel::CompositeObject* object;
	QMatrix4x4 rt, d;
	std::string name;
	
	if (port.getOutputCount() > 0) {
		response.clear();
		port.write(cmd, response);
		
		if ((response.size() > 0) && (response.get(0).asVocab() != VOCAB_FAIL)) {
			for (int i=0; i<response.size(); i++) {
				n = 0;
				empty.clear();

				// parse line
				line = response.get(i).asList(); // get bottle for current object
				if (!line) { continue; }
				object = model->getRpcInterface().getObject(*line, empty, n); // get object pointer
				if (!object) { continue; }
				name = (object) ? object->getName().toStdString().c_str() : ""; // get object name
				rtok = model->getRpcInterface().parseSimRTBottle(name, *line, n, rt); // parse simulator bottle into RT matrix
				if (!rtok)  { continue; }
			
				// check if the object has moved more than 5e-4 meters or degrees
				d = object->getT() - rt;
				moved = false;
				for (int row=0; row<4; row++) {
					for (int col=0; col<4; col++) {
						moved = moved || (abs(d(row, col)) > 5e-4);
					}
				}

				// put object in place
				if(moved) {
					object->setT(rt);
				}
				doUpdate = doUpdate || moved;
			}
		}

		// if any of the objects moved, AND the model thread is not running, THEN new poses should be computed
		if (doUpdate && !model->isRunning()) {
			model->computePose();
		}
	}
}

