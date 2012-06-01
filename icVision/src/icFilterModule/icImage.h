// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_ICIMAGE_H_
#define _ICVISION_ICIMAGE_H_

#include <opencv/cv.h>
#include <highgui.h>
#include <yarp/sig/all.h>
#include <cstdlib>
#include <iostream>

#define Img IplImage*
#define null NULL

/*public*/ class icImage {
// (IplImage*)iCubImage->getIplImage() from icub
public:
	Img Image;
	
	// Constructors
	icImage() {
		Image = NULL;
	}
	
	icImage(Img img_in) {
		m_width = img_in->width;
		m_height = img_in->height;		
		Image = img_in;
	}

	icImage(CvSize size) {
		Image = cvCreateImage(size, IPL_DEPTH_32F, 1);		
	}
	
	icImage(double Value, int Width, int Height) {
		Image = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_32F, 1);
		cvSet(Image, cvRealScalar(Value));
	}	
	
	// Destructor
	~icImage() {
		cvReleaseImage(&Image);
	}
	
	icImage* Clone() const {
		icImage* newImg = new icImage();
		newImg->Image = (IplImage*) cvClone(this->Image);
		
		return newImg;
	}
		
	CvSize getSize(){
		return cvGetSize(Image);
	}
	
//	Rectangle ROI
//	{
//		get { return this.Image.ROI; }
//		set { this.Image.ROI = value; }
//	}
	
//	int getWidth()  { return Image->Width; } }
//	int getHeight() { return Image->Height; } }
	
	icImage* add (icImage* a) const;
	icImage* sub (icImage* a) const;
	icImage* mul (icImage* a) const;
	icImage* avg (icImage* a) const;
	icImage* mulc(double v) const;
	icImage* addc(double v) const;
	icImage* subc(double v) const;
	icImage* absdiff(icImage* a) const;
	
	icImage* canny  (double v) const;
	icImage* dilate (int) const;
	icImage* erode  (int) const;
	icImage* laplace(int) const;
	
	icImage* log(void) const;
	icImage* pow(double P) const;	
	icImage* exp() const;
	icImage* sqrt() const;	
	
	icImage* max (icImage* a) const;
	icImage* min (icImage* a) const;
	icImage* gauss (int) const;
	
	icImage* sobelx (int) const;
	icImage* sobely(int) const;
	icImage* unsharpen(int) const;
	icImage* threshold (double) const;
	icImage* thresholdInv (double) const;

	icImage* SmoothMedian (int) const;
	icImage* SmoothBilateral (int) const;		
	icImage* SmoothBlur (int) const;
	icImage* Normalize(void) const;
	
	icImage* gabor(int, int) const;
	icImage* ResizeThenGabor(int, int, double Scale) const;
	
	icImage* ShiftDown() const;
	icImage* ShiftUp() const;
	icImage* ShiftLeft() const;
	icImage* ShiftRight() const;	
		
	icImage* ReScale(double) const;
	icImage* LocalMax(int) const;
	
	void Save(std::string FileName) const;
	
private:
	int m_width, m_height;
	
};

//	static float ShiftDownMatrix[][3] = { 0, 1, 0 , 0, 0, 0 , 0, 0, 0 };//[ [0, 1, 0] , [0, 0, 0] , [0, 0, 0] ];//{ { 0, 1, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };//[ [0, 1, 0] , [0, 0, 0] , [0, 0, 0] ];
//	static ConvolutionKernelF ShiftDownKernel = new ConvolutionKernelF(ShiftDownMatrix);
//	static float[,] ShiftUpMatrix = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 } };
//	static ConvolutionKernelF ShiftUpKernel = new ConvolutionKernelF(ShiftDownMatrix);
//	static float[,] ShiftLeftMatrix = { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0, 0 } };
//	static ConvolutionKernelF ShiftLeftKernel = new ConvolutionKernelF(ShiftDownMatrix);
//	static float[,] ShiftRightMatrix = { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 0 } };
//	static ConvolutionKernelF ShiftRightKernel = new ConvolutionKernelF(ShiftDownMatrix);


#endif