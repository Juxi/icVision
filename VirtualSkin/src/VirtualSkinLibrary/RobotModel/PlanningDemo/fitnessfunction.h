/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup PlanningDemo
 *	@{
 */

#ifndef FITNESSFUNCTION_H
#define FITNESSFUNCTION_H


#include <math.h>
#include "vector.h"


//! generic fitness function interface
class FitnessFunction
{
public:
	FitnessFunction()
	{ }
	virtual ~FitnessFunction()
	{ }

	virtual double eval(Vector const& point) const = 0;
	inline double operator () (Vector const& point) const
	{ return eval(point); }
};


#endif
/** @} */
