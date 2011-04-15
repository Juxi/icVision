/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup PlanningDemo
 *	@{
 */

#ifndef ELITISTXNES_H
#define ELITISTXNES_H

#include "rng.h"
#include "vector.h"
#include "matrix.h"
#include "fitnessfunction.h"

//!
//! (1+1)-xNES algorithm, see
//! T. Glasmachers, T. Schaul, and J. Schmidhuber<br>
//! A Natural Evolution Strategy for Multi-Objective Optimization.<br>
//! Parallel Problem Solving from Nature (PPSN) XI, 2010.<br>
//!
class ElitistxNES
{
public:
	ElitistxNES(FitnessFunction& f, Rng* r = &rng);
	~ElitistxNES();

	void init(Vector const& point, double stepsize);
	void init(Vector const& point, Vector const& stepsize);
	void init(Vector const& point, Matrix const& A);

	// The determinant of the matrix B is preserved by the algorithm.
	void init(Vector const& point, double sigma, Matrix const& B);
	void iterate();

	unsigned int evaluations() const;
	Vector const& bestPoint() const;
	double bestFitness() const;

	inline Vector mean() const
	{ return x; }
	inline double standardDeviation() const
	{ return s; }
	inline Matrix normalizedCovarianceFactor() const
	{ return B; }

protected:
	// reference to the fitness function
	FitnessFunction& fitness;

	// algorithm state
	Vector x;
	double s;
	Matrix B;
	double f;

	// fixed stuff: dimension and learning rates
	unsigned int dim;
	double lrB;
	double lrS;

	// number of fitness evaluations
	unsigned int evals;

	// random number generator
	Rng* m_rng;
};


#endif
/** @} */

