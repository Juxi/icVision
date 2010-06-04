/*
 *  imageutils.cpp
 *  camview
 *
 *  Created by Alexander Förster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

// see http://en.wikipedia.org/wiki/HSL_color_space  
// TODO: without floats
#if 0
void rgb2hsv(int r,int g,int b,int &h,int& s,int &v)
{
	float red=r/255.0,green=g/255.0,blue=b/255.0;
	float hue,saturation,value;
	float mx=maximum(red,green,blue),mn=minimum(red,green,blue);
	unsigned int mxarg=argMaximum(red,green,blue),mnarg=argMinimum(red,green,blue);
	if (mnarg==mxarg)
	{
		hue = 0;
	}
	else if (mxarg==0) // blue
	{
		hue = 60*(green-blue)/(mx-mn);
		if (hue < 0.0) hue += 360.0; 
	}
	else if (mxarg==1) //green
	{
		hue = 60*(blue-red)/(mx-mn)+120;
	}
	else // (mxarf==2) // blue
	{
		hue = 60*(red-green)/(mx-mn)+240;
	};
	while (hue >= 360.0) 
	{
		hue -= 360.0;
	};
	value = mx;
	if (mx==0)
	{
		saturation = 0;
	} 
	else 
	{
		saturation = (mx-mn)/mx; // == 1-(mn/mx)
	};
	h = hue*255.0/359.0;s=saturation*255;v=value*255;
	//printf ("mx %f mn %f amx %i amn %i\n",mx*255,mn*255,mxarg,mnarg);
	//printf ("r %i g %i b %i ",r,g,b);
	//printf ("h %i s %i v %i\n",h,s,v);
};
#endif

#if 1
void rgb2hsv(int red,int green,int blue,int &hue,int& saturation,int &value)
{
	int mx=maximum(red,green,blue),mn=minimum(red,green,blue);
	unsigned int mxarg=argMaximum(red,green,blue),mnarg=argMinimum(red,green,blue);
	if (mnarg==mxarg)
	{
		hue = 0;
	}
	else if (mxarg==0) // blue
	{
		hue = 60*(green-blue)/(mx-mn);
		if (hue < 0.0) hue += 360.0; 
	}
	else if (mxarg==1) //green
	{
		hue = 60*(blue-red)/(mx-mn)+120;
	}
	else // (mxarf==2) // blue
	{
		hue = 60*(red-green)/(mx-mn)+240;
	};
	while (hue >= 360.0) 
	{
		hue -= 360.0;
	};
	value = mx;
	if (mx==0)
	{
		saturation = 0;
	} 
	else 
	{
		saturation = 255*(mx-mn)/mx; // == 1-(mn/mx)
	};
	hue = hue*256/360;
	assert (hue>=0 && saturation>=0 && value>=0);
	assert (hue<=255 && saturation<=255 && value<=255);
};
#endif

#if 0
// old, original, slow
void rgb2hsl(int r,int g,int b,int &h,int &s,int &l)  // synonyms HSI and HSB
{
	float red=r/255.,green=g/255.,blue=b/255.;
	float hue,saturation,lightness;
	float mx=maximum(red,green,blue),mn=minimum(red,green,blue);
	
	int mxarg=argMaximum(red,green,blue),mnarg=argMinimum(red,green,blue);
	if (mnarg==mxarg)
	{
		hue = 0;
	}
	else if (mxarg==0) // blue
	{
		hue = 60*(green-blue)/(mx-mn);
		if (hue < 0.0) hue += 360.0;
	}
	else if (mxarg==1) //green
	{
		hue = 60*(blue-red)/(mx-mn)+120;
	}
	else // (mxarf==2) // blue
	{
		hue = 60*(red-green)/(mx-mn)+240;
	};
	while (hue >= 360) 
	{
		hue -= 360;
	};
	lightness = (mx+mn)/2;
	if (mxarg==mnarg)
	{
		saturation = 0;
	} 
	else if (lightness <= 0.5)
	{
		saturation = (mx-mn)/(mx+mn); // == (mx-mn)/(2*lightness)
	}
	else // (lightness > 0.5)
	{
		saturation = (mx-mn)/(2-(mx+mn)); // == (mx-mn)/(2-2*lightness)
	};	
	h = hue*255/359;s=saturation*255;l=lightness*255;
	
};
#endif

#if 1
// optimized
void rgb2hsl(int red,int green,int blue,int &hue,int &saturation,int &lightness)  // synonyms HSI and HSB
{
	int mx=maximum(red,green,blue),mn=minimum(red,green,blue);
	
	int mxarg=argMaximum(red,green,blue),mnarg=argMinimum(red,green,blue);
	if (mnarg==mxarg)
	{
		hue = 0;
	}
	else if (mxarg==0) // blue
	{
		hue = 60*(green-blue)/(mx-mn);
		if (hue < 0.0) hue += 360;
	}
	else if (mxarg==1) //green
	{
		hue = 60*(blue-red)/(mx-mn)+120;
	}
	else // (mxarf==2) // blue
	{
		hue = 60*(red-green)/(mx-mn)+240;
	};
	while (hue >= 360) 
	{
		hue -= 360;
	};
	lightness = (mx+mn)/2;
	if (mxarg==mnarg)
	{
		saturation = 0;
	} 
	else if (mx+mn <= 255)
	{
		saturation = 255*(mx-mn)/(mx+mn); // == (mx-mn)/(2*lightness)
	}
	else // (lightness > 127)
	{
		saturation = 255*(mx-mn)/(510-mx-mn); // == (mx-mn)/(2-2*lightness)
	};	
	hue = hue*256/360;
	assert (hue>=0 && saturation>=0 && lightness>=0);
	assert (hue<=255 && saturation<=255 && lightness<=255);
};
#endif
void hsv2rgb(int h,int s,int v,int &r,int &g,int &b)
{
	float hue=h*359.0/255,saturation=s/255.0,value=v/255.0;	
	float red,green,blue;
	float hi,f,p,q,t;
	hi = floorf(hue/60.0);
	f = hue/60.0-hi;
	p = value*(1-saturation);
	q = value*(1-f*saturation);
	t = value*(1-(1-f)*saturation);
	if (hi==0) 
	{
		red=value;green=t;blue=p;
	}
	else if (hi==1)
	{
		red=q;green=value;blue=p;
	}
	else if (hi==2)
	{
		red=p;green=value;blue=t;
	}
	else if (hi==3)
	{
		red=p;green=q;blue=value;
	}
	else if (hi==4)
	{
		red=t;green=p;blue=value;
	}
	else if (hi==5)
	{
		red=value;green=p;blue=q;
	}
	else // oh no!
	{
		printf ("h %i s %i v %i\n",h,s,v);
		printf ("h %f s %f v %f hi %f\n",hue,saturation,value,hi);
		assert(0);
	};
	
	
	r=red*255;g=green*255;b=blue*255;	
};

void hsl2rgb(int h,int s,int l,int &r,int &g,int &b)
{
	float hue=h*359.0/255,saturation=s/255.0,lightness=l/255.0;	
	float red,green,blue;
	float q,p,hk,tr,tg,tb;
	if (lightness<0.5)
	{
		q = lightness*(1+saturation);
	}
	else // (lightness>=0.5)
	{
		q = lightness+saturation-(lightness*saturation);
	};
	p= 2.0 * lightness-q;
	hk = hue/359.0;
	tr = hk+1.0/3.0;
	tg = hk;
	tb = hk-1.0/3.0;
	if (tr<0.0) tr+=1.0;
	if (tr>1.0) tr-=1.0;
	if (tg<0.0) tg+=1.0;
	if (tg>1.0) tg-=1.0;
	if (tb<0.0) tb+=1.0;
	if (tb>1.0) tb-=1.0;
	if (tr<1.0/6.0) 
	{
		red = p +((q-p) * 6.0 *tr);
	}
	else if (1.0/6.0<=tr && tr<1.0/2.0) 
	{
		red = q;
	}
	else if (1.0/2.0<=tr && tr <= 2.0/3.0)
	{
		red = p+((q-p) * 6 * (2.0/3.0-tr));
	}
	else 
	{
		red = p;
	};
	if (tg<1.0/6.0) 
	{
		green = p +((q-p) * 6.0 *tg);
	}
	else if (1.0/6.0<=tg && tg<1.0/2.0) 
	{
		green = q;
	}
	else if (1.0/2.0<=tg && tg <= 2.0/3.0)
	{
		green = p+((q-p) * 6 * (2.0/3.0-tg));
	}
	else 
	{
		green = p;
	};
	if (tb<1.0/6.0) 
	{
		blue = p +((q-p) * 6.0 *tb);
	}
	else if (1.0/6.0<=tb && tb<1.0/2.0) 
	{
		blue = q;
	}
	else if (1.0/2.0<=tb && tb <= 2.0/3.0)
	{
		blue = p+((q-p) * 6 * (2.0/3.0-tb));
	}
	else 
	{
		blue = p;
	};
	
	r=red*255;g=green*255;b=blue*255;	
};

void yuv2rgb(int y,int u,int v,int &r,int &g,int &b)
{
	r = y + ((v*1436) >> 10);
	g = y - ((u*352 + v*731) >> 10);
	b = y + ((u*1814) >> 10);
	r = r < 0 ? 0 : r;
	g = g < 0 ? 0 : g;
	b = b < 0 ? 0 : b;
	r = r > 255 ? 255 : r;
	g = g > 255 ? 255 : g;
	b = b > 255 ? 255 : b; 
};

void rgb2yuv(int r,int g,int b,int &y,int &u,int &v)
{
	y = (306*r + 601*g + 117*b)  >> 10;
	u = ((-172*r - 340*g + 512*b) >> 10)  + 128;
	v = ((512*r - 429*g - 83*b) >> 10) + 128;
	y = y < 0 ? 0 : y;
	u = u < 0 ? 0 : u;
	v = v < 0 ? 0 : v;
	y = y > 255 ? 255 : y;
	u = u > 255 ? 255 : u;
	v = v > 255 ? 255 : v;
};

