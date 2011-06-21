/*
 *  vectorStreamPort.cpp
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "worldRpcInterface.h"
#include "world.h"
#include "robotmodelexception.h"
#include "constants.h"

using namespace VirtualSkin;

WorldRpcInterface::WorldRpcInterface()
{
}
WorldRpcInterface::~WorldRpcInterface()
{
}

bool WorldRpcInterface::handler( const yarp::os::Bottle& command, yarp::os::Bottle& reply ) 
{
	int n = 0;	// identifier of the current bottle element
	int cmd;	// the command (see command vocabs in header)
	
	yarp::os::ConstString prefix = command.get(n).asString();
	if (prefix=="help") {
		reply.addString("No help for YOU!!!");
		return true;
	}
	else if (prefix == "world") { n++; }
	
	cmd  = command.get(n).asVocab(); n++;

	switch (cmd)
	{
		case VOCAB_LS:	getList(reply); break;
		case VOCAB_MK:  make(command,reply,n); break;
		case VOCAB_SET: setPos(command,reply,n); break;
		case VOCAB_GET: getState(command,reply,n); break;
		case VOCAB_ROT: setRot(command,reply,n); break;
		case VOCAB_REM: removeObject(command,reply,n); break;

		case VOCAB_APP:  append(command,reply,n); break;
		case VOCAB_PSET: pSetPos(command,reply,n); break;
		case VOCAB_PROT: pSetRot(command,reply,n); break;
		case VOCAB_PREM: removePrimitive(command,reply,n); break;
			
		case VOCAB_CLEAR: clearWorld(); break;
			
		default: reply.addString("Unknown RPC command"); break;
	}
	//world->emit changedState();
	return true;
}
void WorldRpcInterface::getList(yarp::os::Bottle& reply)
{
	QVector<QString>::iterator i;
	QVector<QString> objectNames = world->getList();
	for ( i=objectNames.begin(); i!=objectNames.end(); ++i )
	{
		reply.addString( (*i).toStdString().c_str() );
    }
}
void WorldRpcInterface::make( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n )
{
	int geom = command.get(n).asVocab();
	n++;
	 
	try {
		if ( geom == VOCAB_SPH || geom == VOCAB_SSPH ) {
			
			double r = command.get(n).asDouble(); n++; // radius 
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			
			if ( geom == VOCAB_SPH ) { world->newSphere( r, QVector3D(px,py,pz) ); }
			else if ( geom == VOCAB_SSPH ) { world->newSSphere( r, QVector3D(px,py,pz) ); }
			
			reply.addString("Made sphere.");
			
		} else if ( geom == VOCAB_CYL || geom == VOCAB_SCYL ) {
			
			double r = command.get(n).asDouble(); n++; // radius
			double h = command.get(n).asDouble(); n++; // height 
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			
			if ( geom == VOCAB_CYL ) { world->newCylinder( r, h, QVector3D(px,py,pz) ); }
			else if ( geom == VOCAB_SCYL ) { world->newSCylinder( r, h, QVector3D(px,py,pz) ); }
			
			reply.addString("Made cylinder.");
			
		} else if ( geom == VOCAB_BOX || geom == VOCAB_SBOX ) {
			
			double x = command.get(n).asDouble(); n++;  // x size
			double y = command.get(n).asDouble(); n++;  // y size  
			double z = command.get(n).asDouble(); n++;  // z size
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position

			if ( geom == VOCAB_BOX ) { world->newBox( QVector3D(x,y,z), QVector3D(px,py,pz) ); }
			else if ( geom == VOCAB_SBOX ) { world->newSBox( QVector3D(x,y,z), QVector3D(px,py,pz) ); }
			
			reply.addString("Made box.");
			
		}
		else if ( geom == VOCAB_CSG ) {
			
			QString name = getName( command, n );
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			
			if ( !world->getObjectByName( name ) ) {
				
				world->newObject( name, QVector3D(px,py,pz) );
				reply.addString("Made csg object.");
				
			}
			else { reply.addString("That name is taken!"); }
		}
		else { reply.addString("MK ERROR: Unknown geometry... use '(s)sph', '(s)cyl', '(s)box' or 'csg'"); }
	}
	catch (RobotModel::RobotModelException e) { reply.clear(); reply.addString(e.what()); }
}
void WorldRpcInterface::append( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );

	RobotModel::PrimitiveObject* primitive;
	int geom = command.get(n).asVocab(); n++;
	
	try
	{
		if ( geom == VOCAB_SPH || geom == VOCAB_SSPH )
		{
			double r = command.get(n).asDouble(); n++;  // radius 
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			QVector3D pos = QVector3D(px,py,pz);

			primitive = object->newSphere(r, pos);
		}
		else if ( geom == VOCAB_CYL || geom == VOCAB_SCYL )
		{
			double r = command.get(n).asDouble(); n++;  // radius
			double h = command.get(n).asDouble(); n++;  // height 
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			QVector3D pos = QVector3D(px,py,pz);
			
			primitive = object->newCylinder( r, h, pos );
		}
		else if ( geom == VOCAB_BOX || geom == VOCAB_SBOX )
		{
			double x = command.get(n).asDouble(); n++;  // x size
			double y = command.get(n).asDouble(); n++;  // y size  
			double z = command.get(n).asDouble(); n++;  // z size
			QVector3D size = QVector3D(x,y,z);
			
			double px = command.get(n).asDouble(); n++; // x position
			double py = command.get(n).asDouble(); n++; // y position  
			double pz = command.get(n).asDouble(); n++; // z position
			QVector3D pos = QVector3D(px,py,pz);
			
			primitive = object->newBox( size, true, pos );
		}
		else { reply.addString("Cannot append to object: Unknown Primitive... use 'sph', 'cyl', 'box'"); }
			
		//world->filterCollisions( primitive );
		object->append(primitive);
		
		reply.addString("Appended ");
		reply.addString( primitive->getName().toStdString().c_str() );
		reply.addString("to");
		reply.addString( object->getName().toStdString().c_str() );
			
	} catch (RobotModel::RobotModelException e) { reply.addString(e.what()); }
}
void WorldRpcInterface::setPos( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );
	
	if ( object )
	{
		double x = command.get(n).asDouble(); n++;  //std::cout << x << std::endl; // x position
		double y = command.get(n).asDouble(); n++;  //std::cout << y << std::endl; // y position  
		double z = command.get(n).asDouble(); n++;  //std::cout << z << std::endl; // z position
		object->setPosition( QVector3D(x,y,z) );
		reply.addString("Set Cartesian position of object.");
	}
}
void WorldRpcInterface::pSetPos( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );
	RobotModel::PrimitiveObject* primitive = getPrimitive( object, command, reply, n );

	if ( primitive )
	{
		double x = command.get(n).asDouble(); n++;  //std::cout << x << std::endl; // x position
		double y = command.get(n).asDouble(); n++;  //std::cout << y << std::endl; // y position  
		double z = command.get(n).asDouble(); n++;  //std::cout << z << std::endl; // z position
		primitive->setPosition( QVector3D(x,y,z) );
		reply.addString("Set Cartesian position of primitive relative to object.");
	}
}
void WorldRpcInterface::getState( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );
	
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
void WorldRpcInterface::setRot( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );
	
	if ( object )
	{
		double x = command.get(n).asDouble(); n++;  //std::cout << x << std::endl; // x position
		double y = command.get(n).asDouble(); n++;  //std::cout << y << std::endl; // y position  
		double z = command.get(n).asDouble(); n++;  //std::cout << z << std::endl; // z position
		object->setCartesianOrientation( QVector3D(x,y,z) );
		reply.addString("Set rotation (about x,y,z in degrees) of object.");
	}
}
void WorldRpcInterface::pSetRot( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );
	RobotModel::PrimitiveObject* primitive = getPrimitive( object, command, reply, n );
	
	if ( primitive )
	{
		double x = command.get(n).asDouble(); n++;  //std::cout << x << std::endl; // x position
		double y = command.get(n).asDouble(); n++;  //std::cout << y << std::endl; // y position  
		double z = command.get(n).asDouble(); n++;  //std::cout << z << std::endl; // z position
		primitive->setCartesianOrientation( QVector3D(x,y,z) );
		reply.addString("Set rotation (about x,y,z in degrees) of primitive relative to object.");
	}
}

void WorldRpcInterface::removeObject( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );

	if ( object )
	{ 
		world->remove( object );
		//emit requestDeleteObject( object );
		reply.addString("Removed object");
	}
}

void WorldRpcInterface::removePrimitive( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	RobotModel::CompositeObject* object = getObject( command, reply, n );
	RobotModel::PrimitiveObject* primitive = getPrimitive( object, command, reply, n );
	
	if ( primitive )
	{
		world->removePrimitive(object, primitive );
		//emit requestDeletePrimitive( object, primitive );
		reply.addString("Requested Delete primitive");
	}
}	

void WorldRpcInterface::clearWorld()
{
	world->clear();
}

RobotModel::CompositeObject* WorldRpcInterface::getObject( const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	QString objName = getName( command, n );
	RobotModel::CompositeObject* object = world->getObjectByName( objName );
	if ( !object ) {
		reply.addString("Object not found.");
		return 0;
	} else {
		reply.addString("Object found.");
		return object;
	}
}

RobotModel::PrimitiveObject* WorldRpcInterface::getPrimitive( const RobotModel::CompositeObject* object, const yarp::os::Bottle& command, yarp::os::Bottle& reply, int& n  )
{
	QString primName = getName( command, n );
	RobotModel::PrimitiveObject* primitive = object->getPrimitiveByName( primName );
	if ( !primitive ) {
		reply.addString("Primitive not found.");
		return 0;
	} else {
		reply.addString("Primitive found.");
		return primitive;
	}
}

QString WorldRpcInterface::getName( const yarp::os::Bottle& command, int& n  )
{
	QString name;
	int unknownData = command.get(n).asVocab();
	 
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
