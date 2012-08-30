/*******************************************************************
 ***        Copyright (C) 2012 Leo Pape                          ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

//#include <vector>
//#include <cmath>
//#include <limits>
//#include <string>
#include <yarp/dev/all.h>
using std::vector;
using std::numeric_limits;
using std::max; 
using std::min;

inline double sign(const double& v) {
    return ((v==0.0)?0.0:((v>0.0)?1.0:-1.0));
}

// absolute of vector
static vector<double> abs(const vector<double>& a) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = abs(a[i]); }
	return c;
}

static vector<vector<double> > abs(const vector<vector<double> >& a) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = abs(a[i]); }
	return c;
}


// maximum vector element
static double max(const vector<double>& a) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { c = max(c, a[i]); }
	return c;
}
static double max(const vector<vector<double> >& a) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { c = max(c, max(a[i])); }
	return c;
}

// yarp sig vector conversion
static yarp::sig::Vector std2yarp(const vector<double> &a) {
	yarp::sig::Vector y(a.size());
	for (int i=0; i<a.size(); i++) { y[i] = a[i]; }
	return y;
}
// yarp sig vector conversion
static vector<double> yarp2std(const yarp::sig::Vector& y) {
	vector<double> a(y.size());
	for (int i=0; i<y.size(); i++) { a[i] = y[i]; }
	return a;
}

// maximum of vector and scalar
static vector<double> max(const double& a, const vector<double>& b) {
	vector<double> c(b.size());
	for (int i=0; i<b.size(); i++) { c[i] = max(a, b[i]); }
	return c;
}
static vector<double> max(const vector<double>& a, const double& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = max(a[i], b); }
	return c;
}

// minimum of vector and scalar
static vector<double> min(const double& a, const vector<double>& b) {
	vector<double> c(b.size());
	for (int i=0; i<b.size(); i++) { c[i] = min(a, b[i]); }
	return c;
}
static vector<double> min(const vector<double>& a, const double& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = min(a[i], b); }
	return c;
}

// maximum elements of two vectors
static vector<double> max(const vector<double>& a, const vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = max(a[i], b[i]); }
	return c;
}
static vector<vector<double> > max(const vector<vector<double> >& a, const vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = max(a[i], b[i]); }
	return c;
}


// minimum elements in two vectors
static vector<double> min(const vector<double>& a, const vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = min(a[i], b[i]); }
	return c;
}
static vector<vector<double> > min(const vector<vector<double> >& a, const vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = min(a[i], b[i]); }
	return c;
}

// add vector elements
static vector<double> operator+(const double& a, const vector<double>& b) {
	vector<double> c(b.size());
	for (int i=0; i<b.size(); i++) { c[i] = a + b[i]; }
	return c;
}
static vector<double> operator+(const vector<double>& a, const double& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] + b; }
	return c;
}

// subtract vector elements
static vector<double> operator-(const double& a, const vector<double>& b) {
	vector<double> c(b.size());
	for (int i=0; i<b.size(); i++) { c[i] = a - b[i]; }
	return c;
}
static vector<double> operator-(const vector<double>& a, const double& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] - b; }
	return c;
}

static vector<double> operator-(const vector<double> &a, const vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] - b[i]; }
	return c;
}
static vector<vector<double> > operator-(const vector<vector<double> >& a, const vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] - b[i]; }
	return c;
}


// multiply vector elements
static vector<double> operator*(const double& a, const vector<double>& b) {
	vector<double> c(b.size());
	for (int i=0; i<b.size(); i++) { c[i] = a * b[i]; }
	return c;
}
static vector<double> operator*(const vector<double> &a, const vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] * b[i]; }
	return c;
}
static vector<vector<double> > operator*(const vector<vector<double> >& a, const vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] * b[i]; }
	return c;
}


// sum vector elements
static double sum(const vector<double> &a) {
	double c = 0.;
	for (int i=0; i<a.size(); i++) { c = c+a[i]; }
	return c;
}
static double sum(const vector<vector<double> >& a) {
	double c = 0.;
	for (int i=0; i<a.size(); i++) { c = c + sum(a[i]); }
	return c;
}


// count number of elements
static size_t numel(const vector<vector<double> >& a) {
	size_t c = 0;
	for (int i=0; i<a.size(); i++) { c = c + a[i].size(); }
	return c;
}


// root-mean-squared difference
static double rmse(const vector<vector<double> >& a, const vector<vector<double> >& b) {
	vector<vector<double> > diff = a - b;
	diff = (diff * diff);
	return sqrt(sum(diff) / numel(diff));
}


// masking functions
static int setmask(vector<double>& e, const vector<bool>& mask, const double& maskValue) {
	int n = 0;
	for (int i=0; i<e.size(); i++) { if (mask[i]) {e[i] = maskValue;} else {n++;} }
	return n;
}

static int setmask(vector<vector<double> >& e, const vector<vector<bool> >& mask, const double& maskValue) {
	int n = 0;
	for (int i=0; i<e.size(); i++) { n=n+setmask(e[i], mask[i], maskValue); }
	return n;
}


// masked rmse
static double rmse(const vector<vector<double> >& a, const vector<vector<double> >& b, const vector<vector<bool> > &mask) {
	vector<vector<double> > diff = a - b;
	int n = setmask(diff, mask, 0.);
	diff = (diff * diff);
	return sqrt(sum(diff) / (double) n);
}

// masked maximum vector element
static double max(const vector<double>& a, const vector<bool>& mask) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { if (!mask[i]) { c = max(c, a[i]); }}
	return c;
}
static double max(const vector<vector<double> >& a, const vector<vector<bool> >& mask) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { c = max(c, max(a[i], mask[i])); }
	return c;
}

#endif
/** @} */
