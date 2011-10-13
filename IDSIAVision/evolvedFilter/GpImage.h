// Copyright: (C) 2011 Juxi Leitner, Simon Harding
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _GPIMAGE_H_
#define _GPIMAGE_H_

#include <opencv/cv.h>
#include <highgui.h>
#include <yarp/sig/all.h>
#include <cstdlib>
#include <iostream>

#define Img IplImage*
#define null NULL

/*public*/ class GpImage {
// (IplImage*)iCubImage->getIplImage() from icub
public:
	Img Image;
	
	// Constructors
	GpImage() {
		Image = NULL;
	}
	
	GpImage(Img img_in) {
		m_width = img_in->width;
		m_height = img_in->height;		
		Image = img_in;
	}

	GpImage(CvSize size) {
		Image = cvCreateImage(size, IPL_DEPTH_32F, 1);		
	}
	
	GpImage(double Value, int Width, int Height) {
		Image = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_32F, 1);
		cvSet(Image, cvRealScalar(Value));
	}	
	
	// Destructor
	~GpImage() {
		cvReleaseImage(&Image);
	}
	
	GpImage* Clone() {
		GpImage* newImg = new GpImage();
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
	
	GpImage* add (GpImage* a) const;
	GpImage* sub (GpImage* a) const;
	GpImage* mul (GpImage* a) const;
	GpImage* avg (GpImage* a) const;
	GpImage* mulc(double v) const;
	GpImage* addc(double v) const;
	GpImage* subc(double v) const;
	GpImage* absdiff(GpImage* a) const;
	
	GpImage* canny  (double v) const;
	GpImage* dilate (int) const;
	GpImage* erode  (int) const;
	GpImage* laplace(int) const;
	
	GpImage* log(void) const;
	GpImage* Pow(double P) const;	
	
	GpImage* max (GpImage* a) const;
	GpImage* min (GpImage* a) const;
	GpImage* gauss (int) const;
	
	GpImage* sobelx (int) const;
	GpImage* sobely(int) const;
	GpImage* unsharpen(int) const;
	GpImage* threshold (double) const;
	GpImage* SmoothMedian (int) const;
	GpImage* Normalize(void) const;
	
	GpImage* gabor(int) const;
	GpImage* ResizeThenGabor(int Orientation, double Scale) const;
	
	GpImage* ShiftDown() const;
	GpImage* ShiftUp() const;
	GpImage* ShiftLeft() const;
	GpImage* ShiftRight() const;	
		
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