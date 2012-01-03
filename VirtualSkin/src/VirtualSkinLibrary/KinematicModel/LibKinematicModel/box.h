/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef BOX_H
#define BOX_H

#include "primitiveobject.h"
#include "modelexception.h"

namespace KinematicModel { class Box; }

/** \brief Represents a box using SOLID and OpenGL
 *
 * This class creates a box (6-sided right polyhedron) of variable length, width and height. The local coordinate system is located at the centroid of the box,
 * and it is oriented such that the box is axis-aligned.
 */
class KinematicModel::Box : public PrimitiveObject
{
public:
    Box( const QVector3D& size, bool lines = false ) throw(KinematicModelException);	//!< Creates the SOLID object
									/**< \param size 3D list of side lengths of the box in x, y and z respectively */
    ~Box();							//!< Destroys the SOLID object

	qreal getX() { return x; }
	qreal getY() { return y; }
	qreal getZ() { return z; }
	
	//DT_ObjectHandle solidify() { return solidObject = DT_CreateObject(this, solidShape = DT_NewBox(x,y,z)); }

	GL_DisplayList makeDisplayList();			//!< Make an OpenGL display list for the box using 6 GL_QUADS

private:
    qreal x, y, z;					//!< Side lengths of the box in the x, y and z directions
	bool withLines;
};

#endif

/** @} */
