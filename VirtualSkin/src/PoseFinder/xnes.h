//  xNES algorithm, written by Tobias Glasmachers, 2011. 
//  This file is free software; you can redistribute it 
//  and/or modify it under the terms of the 
//  GNU General Public License as published by the
//  Free Software Foundation; either version 2, or 
//  (at your option) any later version.
//
//  This library is distributed in the hope that it 
//  will be useful, but WITHOUT ANY WARRANTY; 
//  without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//  See the GNU General Public License for more details.
//  http://www.gnu.org/licenses/gpl-2.0.html
//
//  You should have received a copy of the GNU General 
//  Public License along with this library; if not, 
//  write to the Free Software Foundation, Inc., 
//  675 Mass Ave, Cambridge, MA 02139, USA.
  
#ifndef _XNES_H_
#define _XNES_H_
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <cmath>
#include <cstring> // <-------------- ***** This is the library missing! *****
#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>
#include "function.h"
#include "matrix.h"

//using namespace std;


////////////////////////////////////////////////////////////
// vector and/or matrix class


////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
// xNES algorithm

class NES
{
	size_t d_dim;
public:
	NES(Function& f, bool importanceMixing = true, bool baseline = false)
	: d_dim(0), m_fitness(f)
	{
		rngSeed(time(0));
		m_importanceMixing = importanceMixing;
		m_baseline = baseline;

		alpha = 0.01;
	}

	~NES() { }


	void init(const Matrix& point, double stepsize, unsigned int _lambda = 0, double _lrSigma = 0.0, double _lrCovar = 0.0)
	{
		unsigned int i, ic = point.rows();
		ASSERT(point.cols() == 1);
		Matrix ssz(ic);
		for (i=0; i<ic; i++) ssz(i) = stepsize;
		init(point, ssz, _lambda, _lrSigma, _lrCovar);
	}

	// Calculates log2 of number.  
	double log2( double n ) {  // log(n)/log(2) is log2.  
		return std::log( n ) / std::log( 2. );  
	}


	void init(const Matrix& point, const Matrix& stepsize, unsigned int _lambda = 0, double _lrSigma = 0.0, double _lrCovar = 0.0)
	{
		unsigned int i, dim = point.rows();
		d_dim = dim;
		ASSERT(point.cols() == 1 && stepsize.rows() == dim && stepsize.cols() == 1);

		// batch size
		if (_lambda == 0)
		{
			//if (m_importanceMixing)
//			{
//				lambda = 10 * dim + 4;
//			}
//			else
//			{
				lambda = (unsigned int)(4.0 + floor(3.0 * log((double)dim)));
				if (lambda > dim) lambda = dim;
				if (lambda < 5) lambda = 5;
//			}
		}
		else lambda = _lambda;

		// utility function
		utility.resize(lambda);
		double sum = 0.0;
		for (i=0; i<lambda; i++)
		{
			// double u = ((double)lambda) - (2*i + 1);
			double u = log(lambda/2 + 1.0) - log(i + 1.0);
			if (u < 0.0) u = 0.0;
			utility[i] = u;
			sum += u;
		}
		for (i=0; i<lambda; i++) utility[i] /= sum;

		// baseline correction to the learning rate
		if (m_baseline)
		{
			for (i=0; i<lambda; i++) utility[i] -= 1.0 / (double)lambda;
		}

		// learing rates
		double nesLrSigma, nesLrCovar;
		if (m_importanceMixing)
		{
			nesLrSigma = nesLrCovar = std::min(1.0, (0.12 * lambda) / (log2((double)dim) * dim));
		} 
		else
		{
			nesLrSigma = std::min(1.0, 0.5 * (double)lambda / (double)dim);
			nesLrCovar = std::min(1.0, (double)lambda / (dim * (dim + 1.0) / 2.0));
		}

		lrCenter = 1.0;
		if (_lrSigma <= 0.0) lrSigma = nesLrSigma; else lrSigma = _lrSigma;
		if (_lrCovar <= 0.0) lrCovar = nesLrCovar; else lrCovar = _lrCovar;
// 		printf("[NES]  dim: %d  lambda: %d   lr: %g / %g\n", dim, lambda, lrSigma, lrCovar);

		// initial search distribution
		center = point;
		L.resize(dim, dim);
		C.resize(dim, dim);
		A.resize(dim, dim);
		for (i=0; i<dim; i++)
		{
			A(i, i) = stepsize(i);
			C(i, i) = stepsize(i) * stepsize(i);
			L(i, i) = 2.0 * log(stepsize(i));
		}
		oldDenom = 0.0;

		pop.resize(lambda);
		bestPt.resize(dim);
		bestFit = 1e300;
		evals = 0;


	}

	double variance() {
		return C.twonorm();
	}

	void iterate()
	{
		unsigned int i(0);
		unsigned int dim = center.rows();
		Matrix I = Matrix::eye(dim);
		double denom = pow(2.0 * M_PI, 0.5 * dim) * A.det();

		unsigned int newSamples = 0;

		if (evals == 0 || ! m_importanceMixing)
		{
			// sample lambda new individuals
			for (i=0; i<lambda; i++)
			{
				NesIndividual& ind = pop[i];
				ind.z = rngGauss(dim);
				ind.density = exp(-0.5 * ind.z.twonorm2()) / denom;
				ind.x = A * ind.z + center;
				ind.f = m_fitness.eval(ind.x);
				evals++;
				newSamples++;
			}
		}
		else
		{
			// sample using importance mixing
			for (i=0; i<lambda; i++)
			{
				NesIndividual& ind = pop[i];
				Matrix new_z = A.inv() * (ind.x - center);
				double new_density = exp(-0.5 * new_z.twonorm2()) / denom;
				double p = std::min(1.0, (1.0 - alpha) * new_density / ind.density);
				if (rngUniform() <= p)
				{
					// re-use the individual
					ind.z = new_z;
					ind.density = new_density;
				}
				else
				{
					// sample a new individual
					while (true)
					{
						ind.z = rngGauss(dim);
						ind.x = A * ind.z + center;
						ind.density = exp(-0.5 * ind.z.twonorm2()) / denom;
						Matrix old_z = oldAinv * (ind.x - oldCenter);
						double old_density = exp(-0.5 * old_z.twonorm2()) / oldDenom;
						double p = std::max(alpha, 1.0 - old_density / ind.density);
						if (rngUniform() <= p) break;
					}
					ind.f = m_fitness.eval(ind.x);
					evals++;
					newSamples++;
				}
			}
		}

		// sort by fitness
		std::sort(pop.begin(), pop.end());
		if (pop[0].f < bestFit)
		{
			bestPt = pop[0].x;
			bestFit = pop[0].f;
		}

		// compute (natural) gradients w.r.t. z and L
		Matrix gz(dim);
		Matrix gL(dim, dim);
		for (i=0; i<lambda; i++)
		{
			double u = utility[i];
			if (u == 0.0) break;
			const Matrix& z = pop[i].z;
			gz += u * z;
			gL += u * (z * z.T() - I);
		}

		// remember old distribution for importance mixing
		oldCenter = center;
		oldAinv = A.inv();
		oldDenom = denom;

		center += A * (lrCenter * gz);
		Matrix dL = lrCovar * gL;
		dL += ((lrSigma - lrCovar) * (gL.tr() / (double)dim)) * I;
		C = A * dL.exp() * A;
		A = C.sqrt();
	}

	inline unsigned int evaluations() const
	{ return evals; }

	inline const Matrix& bestPoint() const
	{ return bestPt; }

	inline double bestFitness() const
	{ return bestFit; }

	inline double conditioning() const
	{
		Matrix U, D;
		A.eig(U, D);
		return (D.max() / D.min());
	}

	void set_variance(Matrix variance) {
		for (size_t  i=0; i < d_dim; i++)
		{
			A(i, i) = variance(i);
			C(i, i) = variance(i) * variance(i);
			L(i, i) = 2.0 * log(variance(i));
		}
	}

	size_t dim() {return d_dim;}

protected:
	class NesIndividual
	{
	public:
		NesIndividual()
		{
			f = 1e300;
			density = 0.0;
		}

		NesIndividual(const NesIndividual& other)
		: z(other.z), x(other.x)
		{
			f = other.f;
			density = other.density;
		}

		~NesIndividual() { }

		void operator = (const NesIndividual& other)
		{
			z = other.z;
			x = other.x;
			f = other.f;
			density = other.density;
		}

		bool operator < (const NesIndividual& other) const
		{ return (f < other.f); }

		Matrix z;					// standard normally distributed Gaussian
		Matrix x;					// back-transformed Gaussian
		double f;					// fitness
		double density;				// density in the backtransformed distribution
	};

	Function &m_fitness;

	///////////////////////////////////////////////////////
	// algorithm hyper-parameters
	//

	// offspring population size
	unsigned int lambda;
	std::vector<NesIndividual> pop;

	// learning rates
	double lrCenter;
	double lrSigma;
	double lrCovar;

	// utility function
	std::vector<double> utility;

	///////////////////////////////////////////////////////
	// other stuff
	//

	// algorithm state
	Matrix center;
	Matrix L;
	Matrix C;
	Matrix A;				// derived state variable

	// importance mixing
	double alpha;			// minimal refresh rate
	Matrix oldCenter;
	Matrix oldAinv;
	double oldDenom;

	// important statistics
	unsigned int evals;

	// currently best solution
	Matrix bestPt;
	double bestFit;

	// mode of operation
	bool m_importanceMixing;
	bool m_baseline;
};

////////////////////////////////////////////////////////////
// good old sphere objective function


//class Sphere : public Function
//{
//public:
//	Sphere()
//	{
//	}
//
//	double eval(const Matrix& point) const
//	{
//		return point.twonorm2();
//	}
//};


////////////////////////////////////////////////////////////
// after more than 1000 lines we are finally ready for main!


//int main(int argc, char** argv)
//{
//	try
//	{
//		// Experiment setup:
//		// (*) dimension of sphere function
//		// (*) number of function evaluations
//		// (*) number of trials
//		unsigned int dim = 10;
//		unsigned int maxevals = 5000;
//		unsigned int trials = 10;
//
//		bool useImportanceMixing = true;
//		bool useBaseline = false;
//
//		unsigned int m;
//		double mean = 0.0;
//		for (m=0; m<trials; m++)
//		{
//			rngSeed(123 * m + 45);
//			Sphere f;
//			Matrix pt = rngGauss(dim);
//			Matrix sigma = Matrix::ones(dim);
//			sigma(0) = 1e3;
//			NES nes(f, useImportanceMixing, useBaseline);
//			nes.init(pt, sigma);
//			do
//			{
//				nes.iterate();
//			}
//			while (nes.evaluations() < maxevals);
//			mean -= log10(nes.bestFitness());
//			printf("   trial: %d/%d     fitness: %g\n", m+1, trials, nes.bestFitness());
//		}
//		mean /= (double)trials;
//		printf("xNES mean success (-log10(fitness): %g\n", mean);
//
//	}
//	catch (const char* exception)
//	{
//		printf("\n\nEXCEPTION: %s\n\n", exception);
//	}
//}


#endif
