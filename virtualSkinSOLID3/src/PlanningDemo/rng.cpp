
#include "rng.h"


// global random number generator object for common use
Rng rng;


////////////////////////////////////////////////////////////


Rng::Rng(unsigned int s)
{
	seed(s);
}

Rng::~Rng()
{
}


void Rng::seed(unsigned int s)
{
#ifdef BOOST_RNG
	m_boostRng.seed(s);
#else
	srand(s);
#endif
}

Matrix Rng::uniformOrthogonal(unsigned int dim)
{
	Matrix G(dim, dim);
	unsigned int i, j;
	for (i=0; i<dim; i++)
		for (j=0; j<dim; j++)
			G(i, j) = gauss();

	Matrix U;
	Vector lambda;
	G.eig(U, lambda);
	return U;
}

double Rng::gauss()
{
	double v, w, rsq;
	do
	{
		v = 2.0 * U() - 1.0;
		w = 2.0 * U() - 1.0;
		rsq = v * v + w * w;
	}
	while (rsq >= 1.0 || rsq == 0.0);
	return w * sqrt(-2.0 * log(rsq) / rsq);
}

Vector Rng::gauss(unsigned int dim)
{
	Vector ret(dim);
	unsigned int i;
	for (i=0; i<dim; i++) ret(i) = gauss();
	return ret;
}

double Rng::U()
{
#ifdef BOOST_RNG
	boost::variate_generator<boost::mt19937*, boost::uniform_real<> > r(&m_boostRng, boost::uniform_real<>(0.0, 1.0));
	return r();
#else
	return ((double)::rand() / (double)RAND_MAX);
#endif
}
