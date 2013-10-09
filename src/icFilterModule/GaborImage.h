// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _GABOR_IMAGE_H_
#define _GABOR_IMAGE_H_

#include <opencv/cv.h>
#include <highgui.h>
#include <yarp/sig/all.h>
#include <cstdlib>
#include <iostream>

#define Img IplImage*
#define null NULL
#define ConvolutionKernelF	CvMat*

class GaborImage {
private:
	int GaborWidth, GaborHeight;
	int orientation, frequency;

	void Init();
	void CalculateKernel(int Orientation, int Frequency);
	float KernelRealPart(int x, int y, int Orientation, int Frequency);	
	float KernelImgPart(int x, int y, int Orientation, int Frequency);

	
public:
	GaborImage();
	GaborImage(int, int);
	~GaborImage();

	Img GaborTransform(Img Image, int Frequency, int Orientation);
	ConvolutionKernelF KernelRealData;
	ConvolutionKernelF KernelImgData;
};

#endif