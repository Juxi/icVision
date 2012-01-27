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
  

#include <stdio.h>
#include <math.h>
#include <cstring> // <-------------- ***** This is the library missing! *****
#include <vector>
#include <algorithm>

using namespace std;

#ifdef DEBUG
#define ASSERT(b) { if (! (b)) { char msg[256]; sprintf(msg, "ASSERT failed in file %s, line %d", __FILE__, __LINE__); throw msg; } }
// #define ASSERT(b) { if (! (b)) {*((int*)(NULL)) = 0; } }		// trap the debugger
#else
#define ASSERT(b) { }
#endif


////////////////////////////////////////////////////////////
// vector and/or matrix class


class Matrix
{
public:
	Matrix();
	explicit Matrix(unsigned int rows, unsigned int cols = 1);
	Matrix(const Matrix& other);
	Matrix(double* data, unsigned int rows, unsigned int cols = 1);
	virtual ~Matrix();

	inline bool isValid() const
	{ return (m_data.size() > 0); }
	inline bool isVector() const
	{ return (isColumnVector() || isRowVector()); }
	inline bool isColumnVector() const
	{ return (m_rows == 1); }
	inline bool isRowVector() const
	{ return (m_cols == 1); }
	inline bool isSquare() const
	{ return (m_rows == m_cols); }

	inline unsigned int rows() const
	{ return m_rows; }
	inline unsigned int cols() const
	{ return m_cols; }

	inline unsigned int size() const
	{ return m_data.size(); }

	// type cast for 1x1 matrices
	operator double() const;

	inline double& operator [] (unsigned int index)
	{
		ASSERT(index < m_data.size());
		return m_data[index];
	}
	inline const double& operator [] (unsigned int index) const
	{
		ASSERT(index < m_data.size());
		return m_data[index];
	}

	inline double& operator () (unsigned int index)
	{ return operator [] (index); }
	inline const double& operator () (unsigned int index) const
	{ return operator [] (index); }

	inline double& operator () (unsigned int y, unsigned int x)
	{ return operator [] (y * m_cols + x); }
	inline const double& operator () (unsigned int y, unsigned int x) const
	{ return operator [] (y * m_cols + x); }

	Matrix row(unsigned int r) const;
	Matrix col(unsigned int c) const;

	void resize(unsigned int rows, unsigned int cols = 1);

	const Matrix& operator = (const Matrix& other);

	bool operator == (const Matrix& other);
	bool operator != (const Matrix& other);

	Matrix operator + (const Matrix& other) const;
	Matrix operator - (const Matrix& other) const;
	Matrix operator * (const Matrix& other) const;
	void operator += (const Matrix& other);
	void operator -= (const Matrix& other);
	void operator *= (double scalar);
	void operator /= (double scalar);
	friend Matrix operator * (double scalar, const Matrix& mat);
	Matrix operator / (double scalar) const;

	static Matrix zeros(unsigned int rows, unsigned int cols = 1);
	static Matrix ones(unsigned int rows, unsigned int cols = 1);
	static Matrix eye(unsigned int n);
	static Matrix fromEig(const Matrix& U, const Matrix& lambda);

	void eig(Matrix& U, Matrix& lambda, unsigned int iter = 200, bool ignoreError = true) const;
	Matrix T() const;
	Matrix exp() const;
	Matrix log() const;
	Matrix pow(double e) const;
	inline Matrix power(double e) const
	{ return pow(e); }
	inline Matrix sqrt() const
	{ return pow(0.5); }
	inline Matrix inv() const
	{ return pow(-1.0); }
	double det() const;
	double logdet() const;
	double tr() const;

	double min() const;
	double max() const;

	double norm(double p = 2.0) const;
	double onenorm() const;
	double twonorm() const;
	double twonorm2() const;
	double maxnorm() const;

	Matrix diag() const;

	void print() const;

protected:
	// thread-safe comparison predicate for indices
	class CmpIndex
	{
	public:
		CmpIndex(const Matrix& lambda, std::vector<unsigned int>& index);

		bool operator ()(unsigned int i1, unsigned int i2) const;

	protected:
		const Matrix& m_lambda;
		std::vector<unsigned int>& m_index;
	};

	unsigned int m_rows;
	unsigned int m_cols;
	std::vector<double> m_data;
};


////////////////////////////////////////////////////////////


Matrix::CmpIndex::CmpIndex(const Matrix& lambda, std::vector<unsigned int>& index)
: m_lambda(lambda)
, m_index(index)
{
}


bool Matrix::CmpIndex::operator ()(unsigned int i1, unsigned int i2) const
{
	return (m_lambda(i1) > m_lambda(i2));
}


////////////////////////////////////////////////////////////


Matrix::Matrix()
{
	resize(0, 0);
}

Matrix::Matrix(unsigned int rows, unsigned int cols)
{
	resize(rows, cols);
}

Matrix::Matrix(const Matrix& other)
{
	operator = (other);
}

Matrix::Matrix(double* data, unsigned int rows, unsigned int cols)
{
	resize(rows, cols);
	if (size() > 0) memcpy(&m_data[0], data, size() * sizeof(double));
}

Matrix::~Matrix()
{
}


Matrix::operator double() const
{
	ASSERT(m_rows == 1 && m_cols == 1);
	return m_data[0];
}

Matrix Matrix::row(unsigned int r) const
{
	ASSERT(r < m_rows);
	Matrix ret(1, m_cols);
	unsigned int i;
	for (i=0; i<m_cols; i++) ret(0, i) = operator () (r, i);
	return ret;
}

Matrix Matrix::col(unsigned int c) const
{
	ASSERT(c < m_cols);
	Matrix ret(m_rows, 1);
	unsigned int i;
	for (i=0; i<m_rows; i++) ret(i, 0) = operator () (i, c);
	return ret;
}

void Matrix::resize(unsigned int rows, unsigned int cols)
{
	m_rows = rows;
	m_cols = cols;
	unsigned int sz = rows * cols;
	m_data.resize(sz);
	if (sz > 0) memset(&m_data[0], 0, sz * sizeof(double));
}

const Matrix& Matrix::operator = (const Matrix& other)
{
	resize(other.rows(), other.cols());
	if (size() > 0) memcpy(&m_data[0], &other[0], size() * sizeof(double));
	return other;
}

bool Matrix::operator == (const Matrix& other)
{
	return (m_rows == other.rows() && m_cols == other.cols() && memcmp(&m_data[0], &other[0], size() * sizeof(double)) == 0);
}

bool Matrix::operator != (const Matrix& other)
{
	return (m_rows != other.rows() || m_cols != other.cols() || memcmp(&m_data[0], &other[0], size() * sizeof(double)) != 0);
}

Matrix Matrix::operator + (const Matrix& other) const
{
	Matrix ret(*this);
	ret += other;
	return ret;
}

Matrix Matrix::operator - (const Matrix& other) const
{
	Matrix ret(*this);
	ret -= other;
	return ret;
}

Matrix Matrix::operator * (const Matrix& other) const
{
	unsigned int x, xc = other.m_cols;
	unsigned int y, yc = m_rows;
	unsigned int k, kc = m_cols;
	ASSERT(other.m_rows == kc);
	Matrix ret(yc, xc);
	for (y=0; y<yc; y++)
	{
		for (x=0; x<xc; x++)
		{
			double v = 0.0;
			for (k=0; k<kc; k++) v += operator () (y, k) * other(k, x);
			ret(y, x) = v;
		}
	}
	return ret;
}

void Matrix::operator += (const Matrix& other)
{
	ASSERT(m_rows == other.rows());
	ASSERT(m_cols == other.cols());
	unsigned int i, ic = size();
	for (i=0; i<ic; i++) m_data[i] += other[i];
}

void Matrix::operator -= (const Matrix& other)
{
	ASSERT(m_rows == other.rows());
	ASSERT(m_cols == other.cols());
	unsigned int i, ic = size();
	for (i=0; i<ic; i++) m_data[i] -= other[i];
}

void Matrix::operator *= (double scalar)
{
	unsigned int i, ic = size();
	for (i=0; i<ic; i++) m_data[i] *= scalar;
}

void Matrix::operator /= (double scalar)
{
	unsigned int i, ic = size();
	for (i=0; i<ic; i++) m_data[i] /= scalar;
}

// friend
Matrix operator * (double scalar, const Matrix& mat)
{
	Matrix ret(mat.rows(), mat.cols());
	unsigned int i, ic = mat.size();
	for (i=0; i<ic; i++) ret[i] = scalar * mat[i];
	return ret;
}

Matrix Matrix::operator / (double scalar) const
{
	Matrix ret(*this);
	ret /= scalar;
	return ret;
}

// static
Matrix Matrix::zeros(unsigned int rows, unsigned int cols)
{
	return Matrix(rows, cols);
}

// static
Matrix Matrix::ones(unsigned int rows, unsigned int cols)
{
	Matrix m(rows, cols);
	unsigned int i, ic = rows * cols;
	for (i=0; i<ic; i++) m[i] = 1.0;
	return m;
}

// static
Matrix Matrix::eye(unsigned int n)
{
	Matrix m(n, n);
	unsigned int i;
	for (i=0; i<n; i++) m(i, i) = 1.0;
	return m;
}

// static
Matrix Matrix::fromEig(const Matrix& U, const Matrix& lambda)
{
	return U * lambda.diag() * U.T();
}

// eigen-decomposition of a symmetric matrix
void Matrix::eig(Matrix& U, Matrix& lambda, unsigned int iter, bool ignoreError) const
{
	ASSERT(isValid() && isSquare());
	Matrix basic = *this;
	Matrix eigenval(m_rows);

	// 1-dim case
	if (m_rows == 1) { basic(0, 0) = 1.0; eigenval(0) = m_data[0]; return; }

	std::vector<double> oD(m_rows);
	unsigned int i, j, k, l, m;
	double b, c, f, g, h, hh, p, r, s, scale;

	// reduction to tridiagonal form
	for (i=m_rows; i-- > 1;)
	{
		h = 0.0;
		scale = 0.0;
		if (i > 1) for (k = 0; k < i; k++) scale += fabs(basic(i, k));

		if (scale == 0.0) oD[i] = basic(i, i-1);
		else
		{
			for (k = 0; k < i; k++)
			{
				basic(i, k) /= scale;
				h += basic(i, k) * basic(i, k);
			}

			f = basic(i, i-1);
			g = (f > 0.0) ? -::sqrt(h) : ::sqrt(h);
			oD[i] = scale * g;
			h -= f * g;
			basic(i, i-1) = f - g;
			f = 0.0;

			for (j = 0; j < i; j++)
			{
				basic(j, i) = basic(i, j) / (scale * h);
				g = 0.0;
				for (k=0; k <= j; k++) g += basic(j, k) * basic(i, k);
				for (k=j+1; k < i; k++) g += basic(k, j) * basic(i, k);
				f += (oD[j] = g / h) * basic(i, j);
			}
			hh = f / (2.0 * h);

			for (j=0; j < i; j++)
			{
				f = basic(i, j);
				g = oD[j] - hh * f;
				oD[j] = g;
				for (k=0; k <= j; k++) basic(j, k) -= f * oD[k] + g * basic(i, k);
			}

			for (k=i; k--;) basic(i, k) *= scale;
		}
		eigenval(i) = h;
	}
	eigenval(0) = oD[0] = 0.0;

	// accumulation of transformation matrices
	for (i=0; i < m_rows; i++)
	{
		if (eigenval(i) != 0.0)
		{
			for (j=0; j < i; j++)
			{
				g = 0.0;
				for (k = 0; k < i; k++) g += basic(i, k) * basic(k, j);
				for (k = 0; k < i; k++) basic(k, j) -= g * basic(k, i);
			}
		}
		eigenval(i) = basic(i, i);
		basic(i, i) = 1.0;
		for (j=0; j < i; j++) basic(i, j) = basic(j, i) = 0.0;
	}

	// eigenvalues from tridiagonal form
	for (i=1; i < m_rows; i++) oD[i-1] = oD[i];
	oD[m_rows - 1] = 0.0;

	for (l=0; l < m_rows; l++)
	{
		j = 0;
		do
		{
			// look for small sub-diagonal element
			for (m=l; m < m_rows - 1; m++)
			{
				s = fabs(eigenval(m)) + fabs(eigenval(m + 1));
				if (fabs(oD[m]) + s == s) break;
			}
			p = eigenval(l);

			if (m != l)
			{
				if (j++ == iter)
				{
					// Too many iterations --> numerical instability!
					if (ignoreError) break;
					else throw("[Matrix::eig] numerical problems");
				}

				// form shift
				g = (eigenval(l+1) - p) / (2.0 * oD[l]);
				r = ::sqrt(g * g + 1.0);
				g = eigenval(m) - p + oD[l] / (g + ((g > 0.0) ? fabs(r) : -fabs(r)));
				s = 1.0;
				c = 1.0;
				p = 0.0;

				for (i=m; i-- > l;)
				{
					f = s * oD[i];
					b = c * oD[i];
					if (fabs(f) >= fabs(g))
					{
						c = g / f;
						r = ::sqrt(c * c + 1.0);
						oD[i+1] = f * r;
						s = 1.0 / r;
						c *= s;
					}
					else
					{
						s = f / g;
						r = ::sqrt(s * s + 1.0);
						oD[i+1] = g * r;
						c = 1.0 / r;
						s *= c;
					}

					g = eigenval(i+1) - p;
					r = (eigenval(i) - g) * s + 2.0 * c * b;
					p = s * r;
					eigenval(i+1) = g + p;
					g = c * r - b;

					for (k=0; k < m_rows; k++)
					{
						f = basic(k, i+1);
						basic(k, i+1) = s * basic(k, i) + c * f;
						basic(k, i) = c * basic(k, i) - s * f;
					}
				}

				eigenval(l) -= p;
				oD[l] = g;
				oD[m] = 0.0;
			}
		}
		while (m != l);
	}

	// normalize eigenvectors
	for (j=m_rows; j--;)
	{
		s = 0.0;
		for (i=m_rows; i--;) s += basic(i, j) * basic(i, j);
		s = ::sqrt(s);
		for (i=m_rows; i--;) basic(i, j) /= s;
	}

	// sort by eigenvalues
	std::vector<unsigned int> index(m_rows);
	for (i=0; i<m_rows; i++) index[i] = i;
	CmpIndex cmpidx(eigenval, index);
	std::sort(index.begin(), index.end(), cmpidx);
	U.resize(m_rows, m_rows);
	lambda.resize(m_rows);
	for (i=0; i<m_rows; i++)
	{
		j = index[i];
		lambda(i) = eigenval(j);
		for (k=0; k<m_rows; k++) U(k, i) = basic(k, j);
	}
}

Matrix Matrix::T() const
{
	Matrix ret(m_cols, m_rows);
	unsigned int r, c;
	for (c=0; c<m_cols; c++)
	{
		for (r=0; r<m_rows; r++)
		{
			ret(c, r) = (*this)(r, c);
		}
	}
	return ret;
}

Matrix Matrix::exp() const
{
	ASSERT(isSquare());
	Matrix U;
	Matrix lambda;
	eig(U, lambda);
	unsigned int i;
	for (i=0; i<m_rows; i++) lambda(i) = ::exp(lambda(i));
	Matrix ret = fromEig(U, lambda);
	return ret;
}

Matrix Matrix::log() const
{
	ASSERT(isSquare());
	Matrix U;
	Matrix lambda;
	eig(U, lambda);
	unsigned int i;
	for (i=0; i<m_rows; i++) lambda(i) = ::log(lambda(i));
	return fromEig(U, lambda);
}

Matrix Matrix::pow(double e) const
{
	ASSERT(isSquare());
	Matrix U;
	Matrix lambda;
	eig(U, lambda);
	unsigned int i;
	for (i=0; i<m_rows; i++) lambda(i) = ::pow(lambda(i), e);
	return fromEig(U, lambda);
}

double Matrix::det() const
{
	Matrix U;
	Matrix lambda;
	eig(U, lambda);
	double ret = 1.0;
	unsigned int i, ic = lambda.size();
	for (i=0; i<ic; i++) ret *= lambda(i);
	return ret;
}

double Matrix::logdet() const
{
	Matrix U;
	Matrix lambda;
	eig(U, lambda);
	double ret = 0.0;
	unsigned int i, ic = lambda.size();
	for (i=0; i<ic; i++) ret += ::log(lambda(i));
	return ret;
}

double Matrix::tr() const
{
	ASSERT(isSquare());
	double ret = 0.0;
	unsigned int i;
	for (i=0; i<m_rows; i++) ret += operator() (i, i);
	return ret;
}

double Matrix::min() const
{
	double ret = m_data[0];
	unsigned int i, ic = size();
	for (i=1; i<ic; i++) if (m_data[i] < ret) ret = m_data[i];
	return ret;
}

double Matrix::max() const
{
	double ret = m_data[0];
	unsigned int i, ic = size();
	for (i=1; i<ic; i++) if (m_data[i] > ret) ret = m_data[i];
	return ret;
}

double Matrix::norm(double p) const
{
	unsigned int i, ic = size();
	double sum = 0.0;
	for (i=0; i<ic; i++) sum += ::pow(fabs(m_data[i]), p);
	return ::pow(sum, 1.0 / p);
}

double Matrix::onenorm() const
{
	unsigned int i, ic = size();
	double sum = 0.0;
	for (i=0; i<ic; i++) sum += fabs(m_data[i]);
	return sum;
}

double Matrix::twonorm() const
{
	unsigned int i, ic = size();
	double sum = 0.0;
	for (i=0; i<ic; i++)
	{
		double v = m_data[i];
		sum += v * v;
	}
	return ::sqrt(sum);
}

double Matrix::twonorm2() const
{
	unsigned int i, ic = size();
	double sum = 0.0;
	for (i=0; i<ic; i++)
	{
		double v = m_data[i];
		sum += v * v;
	}
	return sum;
}

double Matrix::maxnorm() const
{
	unsigned int i, ic = size();
	double m = fabs(m_data[0]);
	for (i=1; i<ic; i++)
	{
		double v = fabs(m_data[i]);
		if (v > m) m = v;
	}
	return m;
}

Matrix Matrix::diag() const
{
	Matrix ret;
	unsigned int i;
	if (isSquare())
	{
		ret.resize(m_rows);
		for (i=0; i<m_rows; i++) ret(i) = operator() (i, i);
	}
	else if (m_cols == 1)
	{
		ret.resize(m_rows, m_rows);
		for (i=0; i<m_rows; i++) ret(i, i) = operator() (i);
	}
	else ASSERT(false);

	return ret;
}

void Matrix::print() const
{
	unsigned int r, c;
	printf("%d x %d matrix at object address %p and memory address %p:\n", m_rows, m_cols, this, (size() > 0) ? &m_data[0] : NULL);
	for (r=0; r<m_rows; r++)
	{
		for (c=0; c<m_cols; c++)
		{
			printf("  %10g\t", operator()(r, c));
		}
		printf("\n");
	}
}


////////////////////////////////////////////////////////////
// encapsulation of an objective function


class Function
{
public:
	Function() { }
	virtual ~Function() { }

	virtual double eval(const Matrix& point) const = 0;
};


////////////////////////////////////////////////////////////
// random number generator


unsigned int rngState[3] = {0, 0, 0};

void rngSeed(unsigned int s)
{
	rngState[0] = (s & 255) + 1;
	rngState[1] = ((s >> 8) & 255) + 10000;
	rngState[2] = ((s >> 16) & 255) + 3000;
}

double rngUniform()
{
	double ret = rngState[0] / 1000.0 + rngState[1] / 1234.0 + rngState[2] / 1357.0;

	rngState[0] = (unsigned int)(rngState[0] * 171UL % 30269UL);
	rngState[1] = (unsigned int)(rngState[1] * 172UL % 30307UL);
	rngState[2] = (unsigned int)(rngState[2] * 170UL % 30323UL);

	return (ret - ((unsigned int)ret));
}

double rngGauss()
{
	double v, w, rsq;
	do
	{
		v = 2.0 * rngUniform() - 1.0;
		w = 2.0 * rngUniform() - 1.0;
		rsq = v * v + w * w;
	}
	while (rsq >= 1.0 || rsq == 0.0);
	return w * sqrt(-2.0 * log(rsq) / rsq);
}

Matrix rngGauss(unsigned int dim)
{
	unsigned int i;
	Matrix ret(dim, 1);
	for (i=0; i<dim; i++) ret(i) = rngGauss();
	return ret;
}


////////////////////////////////////////////////////////////
// xNES algorithm

class NES
{
public:
	NES(const Function& f, bool importanceMixing = true, bool baseline = false)
	: m_fitness(f)
	{
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

	void init(const Matrix& point, const Matrix& stepsize, unsigned int _lambda = 0, double _lrSigma = 0.0, double _lrCovar = 0.0)
	{
		unsigned int i, dim = point.rows();
		ASSERT(point.cols() == 1 && stepsize.rows() == dim && stepsize.cols() == 1);

		// batch size
		if (_lambda == 0)
		{
			if (m_importanceMixing)
			{
				lambda = 10 * dim + 4;
			}
			else
			{
				lambda = (unsigned int)(4.0 + floor(3.0 * log((double)dim)));
				if (lambda > dim) lambda = dim;
				if (lambda < 5) lambda = 5;
			}
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

	void iterate()
	{
		unsigned int i;
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

	const Function& m_fitness;

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
