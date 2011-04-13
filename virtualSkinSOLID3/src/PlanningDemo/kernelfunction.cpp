
#include "kernelfunction.h"


// width of the widest level of Gaussians
#define BASE_SIGMA 1.0


#include <math.h>


double kernel(double x, double y, double gamma)
{
	double diff = x - y;
	double m = -gamma * diff;
	return exp(m * diff);
}

double evalFunction(
		unsigned int numLevels,
		const double* coefficients,
		double x,
		bool zero)
{
	unsigned int l, i, ic = 2;
	unsigned int n = 0;
	double sigma = BASE_SIGMA;
	double gamma = 0.5 / (sigma * sigma);
	double ret = 0.0;
	for (l=0; l<numLevels; l++)
	{
		for (i=0; i<ic; i++)
		{
			double base = sigma * i;
			double k = kernel(x, base, gamma);
			if (zero) k -= kernel(0.0, base, gamma);
			ret += coefficients[n] * k;
			n++;
		}
		ic = 2 * ic - 1;
		sigma *= 0.5;
		gamma *= 4.0;
	}
	return ret;
}

void evalFunctions(
		unsigned int numFunctions,
		unsigned int numPositions,
		unsigned int numLevels,
		const double* coefficients,
		const double* x,
		double* f,
		bool zero)
{
	unsigned int i, j;
	unsigned int numCoeff = (1 << numLevels) - 1 + numLevels;
	for (j=0; j<numPositions; j++)
	{
		for (i=0; i<numFunctions; i++)
		{
			*f = evalFunction(numLevels, coefficients + numCoeff * i, x[j], zero);
			f++;
		}
	}
}

unsigned int n_ell[9] = {0, 2, 5, 10, 19, 36, 69, 134, 263};
double inner_product(unsigned int i, unsigned int j)
{
	unsigned int li, lj;
	for (li=1; n_ell[li]<=i; li++);
	for (lj=1; n_ell[lj]<=j; lj++);
	unsigned int ni = n_ell[li-1];
	unsigned int nj = n_ell[lj-1];
	double si = pow(2.0, 1.0 - li);
	double sj = pow(2.0, 1.0 - lj);
	double gi = 0.5 / (si*si);
	double gj = 0.5 / (sj*sj);
	double bi = si * (i - ni);
	double bj = sj * (j - nj);
	double ret = 0.0;
	unsigned int t;
	for (t=0; t<=100; t++)
	{
		double x = 0.01 * t;
		ret += kernel(x, bi, gi) * kernel(x, bj, gj);
	}
	return (ret / 101.0);
}
