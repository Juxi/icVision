/*
 *  GaborImage.cpp
 *  IDSIAVision
 *
 *  Created by Juxi Leitner on 10/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "GaborImage.h"
#include <math.h>

GaborImage::GaborImage() {
	GaborWidth = 9;
	GaborHeight = 9;
	orientation = 8;
	frequency = 3;
	
	// moved here from init
	KernelRealData = cvCreateMat(GaborWidth, GaborHeight, CV_32F);
	KernelImgData = cvCreateMat(GaborWidth, GaborHeight, CV_32F);
}

GaborImage::~GaborImage() {
	cvReleaseMat(&KernelRealData);
	cvReleaseMat(&KernelImgData);
}

void GaborImage::Init()	{
	// moved to constructor
//	KernelRealData = cvCreateMat(GaborWidth, GaborHeight, CV_32F);
//	KernelImgData = cvCreateMat(GaborWidth, GaborHeight, CV_32F);
}

void GaborImage::CalculateKernel(int Orientation, int Frequency) {
	Init();
	
	float real, img;
	
	for (int x = -(GaborWidth - 1) / 2; x < (GaborWidth - 1) / 2 + 1; x++)
		for (int y = -(GaborHeight - 1) / 2; y < (GaborHeight - 1) / 2 + 1; y++)
		{
			real = KernelRealPart(x, y, Orientation, Frequency);
			img = KernelImgPart(x, y, Orientation, Frequency);
			
			cvSetReal2D(KernelRealData, x + (GaborWidth - 1) / 2, y + (GaborHeight - 1) / 2, real);
			cvSetReal2D(KernelImgData,  x + (GaborWidth - 1) / 2, y + (GaborHeight - 1) / 2, img);
		}
}


float GaborImage::KernelRealPart(int x, int y, int Orientation, int Frequency)
{
	double U, V;
	double Sigma, Kv, Qu;
	double tmp1, tmp2;
	
	U = Orientation;
	V = Frequency;
	Sigma = 2 * M_PI * M_PI;
	Kv = M_PI * exp((-(V + 2) / 2) * log2(M_E));
	Qu = U * M_PI / 8;
	
	tmp1 = exp(-(Kv * Kv * (x * x + y * y) / (2 * Sigma)));
	tmp2 = cos(Kv * cos(Qu) * x + Kv * sin(Qu) * y) - exp(-(Sigma / 2));
	
	return (float)(tmp1 * tmp2 * Kv * Kv / Sigma);
}


float GaborImage::KernelImgPart(int x, int y, int Orientation, int Frequency)
{
	double U, V;
	double Sigma, Kv, Qu;
	double tmp1, tmp2;
	
	U = Orientation;
	V = Frequency;
	Sigma = 2 * M_PI * M_PI;
	Kv = M_PI * exp((-(V + 2) / 2) * log2(M_E));
	Qu = U * M_PI / 8;
	
	tmp1 = exp(-(Kv * Kv * (x * x + y * y) / (2 * Sigma)));
	tmp2 = sin(Kv * cos(Qu) * x + Kv * sin(Qu) * y) - exp(-(Sigma / 2));
	
	return (float)(tmp1 * tmp2 * Kv * Kv / Sigma);
}

Img GaborImage::GaborTransform(Img Image, int Orientation) {
	orientation = Orientation;
	CalculateKernel(8, Orientation);

	Img retImg  = (IplImage*) cvClone(Image);
	
	Img gabor_real = (IplImage*) cvClone(Image);
	Img gabor_img  = (IplImage*) cvClone(Image);
	cvFilter2D(Image, gabor_real, KernelRealData);	//image.Convolution(this.KernelRealData);
	cvFilter2D(Image, gabor_img , KernelImgData);	//image.Convolution(this.KernelImgData);
	
	cvPow(gabor_real, gabor_real, 2);
	cvPow(gabor_img,  gabor_img,  2);
	
	// Img gabor = (gabor_real + gabor_img).Pow(0.5);
	cvAdd(gabor_real, gabor_img, retImg);
	cvPow(retImg, retImg, 0.5);
	
	cvReleaseImage(&gabor_real);
	cvReleaseImage(&gabor_img);
	
	return retImg;
}