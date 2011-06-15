/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef WORLD_H
#define WORLD_H

#include <QThread>
#include <QMutex>
#include <QVector>

#include "renderList.h"
#include "compositeObject.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"

namespace RobotModel
{ 
	class Model;
	class World;
}

/*! \brief A list of objects that define the Robots environment
 *
 * The World is compatible with the world of the iCub simulator in terms of the kinds of geometries supported as well as the
 * way those geometries are named.  However in addition to individual primitives, the World supports CompositeObjects, made up of unions of PrimitiveObjects.
 */
class RobotModel::World : public RenderList
{

public:
	
	World( Model *m );	//!< Initializes counters for naming purposes
	~World();	//!< Deletes all objects in the World
	
	CompositeObject* newSphere( double r, const QVector3D& pos );				//!< Creates a new sphere and names it by appending the value of numSpheres to the identifier 'sph'
	CompositeObject* newSSphere( double r, const QVector3D& pos );				//!< Creates a new sphere and names it by appending the value of numSSpheres to the identifier 'ssph'
	CompositeObject* newCylinder( double r, double h, const QVector3D& pos );	//!< Creates a new cylinder and names it by appending the value of numCylinders to the identifier 'cyl'
	CompositeObject* newSCylinder( double r, double h, const QVector3D& pos );	//!< Creates a new cylinder and names it by appending the value of numSCylinders to the identifier 'scyl'
	CompositeObject* newBox( const QVector3D& size, const QVector3D& pos );		//!< Creates a new box and names it by appending the value of numBoxes to the identifier 'box'
	CompositeObject* newSBox( const QVector3D& size, const QVector3D& pos );	//!< Creates a new box and names it by appending the value of numSBoxes to the identifier 'sbox'
	CompositeObject* newObject( const QString& name, const QVector3D& pos );	//!< Creates a new empty CompositeObject
	
	CompositeObject* append( CompositeObject* obj );			//!< Appends an existing CompositeObject to the World
																/**< To append a primitive to a composite object, see CompositeObject.append( PrimitiveObject* ) */
	CompositeObject* getObjectByName( const QString& name );	//!< Returns a pointer to a CompositeObject in the world by looking up its name
	
	bool remove( CompositeObject* object );											//!< Removes a CompositeObject from the list and deletes it
	bool removePrimitive( CompositeObject* object, PrimitiveObject* primitive );	//!< Removes a PrimitiveObject from one of the CompositeObjects
																					/**< This is implemented in World because we need to emit the signal outdatedDisplayList( int )
																						 to inform GLWidget that the PrimitiveObject is gone */	
	void clear();																	//!< Deletes everything in the World and resets counters
																					/**< This returns the World to the state it was in just after construction */

	int size() { return objectList.size(); }
	
	QVector<QString> getList();	//!< Get a list of the objects in the world model
private:
	
	void render();		//!< Calls display lists for all objects in the World using glCallLists( int )

	Model				*model;				//!< The Model that is doing collision detection on the World
	DT_ResponseClass	worldResponseClass;	//!< A response class for collision detection where nothing is compared
	
	QVector<CompositeObject*> objectList;	//!< The list of objects in the World
	
	int numSpheres,		//!< A counter for spheres (counting begins with '1')
		numCylinders,	//!< A counter for cylinders (counting begins with '1')
		numBoxes,		//!< A counter for boxes (counting begins with '1')
		numSSpheres,	//!< A counter for sspheres (counting begins with '1')
						/**< Sspheres are included for compatibility with the iCub simulator, however they 
							 are functionally equivalent to normal spheres as this model does not include dynamics */
		numSCylinders,	//!< A counter for scylinders (counting begins with '1')
						/**< Scylinders are included for compatibility with the iCub simulator, however they 
							 are functionally equivalent to normal cylinders as this model does not include dynamics */
		numSBoxes;		//!< A counter for sboxes (counting begins with '1')
						/**< Sboxes are included for compatibility with the iCub simulator, however they 
							 are functionally equivalent to normal boxes as this model does not include dynamics */
	QMutex mutex;		//!< To prevent reading past the end of objectList

};

#endif

/** @} */
