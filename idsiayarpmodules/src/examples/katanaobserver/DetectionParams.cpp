/*
 *  DetectionParams.cpp
 *
 *  Created by Alexander Förster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DetectionParams.h"

void DetectionParams::setHSVBoundaries(int h,int H, int s, int S, int v, int V)
{
	hueMin = h;
	hueMax = H;
	saturationMin = s;
	saturationMax = S;
	valueMin = v;
	valueMax = V;	
}

void DetectionParams::setRGBBoundaries(int r,int R, int g, int G, int b, int B)
{
	redMin=r;
	redMax=R;
	greenMin=g;
	greenMax=G;
	blueMin=b;
	blueMax=B;
}
