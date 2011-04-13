
#ifndef MATRIX_H
#define MATRIX_H


#include <stdio.h>
#include "vector.h"

////////////////////////////////////////////////////////////
// Real matrix, supporting basic linear algebra operations
// as well as eigen-decomposition, singular value decomposition,
// and operations building thereon, such as matrix exponential.
//
// NOTE: Eigen-decomposinion, matix square root and exponential
//       assume a SYMMETRIC square matrix. For non-symmetric
//       matrices the result is undefined!
//
class Matrix
{
private:
	class CmpIndex
	{
	public:
		CmpIndex(Vector const& lambda, std::vector<unsigned int>& index)
		: m_lambda(lambda)
		, m_index(index)
		{ }

		inline bool operator ()(unsigned int i1, unsigned int i2) const
		{ return (m_lambda[i1] > m_lambda[i2]); }

	protected:
		Vector const& m_lambda;
		std::vector<unsigned int>& m_index;
	};

public:
	Matrix()
	: m_rows(0)
	, m_cols(0)
	, m_data(0)
	{ }

	Matrix(unsigned int rows, unsigned int cols)
	: m_rows(rows)
	, m_cols(cols)
	, m_data(rows * cols)
	{
		unsigned int i, ic = m_data.size();
		for (i=0; i<ic; i++) m_data[i] = 0.0;
	}

	void operator = (Matrix const& other)
	{
		m_rows = other.m_rows;
		m_cols = other.m_cols;
		m_data = other.m_data;
	}

	inline unsigned int rows() const
	{ return m_rows; }
	inline unsigned int cols() const
	{ return m_cols; }
	inline double& operator () (unsigned int y, unsigned int x)
	{
		assert(y < m_rows && x < m_cols);
		return m_data[x + m_cols * y];
	}
	inline double const& operator () (unsigned int y, unsigned int x) const
	{
		assert(y < m_rows && x < m_cols);
		return m_data[x + m_cols * y];
	}

	void operator += (Matrix const& other)
	{
		assert(m_rows == other.rows() && m_cols == other.cols());
		unsigned int i, ic = m_rows * m_cols;
		for (i=0; i<ic; i++) m_data[i] += other.m_data[i];
	}
	void operator -= (Matrix const& other)
	{
		assert(m_rows == other.rows() && m_cols == other.cols());
		unsigned int i, ic = m_rows * m_cols;
		for (i=0; i<ic; i++) m_data[i] -= other.m_data[i];
	}
	void operator *= (double scalar)
	{
		unsigned int i, ic = m_rows * m_cols;
		for (i=0; i<ic; i++) m_data[i] *= scalar;
	}
	void operator /= (double scalar)
	{
		unsigned int i, ic = m_rows * m_cols;
		for (i=0; i<ic; i++) m_data[i] /= scalar;
	}
	Matrix operator + (Matrix const& other) const
	{
		assert(m_rows == other.rows() && m_cols == other.cols());
		Matrix ret(*this);
		ret += other;
		return ret;
	}
	Matrix operator - (Matrix const& other) const
	{
		assert(m_rows == other.rows() && m_cols == other.cols());
		Matrix ret(*this);
		ret -= other;
		return ret;
	}
	Matrix operator * (double scalar) const
	{
		Matrix ret(*this);
		ret *= scalar;
		return ret;
	}
	Matrix operator / (double scalar) const
	{
		Matrix ret(*this);
		ret /= scalar;
		return ret;
	}
	Matrix operator * (Matrix const& other) const
	{
		assert(m_cols == other.rows());
		Matrix ret(m_rows, other.cols());
		unsigned int i, j, k;
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<other.cols(); j++)
			{
				double value = 0.0;
				for (k=0; k<m_cols; k++)
				{
					value += (*this)(i, k) * other(k, j);
				}
				ret(i, j) = value;
			}
		}
		return ret;
	}
	Vector operator * (Vector const& other) const
	{
		assert(m_cols == other.dim());
		Vector ret(m_rows);
		unsigned int i, j;
		for (i=0; i<m_rows; i++)
		{
			double value = 0.0;
			for (j=0; j<m_cols; j++)
			{
				value += (*this)(i, j) * other[j];
			}
			ret[i] = value;
		}
		return ret;
	}

	// resize and erase old content
	void resize(unsigned int rows, unsigned int cols)
	{
		m_rows = rows;
		m_cols = cols;
		unsigned int i, ic = rows * cols;
		m_data.resize(ic);
		for (i=0; i<ic; i++) m_data[i] = 0.0;
	}

	Matrix transpose() const
	{
		Matrix ret(m_cols, m_rows);
		unsigned int i, j;
		for (i=0; i<m_cols; i++)
			for (j=0; j<m_rows; j++)
				ret(i, j) = (*this)(j, i);
		return ret;
	}

	Matrix sub(unsigned int yBegin, unsigned int yNum, unsigned int xBegin, unsigned int xNum) const
	{
		Matrix ret(yNum, xNum);
		unsigned int i, j;
		for (i=0; i<yNum; i++)
			for (j=0; j<xNum; j++)
				ret(i, j) = (*this)(yBegin + i, xBegin + j);
		return ret;
	}

	void print(const char* name = NULL) const
	{
		if (name != NULL) printf("%s:\n", name);
		unsigned int i, j;
		for (i=0; i<m_rows; i++)
		{
			for (j=0; j<m_cols; j++) printf(" %g", (*this)(i, j));
			printf("\n");
		}
	}

	// Singular Value Decomposition:
	// Decompose the current matrix into
	//    U diag(D) V^T
	// with U and V orthogonal.
	void svd(Matrix& U, Vector& D, Matrix& V, unsigned int iter = 200, bool ignoreError = true) const
	{
		Matrix const& amatA(*this);
		unsigned m = amatA.rows();
		unsigned n = amatA.cols();

		Matrix umatA(amatA);
		Matrix vmatA(n, n);
		Vector w(n);

		int flag;
		unsigned i, its, j, jj, k, l, nm(0);
		double anorm, c, f, g, h, s, scale, x, y, z;

		Vector rv1(n);

		g = scale = anorm = 0.0;

		for (i = 0; i < n; i++)
		{
			l = i + 1;
			rv1(i) = scale * g;
			g = s = scale = 0.0;

			if (i < m)
			{
				for (k = i; k < m; k++) scale += fabs(umatA(k, i));

				if (scale != 0.0)
				{
					for (k = i; k < m; k++)
					{
						umatA(k, i) /= scale;
						s += umatA(k, i) * umatA(k, i);
					}

					f = umatA(i, i);
					g = (f > 0.0) ? -::sqrt(s) : ::sqrt(s);
					h = f * g - s;
					umatA(i, i) = f - g;

					for (j = l; j < n; j++)
					{
						s = 0.0;
						for (k = i; k < m; k++) s += umatA(k, i) * umatA(k, j);

						f = s / h;
						for (k = i; k < m; k++) umatA(k, j) += f * umatA(k, i);
					}

					for (k = i; k < m; k++) umatA(k, i) *= scale;
				}
			}

			w(i) = scale * g;
			g = s = scale = 0.0;

			if (i < m && i != n - 1)
			{
				for (k = l; k < n; k++) scale += fabs(umatA(i, k));

				if (scale != 0.0)
				{
					for (k = l; k < n; k++)
					{
						umatA(i, k) /= scale;
						s += umatA(i, k) * umatA(i, k);
					}

					f = umatA(i, l);
					g = (f > 0.0) ? -::sqrt(s) : ::sqrt(s);
					h = f * g - s;
					umatA(i, l) = f - g;

					for (k = l; k < n; k++) rv1(k) = umatA(i, k) / h;

					for (j = l; j < m; j++)
					{
						s = 0.0;
						for (k = l; k < n; k++) s += umatA(j, k) * umatA(i, k);
						for (k = l; k < n; k++) umatA(j, k) += s * rv1(k);
					}

					for (k = l; k < n; k++) umatA(i, k) *= scale;
				}
			}

			anorm = std::max(anorm, fabs(w(i)) + fabs(rv1(i)));
		}

		for (l = i = n; i--; l--)
		{
			if (l < n)
			{
				if (g != 0.0)
				{
					for (j = l; j < n; j++) vmatA(j, i) = (umatA(i, j) / umatA(i, l)) / g;

					for (j = l; j < n; j++)
					{
						s = 0.0;
						for (k = l; k < n; k++) s += umatA(i, k) * vmatA(k, j);
						for (k = l; k < n; k++) vmatA(k, j) += s * vmatA(k, i);
					}
				}

				for (j = l; j < n; j++) vmatA(i, j) = vmatA(j, i) = 0.0;
			}

			vmatA(i, i) = 1.0;
			g = rv1(i);
		}

		for (l = i = std::min(m, n); i--; l--)
		{
			g = w(i);

			for (j = l; j < n; j++) umatA(i, j) = 0.0;

			if (g != 0.0)
			{
				g = 1.0 / g;
				for (j = l; j < n; j++)
				{
					s = 0.0;
					for (k = l; k < m; k++) s += umatA(k, i) * umatA(k, j);
					f = (s / umatA(i, i)) * g;
					for (k = i; k < m; k++) umatA(k, j) += f * umatA(k, i);
				}
				for (j = i; j < m; j++) umatA(j, i) *= g;
			}
			else
			{
				for (j = i; j < m; j++) umatA(j, i) = 0.0;
			}

			umatA(i, i)++;
		}

		for (k = n; k--;)
		{
			for (its = 1; its <= iter; its++)
			{
				flag = 1;

				for (l = k; l > 0; l--)
				{
					nm = l - 1;

					if (fabs(rv1(l)) + anorm == anorm)
					{
						flag = 0;
						break;
					}

					if (fabs(w(nm)) + anorm == anorm) break;
				}

				if (flag)
				{
					c = 0.0;
					s = 1.0;

					for (i = l; i <= k; i++)
					{
						f = s * rv1(i);
						rv1(i) *= c;

						if (fabs(f) + anorm == anorm) break;

						g = w(i);
						h = hypot(f, g);
						w(i) = h;
						h = 1.0 / h;
						c = g * h;
						s = -f * h;

						for (j = 0; j < m; j++)
						{
							y = umatA(j, nm);
							z = umatA(j, i);
							umatA(j, nm) = y * c + z * s;
							umatA(j, i ) = z * c - y * s;
						}
					}
				}

				z = w(k);

				if (l == k)
				{
					if (z < 0.0)
					{
						w(k) = -z;
						for (j = 0; j < n; j++) vmatA(j, k) = -vmatA(j, k);
					}
					break;
				}

				if( its == iter )
				{
					if (ignoreError) break;
					else throw "too many iterations for diagonalization of the bidiagonal form during SVD";
				}

				x = w(l);
				nm = k - 1;
				y = w(nm);
				g = rv1(nm);
				h = rv1(k);
				f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
				g = hypot(f, 1.0);
				f = ((x - z) * (x + z) + h * ((y / (f + ((f > 0.0) ? g : -g))) - h)) / x;

				c = s = 1.0;

				for (j = l; j < k; j++)
				{
					i = j + 1;
					g = rv1(i);
					y = w(i);
					h = s * g;
					g *= c;
					z = hypot(f, h);
					rv1(j) = z;
					c = f / z;
					s = h / z;
					f = x * c + g * s;
					g = g * c - x * s;
					h = y * s;
					y *= c;

					for (jj = 0; jj < n; jj++)
					{
						x = vmatA(jj, j);
						z = vmatA(jj, i);
						vmatA(jj, j) = x * c + z * s;
						vmatA(jj, i) = z * c - x * s;
					}

					z = hypot(f, h);
					w(j) = z;

					if (z != 0.0)
					{
						z = 1.0 / z;
						c = f * z;
						s = h * z;
					}

					f = c * g + s * y;
					x = c * y - s * g;

					for (jj = 0; jj < m; jj++)
					{
						y = umatA(jj, j);
						z = umatA(jj, i);
						umatA(jj, j) = y * c + z * s;
						umatA(jj, i) = z * c - y * s;
					}
				}

				rv1(l) = 0.0;
				rv1(k) = f;
				w(k) = x;
			}
		}

		// sort singular values
		U.resize(m, n);
		D.resize(n);
		V.resize(n, n);
		std::vector<unsigned int> index(n);
		for (i=0; i<n; i++) index[i] = i;
		CmpIndex cmpidx(w, index);
		std::sort(index.begin(), index.end(), cmpidx);
		for (i=0; i<n; i++)
		{
			j = index[i];
			D(i) = w[j];
			for (jj=0; jj<m; jj++) U(jj, i) = umatA(jj, j);
			for (jj=0; jj<n; jj++) V(jj, i) = vmatA(jj, j);
		}
	}

	// generalized inverse
	Matrix inv(double tolerance = 1e-10, unsigned int iter = 200, bool ignoreError = true)
	{
		unsigned int i, rank;
		unsigned int m = rows();
		unsigned int n = cols();

		Matrix U(m, n);
		Matrix V(n, n);
		Vector D(n);
		Matrix ret(n, m);

		svd(U, D, V, iter, ignoreError);

		// compute numerical rank
		{
			for (rank = 0; rank < n && D(rank) > 0.0; rank++);
			double t = (rank < n) ? fabs(D(n-1)) : 0.0;
			rank = 0;
			double s = 0.0;
			while (rank < n && D(rank) > t && D(rank) + s > s)
			{
				s += D(rank);
				rank++;
			}
		}

		// invert singular values
		for (i=0; i<rank; i++)
		{
			if (D(i) < tolerance) D(i) = 0.0; else D(i) = 1.0 / D(i);
		}
		for (; i<n; i++) D(i) = 0.0;

		// compute and return pseudo-inverse
		return fromSvd(U, D, V);
	}

	void operator *= (Matrix const& other)
	{ *this = Matrix::operator *((Matrix const&)other); }

	void eig(Matrix& U, Vector& lambda, unsigned int iter = 200, bool ignoreError = true) const
	{
		assert(m_rows == m_cols);

		Matrix basic(*this);
		U.resize(m_rows, m_cols);
		lambda.resize(m_rows);

		// 1-dim case
		if (m_rows == 1) { U(0, 0) = 1.0; lambda[0] = m_data[0]; return; }

		Vector eigenval(m_rows);
		Vector oD(m_rows);
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
			eigenval[i] = h;
		}
		eigenval[0] = oD[0] = 0.0;

		// accumulation of transformation matrices
		for (i=0; i < m_rows; i++)
		{
			if (eigenval[i] != 0.0)
			{
				for (j=0; j < i; j++)
				{
					g = 0.0;
					for (k = 0; k < i; k++) g += basic(i, k) * basic(k, j);
					for (k = 0; k < i; k++) basic(k, j) -= g * basic(k, i);
				}
			}
			eigenval[i] = basic(i, i);
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
					s = fabs(eigenval[m]) + fabs(eigenval[m + 1]);
					if (fabs(oD[m]) + s == s) break;
				}
				p = eigenval[l];

				if (m != l)
				{
					if (j++ == iter)
					{
						// Too many iterations --> numerical instability!
						if (ignoreError) break;
						else throw("[Arr::eig] numerical problem");
					}

					// form shift
					g = (eigenval[l+1] - p) / (2.0 * oD[l]);
					r = ::sqrt(g * g + 1.0);
					g = eigenval[m] - p + oD[l] / (g + ((g > 0.0) ? fabs(r) : -fabs(r)));
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

						g = eigenval[i+1] - p;
						r = (eigenval[i] - g) * s + 2.0 * c * b;
						p = s * r;
						eigenval[i+1] = g + p;
						g = c * r - b;

						for (k=0; k < m_rows; k++)
						{
							f = basic(k, i+1);
							basic(k, i+1) = s * basic(k, i) + c * f;
							basic(k, i) = c * basic(k, i) - s * f;
						}
					}

					eigenval[l] -= p;
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
		for (i=0; i<m_rows; i++)
		{
			j = index[i];
			lambda[i] = eigenval[j];
			for (k=0; k<m_rows; k++) U(k, i) = basic(k, j);
		}
	}

	static Matrix fromEig(Matrix const& U, Vector const& lambda)
	{
		// return U * Arr::diag(lambda.toFlat()) * (U^T);
		assert(U.rows() == U.cols() && U.rows() == lambda.dim());

		unsigned int n = lambda.size();
		Matrix ret(n, n);
		unsigned int i, j, k;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				double sum = 0.0;
				for (k = 0; k < n; k++) sum += U(i, k) * lambda[k] * U(j, k);
				ret(i, j) = sum;
			}
		}
		return ret;
	}

	static Matrix fromSvd(Matrix const& U, Vector const& D, Matrix const& V)
	{
// 		return U * Arr::diag(D) * (V^T);

		unsigned int m = U.rows();
		unsigned int n = U.cols();
		assert(D.size() == n && V.rows() == n);

		Matrix ret(n, m);
		unsigned int i, j, k;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < m; j++)
			{
				double sum = 0.0;
				for (k = 0; k < n; k++) sum += V(i, k) * D(k) * U(j, k);
				ret(i, j) = sum;
			}
		}
		return ret;
	}

	Matrix sqrt() const
	{
		assert(m_rows == m_cols);
		Matrix U;
		Vector lambda;
		eig(U, lambda);
		unsigned int i;
		for (i=0; i<m_rows; i++) lambda[i] = ::sqrt(lambda[i]);
		return fromEig(U, lambda);
	}

	Matrix exp() const
	{
		assert(m_rows == m_cols);
		Matrix U;
		Vector lambda;
		eig(U, lambda);
		unsigned int i;
		for (i=0; i<m_rows; i++) lambda[i] = ::exp(lambda[i]);
		return fromEig(U, lambda);
	}

	// return I
	static Matrix identity(unsigned int dim)
	{
		Matrix ret(dim, dim);
		unsigned int i;
		for (i=0; i<dim; i++) ret(i, i) = 1.0;
		return ret;
	}

	// return v * w^T
	static Matrix outerProduct(Vector const& v, Vector const& w)
	{
		unsigned int i, j;
		Matrix ret(v.dim(), w.dim());
		for (i=0; i<v.dim(); i++)
		{
			for (j=0; j<w.dim(); j++)
			{
				ret(i, j) = v[i] * w[j];
			}
		}
		return ret;
	}

	// return v * v^T
	static Matrix outerProduct(Vector const& v)
	{
		unsigned int i, j, dim = v.size();
		Matrix ret(dim, dim);
		for (i=0; i<dim; i++)
		{
			for (j=0; j<i; j++)
			{
				double p = v[i] * v[j];
				ret(i, j) = p;
				ret(j, i) = p;
			}
			ret(i, i) = v[i] * v[i];
		}
		return ret;
	}

	// return v * v^T - I
	static Matrix outerProductMinusIdentity(Vector const& v)
	{
		unsigned int i, j, dim = v.size();
		Matrix ret(dim, dim);
		for (i=0; i<dim; i++)
		{
			for (j=0; j<i; j++)
			{
				double p = v[i] * v[j];
				ret(i, j) = p;
				ret(j, i) = p;
			}
			ret(i, i) = v[i] * v[i] - 1.0;
		}
		return ret;
	}

	static Matrix diag(Vector const& vec)
	{
		unsigned int i, ic = vec.dim();
		Matrix ret(ic, ic);
		for (i=0; i<ic; i++) ret(i, i) = vec(i);
		return ret;
	}

	double tr() const
	{
		assert(m_rows == m_cols);
		double ret = 0.0;
		unsigned int i;
		for (i=0; i<m_rows; i++) ret += (*this)(i, i);
		return ret;
	}

	double det() const
	{
		assert(m_rows == m_cols);
		Matrix U, V;
		Vector lambda;
		svd(U, lambda, V);
		double ret = 1.0;
		unsigned int i, ic = lambda.dim();
		for (i=0; i<ic; i++) ret *= lambda[i];
		return ret;
	}

	void subtractFromTrace(double value)
	{
		assert(m_rows == m_cols);
		unsigned int i;
		for (i=0; i<m_rows; i++) (*this)(i, i) -= value;
	}

protected:
	static double sign(double t)
	{
		if (t < 0.0) return -1.0;
		else if (t > 0.0) return 1.0;
		else return 0.0;
	}

	unsigned int m_rows;
	unsigned int m_cols;
	std::vector<double> m_data;
};


#endif
