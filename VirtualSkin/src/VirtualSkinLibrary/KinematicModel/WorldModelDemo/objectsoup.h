/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup WorldModelDemo
 *	@{
 */

#ifndef OBJECTSOUP_H_
#define OBJECTSOUP_H_

#include <QThread>
#include "model.h"

//class KinematicModel::Model;

class ObjectSoup : public QThread
{
	
public:
	ObjectSoup( KinematicModel::Model& m, int n, bool v = false ) : theModel(m), num(n), verbose(v), keepRunning(true) {}
	~ObjectSoup() {}
	
	void stop();
	
private:
	KinematicModel::Model& theModel;	// the world model	
	int num;							// number of CompositeObjects in the model
	bool verbose;
	bool keepRunning;
	//int i,j;							// loop indices
	//int numPrimitives;					// the number of primitives in each CompositeObject (randomly selected for each CompostieObject)
	//KinematicModel::GeomType geomType;	// the type of primitive (randomly selected for each primitive)
	//GLfloat color[4];					// the color of each primitive
	
	void run();
	KinematicModel::CompositeObject* makeARandomObjectLikeAMothaFucka();
	QVector3D randomTranslation( float n );
	QVector3D randomRotation();
};

#endif
/** @} */