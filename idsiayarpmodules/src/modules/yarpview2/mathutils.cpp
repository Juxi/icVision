/*
 *  mathutils.cpp
 *  camview
 *
 *  Created by Alexander Förster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "mathutils.h"

float deg2rad(float deg)
{
	return deg*PI/180.0;
};

float rad2deg(float rad)
{
	return rad*180.0/PI;	
}; 

int maximum(int a,int b)
{
	if (a>b) 
		return a; 
	return b;
};

int minimum(int a,int b)
{
	if (a<b) 
		return a; 
	return b;
};

float maximum(float a,float b)
{
	if (a>b) 
		return a; 
	return b;
};

float minimum(float a,float b)
{
	if (a<b) 
		return a; 
	return b;
};

unsigned int argMaximum(float a,float b)
{
	if (a>b) 
		return 0; 
	return 1;
};

unsigned int argMinimum(float a,float b)
{
	if (a<b) 
		return 0; 
	return 1;
};

float minimum(float a, float b, float c)
{
	if (a<b)
		return minimum(a,c);
	return minimum(b,c);
};

float maximum(float a, float b, float c)
{
	if (a>b)
		return maximum(a,c);
	return maximum(b,c);	
};

unsigned int argMinimum(float a, float b, float c)
{
	if (a<c)
		return argMinimum(a,b);
	return 1+argMinimum(b,c);
};

unsigned int argMaximum(float a, float b, float c)
{
	if (a>c)
		return argMaximum(a,b);
	return 1+argMaximum(b,c);	
};



int minimum(int a, int b, int c)
{
	if (a<b)
		return minimum(a,c);
	return minimum(b,c);
};

int maximum(int a, int b, int c)
{
	if (a>b)
		return maximum(a,c);
	return maximum(b,c);	
};

unsigned int argMinimum(int a, int b, int c)
{
	if (a<c)
		return argMinimum(a,b);
	return 1+argMinimum(b,c);
};

unsigned int argMaximum(int a, int b, int c)
{
	if (a>c)
		return argMaximum(a,b);
	return 1+argMaximum(b,c);	
};

int putInRange(int val,int minval,int maxval)
{
	if (val<minval) return minval;
	if (val>maxval) return maxval;
	return val;
};
