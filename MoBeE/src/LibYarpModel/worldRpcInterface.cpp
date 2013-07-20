/*
 *  vectorStreamPort.cpp
 *  MoBeE
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <QColor>
#include "worldRpcInterface.h"
#include "modelexception.h"
#include "exception.h"
#include "yarpModel.h"
#include "constants.h"
#include "compositeobject.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

using namespace MoBeE;

WorldRpcInterface::WorldRpcInterface() : keepRunning(false), s(0), c(0), b(0), ss(0), sc(0), sb(0), model(NULL)
{
	
}
WorldRpcInterface::~WorldRpcInterface()
{
}

void WorldRpcInterface::open( const QString& portName )
{
	//if ( isOpen ) { close(); }
	if ( !model ) { throw MoBeEException("WorldRpcInterface must have a valid KinematicModel. Use setModel(KinematicModel*)"); }
	if ( !yarp::os::Network::checkNetwork() ) { throw( MoBeEException("yarp network unavailable...") ); }
	
	port.open(portName.toStdString().c_str());
	keepRunning = true;
	start();
}

void WorldRpcInterface::close()
{
	printf("Closing WorldRpcInterface ");
	keepRunning = false;
	port.interrupt();
	while ( isRunning() )
	{ 
		printf("."); 
		msleep(YARP_PERIOD_ms); 
	}
	printf("\n");
	port.close();
}

void WorldRpcInterface::run()
{
	yarp::os::Bottle cmd,response;
	
	while ( keepRunning )
	{
		port.read(cmd,true);
		response.clear();
		if ( cmd.size() > 0 )
		 {
		 //if (debug) { showBottle(cmd); }
		 handler(cmd,response);
		 port.reply(response);
		 if (!model->isRunning()) {
			 model->computePose();
		 }
		 //if (debug) { printf("reply: %s\n",response.toString().c_str()); }
		 }
		//msleep(YARP_PERIOD); this doesn't do anything as msleep takes unsigned long
	}
}

bool WorldRpcInterface::handler( const yarp::os::Bottle& command, yarp::os::Bottle& reply ) 
{
	int n = 0;	// identifier of the current bottle element
	int cmd;	// the command (see command vocabs in header)
	
	yarp::os::ConstString prefix = command.get(n).asString();
	if ( prefix=="help" )
	{
			reply.addVocab(yarp::os::Vocab::encode("many"));
			reply.addString("\nMoBeE world interface, arguments within brackets: \n");
			reply.addString(	"ls: list objects");
			reply.addString(	"mk sph [radius] [xpos] [ypos] [zpos]: create sphere");
			reply.addString(	"mk cyl [radius] [height] [xpos] [ypos] [zpos]: create sphere");
			reply.addString(	"mk box [xsize] [ysize] [zsize] [xpos] [ypos] [zpos]: create sphere");
			reply.addString(	"set [objectname] [xpos] [ypos] [zpos]: set object location (m)");
			reply.addString(	"def [objectname] [targ/obs]: set object class to target or obstacle");
			reply.addString(	"get [objectname]: return object state");
			reply.addString(	"rot [objectname] [xrot] [yrot] [zrot]: set object rotation (degrees)");
			reply.addString(    "rot [objectname] [1*9 rotation matrix]: set object rotation cosine matrix");
			reply.addString(	"rm [objectname]: remove object (persistent objects cannot be removed)");
			reply.addString(	"clr: remove all but persistent objects from the world, and reset object counters");
			reply.addString(	"grab [objectname] [robotname] [markername]: attach object to robot marker");
			reply.addString(	"grab [objectname] [robotname]: detach object from robot");
			reply.addString("\niCub simulator synchronization commands:");
			reply.addString(    "sim [objectname] [xpos] [ypos] [zpos] [xrot] [yrot] [zrot]: set rototranslation as returned in iCubSim coordinates");
			reply.addString(	"srun [period]: run iCubSim synchronization thread");
			reply.addString(	"sstp: stop iCubSim synchronization thread");
			reply.addString(	"sync: do one iCubSim synchronization step");
		return true;
	}
	else if ( prefix == "ls, mk (sph, cyl, box), set, def (obs/tgt), get, rot, rm, clr, grab, sim, srun, sstp, sync" ) { n++; }
	
	cmd  = command.get(n).asVocab(); n++;

	switch (cmd)
	{
		case VOCAB_LS:	getList(reply); break;
		case VOCAB_MK:	make(command,reply,n); break;
		case VOCAB_SET:	set(command,reply,n); break;
		case VOCAB_DEF: respClass(command,reply,n); break;
		case VOCAB_GET:	getState(command,reply,n); break;
		case VOCAB_ROT:	setRot(command,reply,n); break;
		case VOCAB_REM:	removeObject(command,reply,n); break;
		case VOCAB_GRAB: grabObject(command,reply,n); break;
		case VOCAB_SIM: setRTfromSim(command, reply, n); break;
		case VOCAB_SIMSYNC_RUN: startSimSyncer(command, reply, n); break;
		case VOCAB_SIMSYNC_STOP: model->getSimSyncer().stop(); reply.addString("ok"); break;
		case VOCAB_SIMSYNC_NOW: model->getSimSyncer().step(); reply.addString("ok"); break;
		case VOCAB_CLEAR: 
				printf("CLEARING THE WORLD\n");
				model->clearTheWorld();
				s = 0; c = 0; b = 0; ss = 0; sc = 0; sb = 0; // set counters to 0, for icub simulator compatibility
				reply.addString("Removed all world objects");
				printf("FINISHED CLEARING THE WORLD\n");
				break;
			
		default: reply.addString("Unknown RPC command"); return false;
	}
	return true;
}

void WorldRpcInterface::getList(yarp::os::Bottle& reply)
{
	//printf("called getList\n");
	QVector<QString>::iterator i;
	QVector<QString> list = model->listWorldObjects();
	for ( i=list.begin(); i!=list.end(); ++i )
	{
		reply.addString( (*i).toStdString().c_str() );
    }
}

void WorldRpcInterface::removeObject( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	KinematicModel::CompositeObject* object = getObject( command, reply, n );
	if ( object )
	{ 
		object->kill();
		reply.addString("Removed object");
	}
}

void WorldRpcInterface::make( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n )
{
	int geom = command.get(n).asVocab();
	n++;
	 
	try {
		KinematicModel::CompositeObject* composite = new KinematicModel::CompositeObject( model->OBSTACLE(), model->FIELD() );
		KinematicModel::PrimitiveObject* primitive;
		if ( geom == VOCAB_SPH || geom == VOCAB_SSPH ) {
			
			double r = command.get(n).asDouble(); n++; // radius 
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			
			primitive = new KinematicModel::Sphere( r );
			composite->appendPrimitive( primitive );
			composite->setPosition( QVector3D(px,py,pz) );
            
            QString name;
			if ( geom == VOCAB_SPH ) { name = "sph" + QString::number(++s); }
			else if ( geom == VOCAB_SSPH ) { name = "ssph" + QString::number(++ss); }
            composite->setName(name);
            //printf("%s\n",name.toLocal8Bit().data());
			reply.addString( name.toLocal8Bit().data() );
			
		} else if ( geom == VOCAB_CYL || geom == VOCAB_SCYL ) {
			
			double r = command.get(n).asDouble(); n++; // radius
			double h = command.get(n).asDouble(); n++; // height 
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			
			//if ( geom == VOCAB_CYL ) { world->newCylinder( r, h, QVector3D(px,py,pz) ); }
			//else if ( geom == VOCAB_SCYL ) { world->newSCylinder( r, h, QVector3D(px,py,pz) ); }
			
			primitive = new KinematicModel::Cylinder( r, h );
			composite->appendPrimitive( primitive );
			composite->setPosition( QVector3D(px,py,pz) );
			
			QString name;
            if ( geom == VOCAB_CYL ) { name = "cyl" + QString::number(++c); }
			else if ( geom == VOCAB_SCYL ) { name = "scyl" + QString::number(++sc); }
            composite->setName(name);
			reply.addString( name.toLocal8Bit().data() );
			
		} else if ( geom == VOCAB_BOX || geom == VOCAB_SBOX ) {
			
			double x = command.get(n).asDouble(); n++;  // x size
			double y = command.get(n).asDouble(); n++;  // y size  
			double z = command.get(n).asDouble(); n++;  // z size
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position

			//if ( geom == VOCAB_BOX ) { world->newBox( QVector3D(x,y,z), QVector3D(px,py,pz) ); }
			//else if ( geom == VOCAB_SBOX ) { world->newSBox( QVector3D(x,y,z), QVector3D(px,py,pz) ); }
			
			primitive = new KinematicModel::Box( QVector3D(x,y,z) );
			composite->appendPrimitive( primitive );
			composite->setPosition( QVector3D(px,py,pz) );
            
            QString name;
            if ( geom == VOCAB_BOX ) { name = "box" + QString::number(++b); }
			else if ( geom == VOCAB_SBOX ) { name = "sbox" + QString::number(++sb); }
            composite->setName(name);
			reply.addString( name.toLocal8Bit().data() );
		}
		else { reply.addString("MK ERROR: Unknown geometry... use '(s)sph', '(s)cyl', or '(s)box'"); }
		
		model->appendObject( composite );
	}
	catch (std::exception& e)
	{
		reply.clear();
		reply.addString(e.what());
	}
}

void WorldRpcInterface::set( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	KinematicModel::CompositeObject* object = getObject( command, reply, n );
	
	if ( object )
	{
		double x = command.get(n).asDouble(); n++;  //std::cout << x << std::endl; // x position
		double y = command.get(n).asDouble(); n++;  //std::cout << y << std::endl; // y position  
		double z = command.get(n).asDouble(); n++;  //std::cout << z << std::endl; // z position
		object->setPosition( QVector3D(x,y,z) );
		reply.addString("Set Cartesian position of object.");
	}
}

void WorldRpcInterface::setRTfromSim(const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n) {
	KinematicModel::CompositeObject* object = getObject( command, reply, n );
	if ( object ) {
		QMatrix4x4 rt;
		std::string name = object->getName().toStdString();
		if(parseSimRTBottle(name, command, n, rt)) {
			object->setT(rt);
		} else {
			reply.addString("parseSimRTBottle: invalid number of arguments supplied");
		}

	}
}

void WorldRpcInterface::startSimSyncer(const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n) {
	if ((command.size() - n) != 1) {
		reply.addString("Please provide the refresh period for the synchronization thread in seconds");
		return;
	}
	double period = command.get(n).asDouble(); n++;
	if (model->getSimSyncer().isRunning()) {
		model->getSimSyncer().stop();
	}
	model->getSimSyncer().setRefreshPeriod(period);
	model->getSimSyncer().start();
	reply.addString("ok");
}


bool WorldRpcInterface::parseSimRTBottle(const std::string name, const yarp::os::Bottle& command, int& n, QMatrix4x4 &rt) {
	if ( (command.size()-n) != 6) {
		return false;
	}

	//position
	double px = command.get(n).asDouble(); n++;
	double py = command.get(n).asDouble(); n++;
	double pz = command.get(n).asDouble(); n++;
		
	// rotation
	double rx = command.get(n).asDouble(); n++;
	double ry = command.get(n).asDouble(); n++;
	double rz = command.get(n).asDouble(); n++;
		
	// special simulator rotation:
	QQuaternion qrx = QQuaternion::fromAxisAndAngle( QVector3D(1, 0, 0), -rz);
	QQuaternion qry = QQuaternion::fromAxisAndAngle( QVector3D(0, 1, 0), -rx);
	QQuaternion qrz = QQuaternion::fromAxisAndAngle( QVector3D(0, 0, 1), ry);
	rt.setToIdentity();
	rt.rotate(qry * (qrz*qrx));

	// cylinders are rotated 90 degrees on the z-axis with respect to iCubSIM:
	QRegExp rxtype("([^\\d]+)(?:\\s*\\d+)"); // check for: {[one or more non-numeric characters]: return as part 1} {[zero or more whitespace characters followed by one or more numeric characters]: do not return}
	int pos = rxtype.indexIn(QString(name.c_str()));
	if (pos > -1) {
		yarp::os::Value type(rxtype.cap(1).toStdString().c_str());
		int vtype = type.asVocab();
		if (vtype == VOCAB_CYL || vtype == VOCAB_SCYL) {
			rt.rotate( QQuaternion::fromAxisAndAngle( QVector3D(0, 0, 1), 90));
		}
	}
	
	// position translation
	rt(0, 3) = -(pz + 0.026);
	rt(1, 3) = -px;
	rt(2, 3) = py-0.5976;

	return true;
}


void WorldRpcInterface::setRot( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	KinematicModel::CompositeObject* object = getObject( command, reply, n );
	
	if ( object )
	{
		if ((command.size() - n) == 3) {
			double x = command.get(n).asDouble()*M_PI/180.; n++;  //std::cout << x << std::endl; // x position
			double y = command.get(n).asDouble()*M_PI/180.; n++;  //std::cout << y << std::endl; // y position  
			double z = command.get(n).asDouble()*M_PI/180.; n++;  //std::cout << z << std::endl; // z position
			object->setCartesianOrientation( QVector3D(x,y,z) );
			reply.addString("Set rotation (about x,y,z in degrees).");
		} else {
			// replace rotation part of object's T matrix
			QMatrix4x4 rt = object->getT();
			for (int i = 0; i<3; i++) {
				for (int j = 0; j<3; j++) {
					rt(i, j) = command.get(n).asDouble(); n++;
				}
			}
			object->setT(rt);
			reply.addString("Set full rotation matrix.");
		}

	}
}

void WorldRpcInterface::getState( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	KinematicModel::CompositeObject* object = getObject( command, reply, n );
	
	if ( object )
	{
		const qreal* T = object->getT().data();
		for ( int i = 0; i < 16; i++ )
		{
			reply.addDouble( T[i] );
		}
		//reply.addString("Set rotation (about x,y,z in degrees) of object.");
	}
}

void WorldRpcInterface::respClass( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	KinematicModel::CompositeObject* object = getObject( command, reply, n );
	
	if ( object )
	{
		//model->clearWorldObject(object);
        model->removeWorldObject(object);

		int type = command.get(n).asVocab();
		QColor collidingColor,freeColor;
		switch (type)
		{
			case VOCAB_OBSTACLE:
				
				freeColor = Qt::blue;
				freeColor = freeColor.lighter();

				collidingColor = freeColor;
                
                freeColor.setAlphaF(0.5);
				collidingColor.setAlphaF(0.5);
				
				object->setResponseClass(model->OBSTACLE());
				object->setFreeColor( freeColor );
				object->setCollidingColor( collidingColor );
				
				reply.addString("Changed object type to 'obstacle'.");
				
				break;
				
			case VOCAB_TARGET:
				
				freeColor = Qt::green;
				freeColor = freeColor.lighter();
				collidingColor = freeColor;
				
				freeColor.setAlphaF(1.0);
				collidingColor.setAlphaF(0.5);
				
				object->setResponseClass(model->TARGET());
				object->setFreeColor( freeColor );
				object->setCollidingColor( collidingColor );
	
				reply.addString("Changed object type to 'target'.");
				
				break;
                
            case VOCAB_GHOST:
				
				freeColor = Qt::white;
				freeColor.setAlphaF(0.3);
				collidingColor = freeColor;
				
				object->setResponseClass(model->GHOST());
				object->setFreeColor( freeColor );
				object->setCollidingColor( collidingColor );
                
				reply.addString("Changed object type to 'ghost'.");
				
				break;
				
			default:
				
				reply.addString("Unknown definition, use 'obs' or 'tgt'.");
		}
		
		model->appendObject( object );
	}
}


void WorldRpcInterface::grabObject( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	KinematicModel::CompositeObject* object = getObject(command, reply, n);
	if (!object)
		return;

	QString robotName = command.get(n).asString().c_str(); n++;
	KinematicModel::Robot *robot = model->getRobot(robotName);
	if (!robot) {
		reply.addString("Robot not found.");
		return;
	}
	reply.addString("Robot found.");

	QString markerName = command.get(n).asString().c_str(); n++;
	int markerIndex = -1;
	//KinematicModel::RobotObservation robotObs = robot->observe();
	//for (int i = 0; i<robotObs.getNumMarkers(); i++) {
	//	if (robotObs.markerName(i).compare(markerName) == 0)
	//		markerIndex = i;
	//}

	if (markerIndex < 0) {
		reply.addString("Marker not found, releasing object.");
	//	return;
	} else
		reply.addString("Marker found, grabbing object.");
 
	model->grabObject(object, robot, markerIndex);

	//printf("Grabbing object \"%s\" with marker \"%s\" on robot \"%s\"\n", object->getName().toStdString().c_str(), markerName.toStdString().c_str(), robotName.toStdString().c_str());
}


QString WorldRpcInterface::getName( const yarp::os::Bottle& command, int& n  )
{
	QString name;
	int unknownData = command.get(n).asVocab();
	if (command.get(n).isString() && command.get(n).asString().length() > 4) // asVocab returns only valid VOCABs for strings of 4 or less characters
		unknownData = -1;
	 
	// for primitive shapes build the name from the vocab and the following number
	if ( unknownData == VOCAB_SPH || unknownData == VOCAB_SSPH ||
		 unknownData == VOCAB_CYL || unknownData == VOCAB_SCYL ||
		 unknownData == VOCAB_BOX || unknownData == VOCAB_SBOX )
	{
		name = command.get(n).asString(); n++;
		name += QString::number( command.get(n).asInt() ); n++;
	}
	else // otherwise treat the vocab as a name directly
	{
		name = command.get(n).asString(); n++;
	}
	return name;
}

KinematicModel::CompositeObject* WorldRpcInterface::getObject( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	QString objName = getName( command, n );
	KinematicModel::CompositeObject* object = model->getObject( objName );
	if ( !object ) {
		reply.addString((QString("Object ") + objName + QString(" not found.")).toStdString().c_str());
		return 0;
	} else {
		reply.addString((QString("Object ") + objName + QString(" found.")).toStdString().c_str());
		return object;
	}
}
