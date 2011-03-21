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

class RobotModel::Cylinder : public PrimitiveObject
{
public:
    Cylinder( qreal radius, qreal height, int lod = 12);
    ~Cylinder();

    virtual void makeDisplayList();

private:
    qreal radius, height;
    int lod;
};

#endif

/** @} */
