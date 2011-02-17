/*
 *  imageutils.h
 *  camview
 *
 *  Created by Alexander Förster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IMAGEUTILS_ISDEF
#define IMAGEUTILS_ISDEF

void rgb2hsv(int r,int g,int b,int &h,int& s,int &v); // r g b h s v are in range 0..255
void rgb2hsl(int r,int g,int b,int &h,int &s,int &l);  // synonyms HSI and HSB
void hsv2rgb(int h,int s,int v,int &r,int &g,int &b);
void hsl2rgb(int h,int s,int l,int &r,int &g,int &b);
void yuv2rgb(int y,int u,int v,int &r,int &g,int &b);
void rgb2yuv(int r,int g,int b,int &y,int &u,int &v);
#endif
