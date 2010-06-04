/*
 *  mathutils.h
 *  camview
 *
 *  Created by Alexander Förster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MATHUTILS_ISDEF
#define MATHUTILS_SISDEF

#define PI 3.141592653589793

float deg2rad(float deg);
float rad2deg(float rad);
int maximum(int a,int b);
int minimum(int a,int b);
float maximum(float a,float b);
float minimum(float a,float b);
unsigned int argMaximum(float a,float b);
unsigned int argMinimum(float a,float b);
float minimum(float a, float b, float c);
float maximum(float a, float b, float c);
unsigned int argMinimum(float a, float b, float c);
unsigned int argMaximum(float a, float b, float c);
int minimum(int a, int b, int c);
int maximum(int a, int b, int c);
unsigned int argMinimum(int a, int b, int c);
unsigned int argMaximum(int a, int b, int c);
int putInRange(int val,int minval,int maxval);

#endif
