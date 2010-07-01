/*
 *  DetectionParams.h
 *
 *  Created by Alexander Fšrster on 7/18/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DotParam_ISDEF
#define DotParam_ISDEF
struct DetectionParams {
public:
	int redMin,redMax;
	int greenMin,greenMax;
	int blueMin,blueMax;
	int distMax;
	int pixelsMin;
	int pixelsMax;
	int sizeMax;
	int sizeMin;

	int hueMin,hueMax;
	int saturationMin,saturationMax;
	int valueMin,valueMax;
	void setHSVBoundaries(int h,int H, int s, int S, int v, int V);
	void setRGBBoundaries(int r,int R, int g, int G, int b, int B);
};

#endif
