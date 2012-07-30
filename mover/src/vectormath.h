/*******************************************************************
 ***        Copyright (C) 2012 Leo Pape                          ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <vector>
#include <cmath>
#include <limits>
#include <string>
#include <yarp/dev/all.h>
using namespace std;


// absolute of vector
static vector<double> abs(vector<double>& a) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = abs(a[i]); }
	return c;
}

static vector<vector<double> > abs(vector<vector<double> >& a) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = abs(a[i]); }
	return c;
}


// maximum vector element
static double max(vector<double>& a) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { c = max(c, a[i]); }
	return c;
}
static double max(vector<vector<double> >& a) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { c = max(c, max(a[i])); }
	return c;
}

// yarp sig vector conversion
static yarp::sig::Vector std2yarp(vector<double> &a) {
	yarp::sig::Vector y(a.size());
	for (int i=0; i<a.size(); i++) { y[i] = a[i]; }
	return y;
}
// yarp sig vector conversion
static vector<double> yarp2std(yarp::sig::Vector& y) {
	vector<double> a(y.size());
	for (int i=0; i<y.size(); i++) { a[i] = y[i]; }
	return a;
}

// maximum of vector and scalar
static vector<double> max(vector<double>& a, double b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = max(a[i], b); }
	return c;
}


// minimum of vector and scalar
static vector<double> min(vector<double>& a, double b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = min(a[i], b); }
	return c;
}


// maximum elements of two vectors
static vector<double> max(vector<double>& a, vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = max(a[i], b[i]); }
	return c;
}
static vector<vector<double> > max(vector<vector<double> >& a, vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = max(a[i], b[i]); }
	return c;
}


// minimum elements in two vectors
static vector<double> min(vector<double>& a, vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = min(a[i], b[i]); }
	return c;
}
static vector<vector<double> > min(vector<vector<double> >& a, vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = min(a[i], b[i]); }
	return c;
}


// subtract vector elements
static vector<double> operator-(vector<double> &a, vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] - b[i]; }
	return c;
}
static vector<vector<double> > operator-(vector<vector<double> >& a, vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] - b[i]; }
	return c;
}


// multiply vector elements
static vector<double> operator*(vector<double> &a, vector<double>& b) {
	vector<double> c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] * b[i]; }
	return c;
}
static vector<vector<double> > operator*(vector<vector<double> >& a, vector<vector<double> >& b) {
	vector<vector<double> > c(a.size());
	for (int i=0; i<a.size(); i++) { c[i] = a[i] * b[i]; }
	return c;
}


// sum vector elements
static double sum(vector<double> &a) {
	double c = 0.;
	for (int i=0; i<a.size(); i++) { c = c+a[i]; }
	return c;
}
static double sum(vector<vector<double> >& a) {
	double c = 0.;
	for (int i=0; i<a.size(); i++) { c = c + sum(a[i]); }
	return c;
}


// count number of elements
static size_t numel(vector<vector<double> >& a) {
	size_t c = 0;
	for (int i=0; i<a.size(); i++) { c = c + a[i].size(); }
	return c;
}


// root-mean-squared difference
static double rmse(vector<vector<double> >& a, vector<vector<double> >& b) {
	vector<vector<double> > diff = a - b;
	diff = (diff * diff);
	return sqrt(sum(diff) / numel(diff));
}


// masking functions
static int setmask(vector<double>& e, vector<bool>& mask, double maskValue) {
	int n = 0;
	for (int i=0; i<e.size(); i++) { if (mask[i]) {e[i] = maskValue;} else {n++;} }
	return n;
}

static int setmask(vector<vector<double> >& e, vector<vector<bool> >& mask, double maskValue) {
	int n = 0;
	for (int i=0; i<e.size(); i++) { n=n+setmask(e[i], mask[i], maskValue); }
	return n;
}


// masked rmse
static double rmse(vector<vector<double> >& a, vector<vector<double> >& b, vector<vector<bool> > &mask) {
	vector<vector<double> > diff = a - b;
	int n = setmask(diff, mask, 0.);
	diff = (diff * diff);
	return sqrt(sum(diff) / (double) n);
}

// masked maximum vector element
static double max(vector<double>& a, vector<bool>& mask) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { if (!mask[i]) { c = max(c, a[i]); }}
	return c;
}
static double max(vector<vector<double> >& a, vector<vector<bool> >& mask) {
	double c = -numeric_limits<double>::infinity();
	for (int i=0; i<a.size(); i++) { c = max(c, max(a[i], mask[i])); }
	return c;
}

#endif
/** @} */
