/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup PlanningDemo
 *	@{
 */

#ifndef _rng_H_
#define _rng_H_


// Uncomment the following line if you have the boost libraries
// (www.boost.org) and want high-quality random numbers:
// #define BOOST_RNG


#ifdef BOOST_RNG
#include <boost/random.hpp>
#endif

#include "matrix.h"


//! random number generator encapsulation
class Rng
{
public:
	Rng(unsigned int s = 0);
	~Rng();

	void seed(unsigned int s = 0);


	////////////////////////////////////////////////////////////
	// discrete distributions
	//

	// toss a fair coin
	inline bool coinToss()
	{ return (U() < 0.5); }

	// return true with probability p
	inline bool coinToss(double p)
	{ return (U() < p); }

	// uniform distribution on (0, 1, ..., n-1)
	inline unsigned int discrete(unsigned int n)
	{ return (unsigned int)floor(n * U()); }

	// uniform distribution on (a, a+1, ..., b-1)
	inline int discrete(int a, int b)
	{ return a + (int)floor((b - a) * U()); }


	////////////////////////////////////////////////////////////
	// continuous distributions
	//

	// uniform distribution on [0, 1)
	inline double uniform()
	{ return U(); }

	// uniform distribution on [a, b)
	inline double uniform(double a, double b)
	{ return a + (b - a) * U(); }

	// uniform distribution on the (N-1)-dimensional unit sphere in R^N
	inline Vector uniformSphere(unsigned int dim)
	{ Vector v = gauss(dim); return v / v.twonorm(); }

	// uniform distribution on the orthogonal group O_N(R)
	Matrix uniformOrthogonal(unsigned int dim);

	// standard normal distribution N(0, 1)
	double gauss();

	// normal distribution N(mu, sigma^2)
	inline double gauss(double mu, double sigma)
	{ return mu + sigma * gauss(); }

	// N-dimensional standard normal distribution
	Vector gauss(unsigned int dim);

protected:
	// basic generators
	double U();					// generate double in [0, 1)

	// state
#ifdef BOOST_RNG
	boost::mt19937 m_boostRng;
#endif
};


// global Rng instance, "the" global random number generator
extern Rng rng;


#endif
/** @} */

