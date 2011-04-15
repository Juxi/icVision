/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#include "elitistxnes.h"
#include <math.h>


ElitistxNES::ElitistxNES(FitnessFunction& f, Rng* r)
: fitness(f)
{
	m_rng = r;
}

ElitistxNES::~ElitistxNES()
{
}


void ElitistxNES::init(Vector const& point, double stepsize)
{
	Matrix B = Matrix::identity(point.dim());
	init(point, stepsize, B);
}

void ElitistxNES::init(Vector const& point, Vector const& stepsize)
{
	Matrix A = Matrix::diag(stepsize);
	init(point, A);
}

void ElitistxNES::init(Vector const& point, Matrix const& A)
{
	double sigma = pow(A.det(), 1.0 / point.size());
	init(point, sigma, A / sigma);
}

void ElitistxNES::init(Vector const& point, double sigma, Matrix const& B)
{
	this->dim = point.size();

	this->lrB = 0.25 / (dim * sqrt((double)dim));
	this->lrS = 0.2 / (double)dim;

	this->x = point;
	this->s = sigma;
	this->B = B;

	this->f = fitness(x);
	this->evals = 1;
}

void ElitistxNES::iterate()
{
	Vector z = m_rng->gauss(dim);
	Vector ox = x + (B * z) * s;
	double of = fitness(ox);
	evals++;

	if (of <= f)
	{
		x = ox;
		s *= exp(5.0 * lrS);
		if (of < f)
		{
			f = of;
			double n2 = z.twonorm2();
			Matrix S = Matrix::outerProduct(z / sqrt(n2));
			Matrix R = Matrix::identity(dim) - S;
			double trLog = n2 / dim - 1.0;
			double mR = exp(lrB * (-1.0 - trLog));
			double mS = exp(lrB * ((n2 - 1.0) - trLog));
			Matrix E = S * mS + R * mR;
			B = B * E;
		}
	}
	else
	{
		s /= exp(lrS);
	}
}

unsigned int ElitistxNES::evaluations() const
{
	return evals;
}

Vector const& ElitistxNES::bestPoint() const
{
	return x;
}

double ElitistxNES::bestFitness() const
{
	return f;
}
