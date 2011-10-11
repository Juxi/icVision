/*
 *  GpImage.cpp
 *  IDSIAVision
 *
 *  Created by Juxi Leitner on 10/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "GpImage.h"
#include "GaborImage.h"


GpImage* GpImage::add (GpImage* a) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvAdd(Image, a->Image, retImg);
	return new GpImage(retImg);
}
GpImage* GpImage::sub (GpImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvSub(Image, a->Image, retImg);
	return new GpImage(retImg);
}
GpImage* GpImage::mul (GpImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvMul(Image, a->Image, retImg);
	return new GpImage(retImg);
}
GpImage* GpImage::avg (GpImage* a) const { 
//GpImage* GpImage::avg (GpImage a) { Img Temp = this.Image + a.Image; Temp._Mul(0.5); return new GpImage(Temp); }
	Img retImg = (IplImage*) cvClone(Image);
	Img unity  = (IplImage*) cvClone(Image);

	// adding two pictures together
	cvAdd(Image, a->Image, retImg);
	
	// multiply with 0.5
	cvSet(unity, cvRealScalar(1.0));
	cvMul(retImg, unity, retImg, 0.5);

	cvReleaseImage(&unity);
	
	return new GpImage(retImg);
}

GpImage* GpImage::mulc(double v) const { 
	Img retImg = (IplImage*) cvClone(Image);
	Img unity  = (IplImage*) cvClone(Image);
	cvSet(unity, cvRealScalar(1.0));
	cvMul(Image, unity, retImg, v);
	cvReleaseImage(&unity);
	return new GpImage(retImg);
}

GpImage* GpImage::addc(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvAddS(Image, cvRealScalar(v), retImg);
	return new GpImage(retImg);
}
GpImage* GpImage::subc(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvSubS(Image, cvRealScalar(v), retImg);
	return new GpImage(retImg);	
}
GpImage* GpImage::absdiff(GpImage* a) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvAbsDiff(Image, a->Image, retImg);
	return new GpImage(retImg);	
}

GpImage* GpImage::canny(double v) const {
	//	Value = this.Image.Canny(new Gray(v), new Gray(10));
	Img retImg = (IplImage*) cvClone(Image);
	cvCanny(Image, retImg, v, 10);
	return new GpImage(retImg);
}

GpImage* GpImage::dilate(int Iterations) const {
			// Value = this.Image.Dilate(Iterations);
	Img retImg = (IplImage*) cvClone(Image);
	cvDilate(Image, retImg, NULL, Iterations);
	return new GpImage(retImg);
}

GpImage* GpImage::erode(int Iterations) const {
//	Value = this.Image.Erode(Iterations);
	Img retImg = (IplImage*) cvClone(Image);
	cvErode(Image, retImg, NULL, Iterations);
	return new GpImage(retImg);
}

GpImage* GpImage::laplace(int Aperture) const {
	//	Value = this.Image.Laplace(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvLaplace(Image, retImg, Aperture);
	return new GpImage(retImg);	
}

GpImage* GpImage::log() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvLog(Image, retImg);
	return new GpImage(retImg);	
}

GpImage* GpImage::Pow(double P) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvPow(Image, retImg, P);
	return new GpImage(retImg);	
}


GpImage* GpImage::max(GpImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvMax(Image, a->Image, retImg);
	return new GpImage(retImg);	
}
GpImage* GpImage::min(GpImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvMin(Image, a->Image, retImg);
	return new GpImage(retImg);	
}
	
GpImage* GpImage::gauss(int Aperture) const {
//		Value = this.Image.SmoothGaussian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_GAUSSIAN, Aperture, Aperture);
	return new GpImage(retImg);	
}
GpImage* GpImage::sobelx(int Aperture) const {
//		Value = this.Image.Sobel(1, 0, Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSobel(Image, retImg, 1, 0, Aperture);
	return new GpImage(retImg);	
}
GpImage* GpImage::sobely(int Aperture) const {
	//		Value = this.Image.Sobel(1, 0, Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSobel(Image, retImg, 0, 1, Aperture);
	return new GpImage(retImg);	
}
GpImage* GpImage::unsharpen(int Aperture) const {
//		Img blurred = this.Image.SmoothGaussian(Apertrue);
//		Img unsharpenmask = this.Image.Sub(blurred);
//		Img hicontrast = unsharpenmask.Mul(1.5);/
//		Value = this.Image.Add(hicontrast);
	Img retImg = (IplImage*) cvClone(Image);

	Img blurred = (IplImage*) cvClone(Image);
	Img unsharpenmask = (IplImage*) cvClone(Image);
	Img hicontrast = (IplImage*) cvClone(Image);
	Img cv1 = (IplImage*) cvClone(Image);	
	
	cvSmooth(Image, blurred, CV_GAUSSIAN, Aperture, Aperture);
	cvSub(Image, blurred, unsharpenmask);
	cvSet(cv1, cvRealScalar(1.0));
	cvMul(unsharpenmask, cv1, hicontrast, 1.5);
	
	cvAdd(Image, hicontrast, retImg);

	cvReleaseImage(&blurred);
	cvReleaseImage(&unsharpenmask);
	cvReleaseImage(&hicontrast);
	cvReleaseImage(&cv1);
	
	return new GpImage(retImg);	
}
GpImage* GpImage::threshold(double v) const {
//		Value = this.Image.ThresholdBinary(new Gray(v), new Gray(255));
	Img retImg = (IplImage*) cvClone(Image);
	cvThreshold(Image, retImg, v, 255.0, CV_THRESH_BINARY);
	return new GpImage(retImg);		
}

GpImage* GpImage::SmoothMedian(int Aperture) const { 
//		Value = this.Image.SmoothMedian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_MEDIAN, Aperture);
	return new GpImage(retImg);			
}

GpImage* GpImage::gabor(int p) const {
	//GaborEmguImg gab = new GaborEmguImg();
//		Value = gab.GaborTransform(this.Image, p);
	//	std::cout << "TODO!:" << "IMplement GABOR" << std::endl;
	//	exit(1);	
	
	GaborImage *gab = new GaborImage();
	Img retImg = gab->GaborTransform(Image, p);
	delete gab;
	return new GpImage(retImg);
}

GpImage* GpImage::ShiftDown() const {
//		Value = this.Image.Convolution(ShiftDownKernel);
	Img retImg = (IplImage*) cvClone(Image);	
	
	float down[] = { 0, 1, 0 , 0, 0, 0 , 0, 0, 0 };	
	CvMat* ShiftDownKernel = cvCreateMat(3, 3, CV_32F);;
	cvSetData(ShiftDownKernel, down, 3*sizeof(float));
	cvFilter2D(Image, retImg, ShiftDownKernel);
	
	cvReleaseMat(&ShiftDownKernel);	

	return new GpImage(retImg);
}
GpImage* GpImage::ShiftUp() const {
	Img retImg = (IplImage*) cvClone(Image);	

	float up[] = { 0, 0, 0 , 0, 0, 0 , 0, 1, 0 };
	CvMat* ShiftUpKernel = cvCreateMat(3, 3, CV_32F);
	cvSetData(ShiftUpKernel, up, 3*sizeof(float));		
	cvFilter2D(Image, retImg, ShiftUpKernel);
	
	cvReleaseMat(&ShiftUpKernel);

	return new GpImage(retImg);
}
GpImage* GpImage::ShiftRight() const {
	Img retImg = (IplImage*) cvClone(Image);	
	
	float left[] = { 0, 0, 0 , 1, 0, 0 , 0, 0, 0 };
	CvMat* ShiftLeftKernel = cvCreateMat(3, 3, CV_32F);;
	cvSetData(ShiftLeftKernel,  left, 3*sizeof(float));
	cvFilter2D(Image, retImg, ShiftLeftKernel);
	
	cvReleaseMat(&ShiftLeftKernel);
	
	return new GpImage(retImg);
}
GpImage* GpImage::ShiftLeft() const {
	Img retImg = (IplImage*) cvClone(Image);	
	
	float right[]= { 0, 0, 0 , 0, 0, 1 , 0, 0, 0 };
	CvMat* ShiftRightKernel = cvCreateMat(3, 3, CV_32F);;	
	cvSetData(ShiftRightKernel, right, 3*sizeof(float));
	cvFilter2D(Image, retImg, ShiftRightKernel);
	
	cvReleaseMat(&ShiftRightKernel);
	
	return new GpImage(retImg);
}


GpImage* GpImage::Normalize() const {
	Img retImg = (IplImage*) cvClone(Image);	
	
	double min, max;
	CvPoint min_loc, max_loc;
	Img temp  = (IplImage*) cvClone(Image);		
	Img unity = (IplImage*) cvClone(Image);

	//	this.Image.MinMax(out MinValues, out MaxValues, out MinPoints, out MaxPoints);
	cvMinMaxLoc(Image, &min, &max, &min_loc, &max_loc);

	//	Value = this.Image - MinValues[0];
	cvSubS(Image, cvRealScalar(min), temp);

	//		Value._Mul(255d / (MaxValues[0] - MinValues[0]));
	cvSet(unity, cvRealScalar(1.0));
	cvMul(temp, unity, retImg, 255 / (max - min));
	
	cvReleaseImage(&unity);
	cvReleaseImage(&temp);
	return new GpImage(retImg);
}

GpImage* GpImage::ResizeThenGabor(int Orientation, double Scale) const {
	GaborImage *gab = new GaborImage();
	
	Img retImg = (IplImage*) cvClone(Image);
	Img temp   = cvCreateImage(cvSize(Image->width*Scale, Image->height*Scale), IPL_DEPTH_32F, 1);
	cvResize(Image, temp, CV_INTER_CUBIC);

	Img temp2 = gab->GaborTransform(temp, Orientation);
	
	cvResize(temp2, retImg, CV_INTER_CUBIC);
	
	delete gab;
	cvReleaseImage(&temp);
	cvReleaseImage(&temp2);	
	
	return new GpImage(retImg);

}
	
	//		GaborEmguImg gab = new GaborEmguImg();
	
//			
//			int H = (int)(this.Height * Scale);
//			int W = (int)(this.Width * Scale);
//			
//			Img t = this.Image.Resize(W, H, Emgu.CV.CvEnum.INTER.CV_INTER_CUBIC);

	//			t = t.Resize(this.Width, this.Height, Emgu.CV.CvEnum.INTER.CV_INTER_CUBIC);
//			
//			Value = gab.GaborTransform(this.Image, Orientation);
//			this.Cache.Add(Key, Value);
//			           }
//	           return new GpImage(Value);
//	       }



void GpImage::Save(string FileName) const {
	cvSaveImage(FileName.c_str(), Image);
//	Image->Save(FileName);
}
