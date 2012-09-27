/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup KinematicModel
 *	@{
 */

#ifndef CYLINDER_H
#define CYLINDER_H

#include "primitiveobject.h"
#include "modelexception.h"

namespace KinematicModel { class Cylinder; }

/** \brief Represents a cylinder using SOLID and OpenGL
 *
 * This class creates a cylinder of variable radius and length. The local coordinate system is located at the centroid of the cylinder, and
 * it is oriented such that the y direction is the cylinder's axis of symmetry.
 */
class KinematicModel::Cylinder : public PrimitiveObject
{
public:
    Cylinder( qreal radius, qreal height, int lod = 12, bool lines = false ) throw(KinematicModelException) ;	//!< Creates the SOLID object
																						/**< \param radius The radius of the cylinder
																							 \param height The height of the cylinder
																							 \param lod Level Of Detail - the number of line segments used to represent each circular edge of the cylinder */
    ~Cylinder();											//!< Destroys the SOLID object

	qreal r() { return radius; }
	qreal h() { return height; }
	//DT_ObjectHandle solidify() { return solidObject = DT_CreateObject(this, solidShape = DT_NewCylinder(radius,height)); }
	
    GL_DisplayList makeDisplayList();							//!< Make an OpenGL display list for the cylinder using GL_TRIANGLE_FAN and a GL_TRIANGLE_STRIP

private:
    qreal	radius,									//!< Radius of the cylinder
			height;									//!< Height of the cylinder
    int		lod;									//!< Level Of Detail - the number of line segments used to represent each circular edge of the cylinder
	bool	withLines;
};

#endif

/** @} */
