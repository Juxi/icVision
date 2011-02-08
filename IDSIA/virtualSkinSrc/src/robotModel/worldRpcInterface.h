/*
 *  vectorRpcPort.h
 *  virtualSkin
 *
 *  Created by Kail Frank on 11/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WORLDRPCINTERFACE_H
#define WORLDRPCINTERFACE_H

#include <QVector3D>
#include <yarp/os/all.h>
#include "yarpRpcPort.h"
#include <iostream>

class World;
class CompositeObject;
class PrimitiveObject;

class WorldRpcInterface: public YarpRpcPort
{
	Q_OBJECT
		
public:

	// commands for objects
	#define VOCAB_MK VOCAB2('m','k')
	#define VOCAB_SET VOCAB3('s','e','t')
	#define VOCAB_ROT VOCAB3('r','o','t')
	#define VOCAB_REM VOCAB2('r','m')
	
	// commands for primitives
	#define VOCAB_APP VOCAB3('a','p','p')
	#define VOCAB_PSET VOCAB4('s','e','t','p')
	#define VOCAB_PROT VOCAB4('r','o','t','p')
	#define VOCAB_PREM VOCAB3('r','m','p')

	// geometries
	#define VOCAB_CSG VOCAB3('c','s','g')
	#define VOCAB_SPH VOCAB3('s','p','h')
	#define VOCAB_CYL VOCAB3('c','y','l')
	#define VOCAB_BOX VOCAB3('b','o','x')
	#define VOCAB_SSPH VOCAB4('s','s','p','h')
	#define VOCAB_SCYL VOCAB4('s','c','y','l')
	#define VOCAB_SBOX VOCAB4('s','b','o','x')
	
	// admin
	#define VOCAB_CLEAR VOCAB3('c','l','r')
	
	WorldRpcInterface();
	virtual ~WorldRpcInterface();
	
	void setWorld( World* aWorld ) { world = aWorld; }
	
signals:
	void requestDeleteObject( CompositeObject* object );
	void requestDeletePrimitive( CompositeObject* object, PrimitiveObject* primitive );
	void requestClearWorld();

private:
	virtual bool handler( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply );
	
	void make( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	void setPos( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	void pSetPos( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	void setRot( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	void pSetRot( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	void append( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	
	void removeObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	void removePrimitive( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	
	void clearWorld() { /*world->clear();*/ }
	
	CompositeObject* getObject( const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	PrimitiveObject* getPrimitive( const CompositeObject* Object, const yarp::os::Bottle& cmd, yarp::os::Bottle& reply, int& n  );
	QString getName( const yarp::os::Bottle& cmd, int& n  );
	
	World* world;
};

#endif WORLDRPCINTERFACE_H
