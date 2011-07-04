/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef CYLINDER_H
#define CYLINDER_H

#include "primitiveobject.h"

namespace RobotModel { class Cylinder; }

/** \brief Represents a cylinder using SOLID and OpenGL
 *
 * This class creates a cylinder of variable radius and length. The local coordinate system is located at the centroid of the cylinder, and
 * it is oriented such that the y direction is the cylinder's axis of symmetry.
 */
class RobotModel::Cylinder : public PrimitiveObject
{
public:
    Cylinder( qreal radius, qreal height, int lod = 12);	//!< Creates the SOLID object
															/**< \param radius The radius of the cylinder
																 \param height The height of the cylinder
																 \param lod Level Of Detail - the number of line segments used to represent each circular edge of the cylinder */
    ~Cylinder();											//!< Destroys the SOLID object

	qreal r() { return radius; }
	qreal h() { return height; }
    virtual void makeDisplayList();							//!< Make an OpenGL display list for the cylinder using GL_TRIANGLE_FAN and a GL_TRIANGLE_STRIP

private:
    qreal	radius,									//!< Radius of the cylinder
			height;									//!< Height of the cylinder
    int		lod;									//!< Level Of Detail - the number of line segments used to represent each circular edge of the cylinder
};

#endif

/** @} */
