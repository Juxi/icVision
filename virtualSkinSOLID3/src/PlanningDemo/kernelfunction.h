
#ifndef KERNELFUNCTION_H
#define KERNELFUNCTION_H


////////////////////////////////////////////////////////////
// Evaluate the Gaussian kernel function (k)
//    exp(-gamma * (x-y)^2)
// and its first (v) and second (a) derivative
// w.r.t. the varibale x.
//
// parameters:
//    x           first argument of the kernel function
//    y           second argument of the kernel function - usually the basis point
//    gamma       bandwidth parameter = 1 / (2*sigma^2)
// return: kernel value
double kernel(double x, double y, double gamma);


////////////////////////////////////////////////////////////
// Evaluate the kernel expansion with numLevels different
// levels of granularity. On each level, there are 2^{n-1}+1
// basis points on an equi-distant grid in the interval [0, 1],
// and a kernel basis function with width 2^{-n}. This makes a
// total of 2^numLevels - 1 + numLevels coefficients, which are
// expected in the coefficients array.
//
// parameters:
//    numLevels        number of "levels" of granularity of the
//                     implicit function representation; there
//                     are 2 + 3 + 5 + 9 + ... + 2^{n-1}+1 = 2^n+n-1
//                     coefficients or basis functions
//    coefficients     one coefficient per basis function
//    x                value in [0, 1] at which the function should
//                     be evaluated
//    zero             subtract the function value at position zero
//    return: function value
//
double evalFunction(
		unsigned int numLevels,
		const double* coefficients,
		double x,
		bool zero = true);


////////////////////////////////////////////////////////////
// Evaluate a number of kernel expansions
// at a number of positions at the same time.
//
// parameters:
//    numFunctions     number of functions to evaluate
//    numPositions     number of positions at which to evaluate
//    numLevels        number of "levels" of granularity of the
//                     implicit function representation; there
//                     are 2 + 3 + 5 + 9 + ... + 2^{n-1}+1 = 2^n+n-1
//                     coefficients or basis functions
//    coefficients     one coefficient per basis function and function
//    x                positions at which to evaluate the functions
//    f                [out] function values
//
void evalFunctions(
		unsigned int numFunctions,
		unsigned int numPositions,
		unsigned int numLevels,
		const double* coefficients,
		const double* x,
		double* f,
		bool zero = true);


double inner_product(unsigned int i, unsigned int j);


#endif
