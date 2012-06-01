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

GpImage* GpImage::exp() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvExp(Image, retImg);
	return new GpImage(retImg);	
}

GpImage* GpImage::sqrt() const {
	return pow(0.5);
}

GpImage* GpImage::log() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvLog(Image, retImg);
	return new GpImage(retImg);	
}

GpImage* GpImage::pow(double P) const {
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


GpImage* GpImage::min() const {
	double MinValue, MaxValue;
	cvMinMaxLoc(Image, &MinValue, &MaxValue, NULL, NULL);
	
	Img retImg = (IplImage*) cvClone(Image);
	cvZero(retImg);
	cvAddS(retImg, cvRealScalar(MinValue), retImg);
	return new GpImage(retImg);
}

GpImage* GpImage::min (float v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvMinS(Image, v, retImg);
	return new GpImage(retImg);	
}

GpImage* GpImage::max() const {
	double MinValue, MaxValue;
	cvMinMaxLoc(Image, &MinValue, &MaxValue, NULL, NULL);

	Img retImg = (IplImage*) cvClone(Image);
	cvZero(retImg);
	cvAddS(retImg, cvRealScalar(MaxValue), retImg);
	return new GpImage(retImg);
}

GpImage* GpImage::max (float v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvMaxS(Image, v, retImg);
	return new GpImage(retImg);	
}


double GpImage::getSum() const {
	return cvSum(Image).val[0];
}


GpImage* GpImage::avg() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvZero(retImg);
	double v = getSum() / (Image->width * Image->height);
	cvAddS(retImg, cvRealScalar(v), retImg);
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

GpImage* GpImage::thresholdInv(double v) const {
	//		Value = this.Image.ThresholdBinary(new Gray(v), new Gray(255));
	Img retImg = (IplImage*) cvClone(Image);
	cvThreshold(Image, retImg, v, 255.0, CV_THRESH_BINARY_INV);
	return new GpImage(retImg);		
}


GpImage* GpImage::SmoothMedian(int Aperture) const { 
//		Value = this.Image.SmoothMedian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_MEDIAN, Aperture);
	return new GpImage(retImg);			
}

GpImage* GpImage::SmoothBilateral(int Aperture) const { 
	//		Value = this.Image.SmoothMedian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_BILATERAL, Aperture, 10, 10);
	return new GpImage(retImg);			
}

GpImage* GpImage::SmoothBlur(int Aperture) const { 
	//		Value = this.Image.SmoothMedian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_BLUR, Aperture, Aperture);
	return new GpImage(retImg);			
}

GpImage* GpImage::gabor(int frequ, int orientation) const {
	//GaborEmguImg gab = new GaborEmguImg();
//		Value = gab.GaborTransform(this.Image, p);
	//	std::cout << "TODO!:" << "IMplement GABOR" << std::endl;
	//	exit(1);	
	
	GaborImage *gab = new GaborImage();
	Img retImg = gab->GaborTransform(Image, frequ, orientation);
	delete gab;
	return new GpImage(retImg);
}


GpImage* GpImage::Shift(int XShift, int YShift) const {
	int width = Image->width;
	int height = Image->height;
	
	Img retImg = (IplImage*) cvClone(Image);	
	
	for (int x = 0; x < width ; x++) {
		for (int y = 0; y < height; y++) {
			int x2 = abs(x + XShift) % width;
			int y2 = abs(y + YShift) % height;
//			Value.Data[y, x, 0] = this.Image.Data[y2, x2, 0];
			CV_IMAGE_ELEM(retImg, float, y, x) = CV_IMAGE_ELEM(Image, float, y2, x2);
		}
	}
	return new GpImage(retImg);
}


GpImage* GpImage::ShiftDown() const {
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
	
	//	float right[]= { 0, 0, 0 , 0, 0, 1 , 0, 0, 0 };
		float right[]= { 0, 1, 0 , 0, 0, 0 , 0, 0, 0 };		// actually down(simon!?)
	CvMat* ShiftRightKernel = cvCreateMat(3, 3, CV_32F);;	
	cvSetData(ShiftRightKernel, right, 3 * sizeof(float));
	cvFilter2D(Image, retImg, ShiftRightKernel);
	
	cvReleaseMat(&ShiftRightKernel);
	
	return new GpImage(retImg);
}
GpImage* GpImage::ShiftLeft() const {
	Img retImg = (IplImage*) cvClone(Image);	
	
	float left[] = { 0, 0, 0 , 1, 0, 0 , 0, 0, 0 };
	CvMat* ShiftLeftKernel = cvCreateMat(3, 3, CV_32F);;
	cvSetData(ShiftLeftKernel,  left, 3*sizeof(float));
	cvFilter2D(Image, retImg, ShiftLeftKernel);
	
	cvReleaseMat(&ShiftLeftKernel);
	
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

GpImage* GpImage::ResizeThenGabor(int Frequency, int Orientation, double Scale) const {
	GaborImage *gab = new GaborImage();
	
	Img retImg = (IplImage*) cvClone(Image);
	Img temp   = cvCreateImage(cvSize(Image->width*Scale, Image->height*Scale), IPL_DEPTH_32F, 1);
	cvResize(Image, temp, CV_INTER_CUBIC);

	Img temp2 = gab->GaborTransform(temp, Frequency, Orientation);
	
	cvResize(temp2, retImg, CV_INTER_CUBIC);
	
	delete gab;
	cvReleaseImage(&temp);
	cvReleaseImage(&temp2);	
	
	return new GpImage(retImg);

}


GpImage* GpImage::ReScale(double S) const {
//	if (S >= 1) {
//		return new GpImage(this.Image.Clone());
//	}
	
	Img retImg = (IplImage*) cvClone(Image);	

	int height = (int)(Image->height * S);
	int width = (int)(Image->width * S);
	if (height < Image->height / 4) height = Image->height / 4;
	if (width  < Image->width / 4)  width  = Image->width / 4;
	if (height > Image->height * 2) height = Image->height;
	if (width  > Image->width * 2)  width  = Image->width;

	Img temp   = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	cvResize(Image, temp, CV_INTER_CUBIC);
	cvResize(temp, retImg, CV_INTER_CUBIC);
	
	cvReleaseImage(&temp);

	return new GpImage(retImg);
}


GpImage* GpImage::LocalMax(int Aperture) const {
	int width = Image->width;
	int height = Image->height;

	Img retImg = (IplImage*) cvClone(Image);	

	for (int x = Aperture; x < width - Aperture; x++) {
		for (int y = Aperture; y < height - Aperture; y++) {
			float Acc = FLT_MIN;
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++) {
//					uchar* temp_ptr = &((uchar*)(img->imageData + img->widthStep*pt.y))[pt.x*3];
					Acc = std::max(Acc, CV_IMAGE_ELEM(Image, float, y+j, x+i));
//					Acc = std::max(Acc, *((float*)(Image->imageData + Image->widthStep*(y + j))[x + i]));
				}
					
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++)
//					*((float*)(retImg->imageData + retImg->widthStep*(y + j))[x + i]) = Acc;
					CV_IMAGE_ELEM(retImg, float, y+j, x+i) = Acc;			
//					CV_MAT_ELEM(retImg, float, x + i, y + j) = Acc;
		}
	}
	return new GpImage(retImg);
}

GpImage* GpImage::LocalAvg(int Aperture) const {
	int width = Image->width;
	int height = Image->height;
	
	Img retImg = (IplImage*) cvClone(Image);	
	
	for (int x = Aperture; x < width - Aperture; x++) {
		for (int y = Aperture; y < height - Aperture; y++) {
			float Acc = 0;
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++) 
					Acc += CV_IMAGE_ELEM(Image, float, y+j, x+i);
			
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++)
					CV_IMAGE_ELEM(retImg, float, y+j, x+i) = Acc / (((Aperture * 2) + 1) * ((Aperture * 2) + 1));			
		}
	}
	return new GpImage(retImg);
}

GpImage* GpImage::LocalMin(int Aperture) const {
	int width = Image->width;
	int height = Image->height;
	
	Img retImg = (IplImage*) cvClone(Image);	
	
	for (int x = Aperture; x < width - Aperture; x++) {
		for (int y = Aperture; y < height - Aperture; y++) {
			float Acc = FLT_MAX;
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++) {
					//					uchar* temp_ptr = &((uchar*)(img->imageData + img->widthStep*pt.y))[pt.x*3];
					Acc = std::min(Acc, CV_IMAGE_ELEM(Image, float, y+j, x+i));
					//					Acc = std::max(Acc, *((float*)(Image->imageData + Image->widthStep*(y + j))[x + i]));
				}
			
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++)
					//					*((float*)(retImg->imageData + retImg->widthStep*(y + j))[x + i]) = Acc;
					CV_IMAGE_ELEM(retImg, float, y+j, x+i) = Acc;			
			//					CV_MAT_ELEM(retImg, float, x + i, y + j) = Acc;
		}
	}
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



void GpImage::Save(std::string FileName) const {
	cvSaveImage(FileName.c_str(), Image);
//	Image->Save(FileName);
}
