
#ifndef VECTOR_H
#define VECTOR_H


#include <math.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <stdio.h>

////////////////////////////////////////////////////////////
// Class representing a real vector, or tuple, of doubles,
// supportling standard linear algebra operations.
//
class Vector
{
public:
	Vector()
	: m_data(0)
	{ }

	explicit Vector(unsigned int dim)
	: m_data(dim)
	{
		unsigned int i;
		for (i=0; i<dim; i++) m_data[i] = 0.0;
	}

	Vector(unsigned int dim, const double* data)
	: m_data(dim)
	{
		unsigned int i;
		for (i=0; i<dim; i++) m_data[i] = data[i];
	}

	Vector(Vector const& other)
	: m_data(other.m_data)
	{ }

// 	Vector(std::vector<double> const& stdvec)
// 	: m_data(stdvec)
// 	{ }

	void operator = (Vector const& other)
	{ m_data = other.m_data; }

	void assign(unsigned int n, const double* data)
	{
		unsigned int i;
		m_data.resize(n);
		for (i=0; i<n; i++) m_data[i] = data[i];
	}

	unsigned int dim() const
	{ return m_data.size(); }
	unsigned int size() const
	{ return m_data.size(); }
	double& operator [] (unsigned int index)
	{ return m_data[index]; }
	double const& operator [] (unsigned int index) const
	{ return m_data[index]; }
	double& operator () (unsigned int index)
	{ return m_data[index]; }
	double const& operator () (unsigned int index) const
	{ return m_data[index]; }

	void operator += (Vector const& other)
	{
		assert(other.dim() == dim());
		unsigned int i, ic = size();
		for (i=0; i<ic; i++) m_data[i] += other.m_data[i];
	}
	void operator -= (Vector const& other)
	{
		assert(other.dim() == dim());
		unsigned int i, ic = size();
		for (i=0; i<ic; i++) m_data[i] -= other.m_data[i];
	}
	void operator *= (double scalar)
	{
		unsigned int i, ic = size();
		for (i=0; i<ic; i++) m_data[i] *= scalar;
	}
	void operator /= (double scalar)
	{
		unsigned int i, ic = size();
		for (i=0; i<ic; i++) m_data[i] /= scalar;
	}
	Vector operator + (Vector const& other) const
	{
		assert(other.dim() == dim());
		Vector ret(*this);
		ret += other;
		return ret;
	}
	Vector operator - (Vector const& other) const
	{
		assert(other.dim() == dim());
		Vector ret(*this);
		ret -= other;
		return ret;
	}
	Vector operator * (double scalar) const
	{
		Vector ret(*this);
		ret *= scalar;
		return ret;
	}
	Vector operator / (double scalar) const
	{
		Vector ret(*this);
		ret /= scalar;
		return ret;
	}
	double operator * (Vector const& other) const
	{
		assert(other.dim() == dim());
		double ret = 0.0;
		unsigned int i, ic = size();
		for (i=0; i<ic; i++) ret += m_data[i] * other.m_data[i];
		return ret;
	}

	// resize, keep old content
	void resize(unsigned int dim)
	{
		unsigned int i, olddim = m_data.size();
		m_data.resize(dim);
		for (i=olddim; i<dim; i++) m_data[i] = 0.0;
	}

	Vector sub(unsigned int start, unsigned int num) const
	{
		Vector ret(num);
		unsigned int i;
		for (i=0; i<num; i++) ret(i) = operator () (start + i);
		return ret;
	}

	double twonorm2() const
	{ return (*this) * (*this); }
	double twonorm() const
	{ return sqrt(twonorm2()); }
	void normalize()
	{ (*this) /= twonorm(); }

	void print(const char* name = NULL) const
	{
		if (name != NULL) printf("%s:\n", name);
		unsigned int i;
		for (i=0; i<dim(); i++) printf(" %g", (*this)(i));
		printf("\n");
	}

protected:
	std::vector<double> m_data;
};


#endif
