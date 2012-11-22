#include "matrix.h"
#include <string.h>
#include <algorithm>

unsigned int rngState[3] = {0, 0, 0};

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

Matrix::Matrix(std::vector<double> &other)
{
  resize(other.size(), 1);
	if (size() > 0) memcpy(&m_data[0], &other[0], size() * sizeof(double));
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
	ASSERT(rows && cols);
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
	printf("%d x %d matrix:\n", m_rows, m_cols);
	for (r=0; r<m_rows; r++)
	{
		for (c=0; c<m_cols; c++)
		{
			printf("  %10g\t", operator()(r, c));
		}
		printf("\n");
	}
}
