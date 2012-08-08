/*
 *  simSyncer.cpp
 *  virtualSkin
 *
 *  Created by Leo Pape on 16/08/12.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <QColor>
#include "simSyncer.h"
#include "modelexception.h"
#include "virtualskinexception.h"
#include "constants.h"
#include "compositeobject.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

using namespace VirtualSkin;

SimSyncer::SimSyncer(VirtualSkin::YarpModel* m, double p) : keepRunning(false), model(m), period(p)
{
}

SimSyncer::~SimSyncer()
{
}

void SimSyncer::open( const QString& portName, const QString& simPortName )
{
	//if ( isOpen ) { close(); }
	if ( !model ) { throw VirtualSkinException("SimSyncer must have a valid YarpModel."); }
	if ( !yarp::os::Network::checkNetwork() ) { throw( VirtualSkinException("yarp network unavailable...") ); }
	
	port.open(portName.toStdString().c_str());
	if(!port.addOutput(simPortName.toStdString().c_str())) {
		printf("\n\nWarning: could not connect to simulator world port for syncing\n\n");
	}
	keepRunning = true;
	start();
	printf("\n\nSimSyncer successfully started\n\n");
}

void SimSyncer::close()
{
	printf("Closing SimSyncer ");
	keepRunning = false;
	port.interrupt();
	while ( isRunning() )
	{ 
		printf("."); 
		msleep(1); 
	}
	printf("\n");
	port.close();
}

void SimSyncer::run()
{
	yarp::os::Bottle cmd,response;
	cmd.fromString("world get all");

	while ( keepRunning ) {
		if (port.getOutputCount() > 0) {
			response.clear();
			port.write(cmd, response);
			handler(response);
		}
		msleep(period);
	}
}

bool SimSyncer::handler( const yarp::os::Bottle& reply ) {
	bool moved = false, doUpdate = false, rtok = false, success = false;
	int n = 0;	// identifier of the current bottle element
	yarp::os::Bottle empty;
	yarp::os::Bottle* line;
	KinematicModel::CompositeObject* object;
	QMatrix4x4 rt, d;
	std::string name;
	
	if (reply.get(0).asInt() != VOCAB_FAIL) {
		for (int i=0; i<reply.size(); i++) {
			n = 0;
			empty.clear();

			line = reply.get(i).asList(); // get bottle for current object
			if (!line) { continue; }
			object = model->getRpcInterface().getObject(*line, empty, n); // get object pointer
			if (!object) { continue; }
			name = (object) ? object->getName().toStdString().c_str() : ""; // get object name
			rtok = model->getRpcInterface().parseSimRTBottle(name, *line, n, rt); // parse simulator bottle into RT matrix
			if (!rtok)  { continue; }
			
			// check if the object has moved more than 1e-4 meters or degrees
			d = object->getT() - rt;
			moved = false;
			for (int row=0; row<4; row++) {
				for (int col=0; col<4; col++) {
					moved = moved || (abs(d(row, col)) > 1e-4);
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

	return true;
}

