// Copyright: (C) 2011-2013 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "icImage.h"
//#include "GaborImage.h"

icImage* icImage::equalizeHistogram() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvEqualizeHist(Image, retImg);
	return new icImage(retImg);
}


icImage* icImage::add (icImage* a) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvAdd(Image, a->Image, retImg);
	return new icImage(retImg);
}
icImage* icImage::sub (icImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvSub(Image, a->Image, retImg);
	return new icImage(retImg);
}
icImage* icImage::mul (icImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvMul(Image, a->Image, retImg);
	return new icImage(retImg);
}
icImage* icImage::avg (icImage* a) const { 
//icImage* icImage::avg (icImage a) { Img Temp = this.Image + a.Image; Temp._Mul(0.5); return new icImage(Temp); }
	Img retImg = (IplImage*) cvClone(Image);
	Img unity  = (IplImage*) cvClone(Image);

	// adding two pictures together
	cvAdd(Image, a->Image, retImg);
	
	// multiply with 0.5
	cvSet(unity, cvRealScalar(1.0));
	cvMul(retImg, unity, retImg, 0.5);

	cvReleaseImage(&unity);
	
	return new icImage(retImg);
}

// set full image to average of it all
icImage* icImage::Avg(void) const {
	Img retImg = (IplImage*) cvClone(Image);
    cvSet(retImg, cvRealScalar(cvAvg(Image).val[0]));
	return new icImage(retImg);
}

icImage* icImage::mulc(double v) const { 
	Img retImg = (IplImage*) cvClone(Image);
	Img unity  = (IplImage*) cvClone(Image);
	cvSet(unity, cvRealScalar(1.0));
	cvMul(Image, unity, retImg, v);
	cvReleaseImage(&unity);
	return new icImage(retImg);
}

icImage* icImage::addc(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvAddS(Image, cvRealScalar(v), retImg);
	return new icImage(retImg);
}
icImage* icImage::subc(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvSubS(Image, cvRealScalar(v), retImg);
	return new icImage(retImg);	
}
icImage* icImage::absdiff(icImage* a) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvAbsDiff(Image, a->Image, retImg);
	return new icImage(retImg);	
}

icImage* icImage::canny(double v) const {
	//	Value = this.Image.Canny(new Gray(v), new Gray(10));
	Img retImg = (IplImage*) cvClone(Image);
    Img gtempIn   = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 1);
    Img gtempOut  = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 1);
    cvScale(Image, gtempIn);
	cvCanny(gtempIn, gtempOut, v, 10);
    cvScale(gtempOut, retImg);
    cvReleaseImage(&gtempIn);
    cvReleaseImage(&gtempOut);
	return new icImage(retImg);
}

icImage* icImage::dilate(int Iterations) const {
			// Value = this.Image.Dilate(Iterations);
	Img retImg = (IplImage*) cvClone(Image);
	cvDilate(Image, retImg, NULL, Iterations);
	return new icImage(retImg);
}

icImage* icImage::erode(int Iterations) const {
//	Value = this.Image.Erode(Iterations);
	Img retImg = (IplImage*) cvClone(Image);
	cvErode(Image, retImg, NULL, Iterations);
	return new icImage(retImg);
}

icImage* icImage::laplace(int Aperture) const {
	//	Value = this.Image.Laplace(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvLaplace(Image, retImg, Aperture);
	return new icImage(retImg);	
}

icImage* icImage::log() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvLog(Image, retImg);
	return new icImage(retImg);	
}

icImage* icImage::pow(double P) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvPow(Image, retImg, P);
	return new icImage(retImg);	
}


icImage* icImage::exp() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvExp(Image, retImg);
	return new icImage(retImg);
}

icImage* icImage::sqrt() const {
	Img retImg = (IplImage*) cvClone(Image);
	cvPow(Image, retImg, 0.5);
	return new icImage(retImg);	
}


icImage* icImage::Max(void) const {
	Img retImg = (IplImage*) cvClone(Image);
    double min, max = 0;
    cvMinMaxLoc(Image, &min, &max);
    cvSet(retImg, cvRealScalar(max));
	return new icImage(retImg);
}

icImage* icImage::max(icImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvMax(Image, a->Image, retImg);
	return new icImage(retImg);	
}

icImage* icImage::max(double val) const {
	Img retImg = (IplImage*) cvClone(Image);
    Img constant = (IplImage*) cvClone(Image);
	cvSet(constant, cvRealScalar(val));
	cvMax(Image, constant, retImg);
    cvReleaseImage(&constant);
	return new icImage(retImg);
}


icImage* icImage::Min(void) const {
	Img retImg = (IplImage*) cvClone(Image);
    double min = 0, max;
    cvMinMaxLoc(Image, &min, &max);
    cvSet(retImg, cvRealScalar(min));
	return new icImage(retImg);
}

icImage* icImage::Min(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
    cvSet(retImg, cvRealScalar(v));
	cvMin(Image, retImg, retImg);
	return new icImage(retImg);
}

icImage* icImage::min(icImage* a) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvMin(Image, a->Image, retImg);
	return new icImage(retImg);	
}
	
icImage* icImage::gauss(int x, int y) const {
    //		Value = this.Image.SmoothGaussian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_GAUSSIAN, x, y);
	return new icImage(retImg);
}

icImage* icImage::gauss(int Aperture) const {
//		Value = this.Image.SmoothGaussian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_GAUSSIAN, Aperture, Aperture);
	return new icImage(retImg);	
}
icImage* icImage::sobelx(int Aperture) const {
//		Value = this.Image.Sobel(1, 0, Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSobel(Image, retImg, 1, 0, Aperture);
	return new icImage(retImg);	
}
icImage* icImage::sobely(int Aperture) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvSobel(Image, retImg, 0, 1, Aperture);
	return new icImage(retImg);	
}


icImage* icImage::unsharpen(int x, int y) const {
    //		Img blurred = this.Image.SmoothGaussian(Apertrue);
    //		Img unsharpenmask = this.Image.Sub(blurred);
    //		Img hicontrast = unsharpenmask.Mul(1.5);/
    //		Value = this.Image.Add(hicontrast);
	Img retImg = (IplImage*) cvClone(Image);
    
	Img blurred = (IplImage*) cvClone(Image);
	Img unsharpenmask = (IplImage*) cvClone(Image);
	Img hicontrast = (IplImage*) cvClone(Image);
	Img cv1 = (IplImage*) cvClone(Image);
	
	cvSmooth(Image, blurred, CV_GAUSSIAN, x, y);
	cvSub(Image, blurred, unsharpenmask);
	cvSet(cv1, cvRealScalar(1.0));
	cvMul(unsharpenmask, cv1, hicontrast, 1.5);
	
	cvAdd(Image, hicontrast, retImg);
    
	cvReleaseImage(&blurred);
	cvReleaseImage(&unsharpenmask);
	cvReleaseImage(&hicontrast);
	cvReleaseImage(&cv1);
	
	return new icImage(retImg);
}

icImage* icImage::unsharpen(int Aperture) const {
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
	
	return new icImage(retImg);	
}
icImage* icImage::threshold(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvThreshold(Image, retImg, v, 255.0, CV_THRESH_BINARY);
	return new icImage(retImg);		
}

icImage* icImage::thresholdInv(double v) const {
	Img retImg = (IplImage*) cvClone(Image);
	cvThreshold(Image, retImg, v, 255.0, CV_THRESH_BINARY_INV);
	return new icImage(retImg);		
}


icImage* icImage::SmoothMedian(int Aperture) const { 
//		Value = this.Image.SmoothMedian(Apertrue);
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_MEDIAN, Aperture);
	return new icImage(retImg);			
}

icImage* icImage::SmoothBilateral(int Aperture) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_BILATERAL, Aperture, 10, 10);
	return new icImage(retImg);			
}

icImage* icImage::SmoothBlur(int Aperture) const { 
	Img retImg = (IplImage*) cvClone(Image);
	cvSmooth(Image, retImg, CV_BLUR, Aperture, Aperture);
	return new icImage(retImg);			
}


//icImage* icImage::gabor(int width, int height, int frequ, int orientation) const {
//	GaborImage *gab = new GaborImage(abs(width), abs(height));
//	Img retImg = gab->GaborTransform(Image, frequ, orientation);
//	delete gab;
//	return new icImage(retImg);
//}
//
//icImage* icImage::gabor(int frequ, int orientation) const {
//	GaborImage *gab = new GaborImage();
//	Img retImg = gab->GaborTransform(Image, frequ, orientation);
//	delete gab;
//	return new icImage(retImg);
//}



icImage* icImage::Shift(int Xshift, int Yshift) const {
	Img retImg = (IplImage*) cvClone(Image);

    int w = m_width;
    int h = m_height;
    
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            int x2 = abs(x + Xshift) % w;
            int y2 = abs(y + Yshift) % h;
			CV_IMAGE_ELEM(retImg, float, y, x) = CV_IMAGE_ELEM(Image, float, y2, x2);
        }
    
    return new icImage(retImg);
}


icImage* icImage::ShiftDown() const {
	Img retImg = (IplImage*) cvClone(Image);
	
	float down[] = { 0, 1, 0 , 0, 0, 0 , 0, 0, 0 };
	CvMat* ShiftDownKernel = cvCreateMat(3, 3, CV_32F);;
	cvSetData(ShiftDownKernel, down, 3*sizeof(float));
	cvFilter2D(Image, retImg, ShiftDownKernel);
	
	cvReleaseMat(&ShiftDownKernel);	

	return new icImage(retImg);
}
icImage* icImage::ShiftUp() const {
	Img retImg = (IplImage*) cvClone(Image);	

	float up[] = { 0, 0, 0 , 0, 0, 0 , 0, 1, 0 };
	CvMat* ShiftUpKernel = cvCreateMat(3, 3, CV_32F);
	cvSetData(ShiftUpKernel, up, 3*sizeof(float));		
	cvFilter2D(Image, retImg, ShiftUpKernel);
	
	cvReleaseMat(&ShiftUpKernel);

	return new icImage(retImg);
}
icImage* icImage::ShiftRight() const {
	Img retImg = (IplImage*) cvClone(Image);	
	
	//	float right[]= { 0, 0, 0 , 0, 0, 1 , 0, 0, 0 };
		float right[]= { 0, 1, 0 , 0, 0, 0 , 0, 0, 0 };		// actually down(simon!?)
	CvMat* ShiftRightKernel = cvCreateMat(3, 3, CV_32F);;	
	cvSetData(ShiftRightKernel, right, 3 * sizeof(float));
	cvFilter2D(Image, retImg, ShiftRightKernel);
	
	cvReleaseMat(&ShiftRightKernel);
	
	return new icImage(retImg);
}
icImage* icImage::ShiftLeft() const {
	Img retImg = (IplImage*) cvClone(Image);	
	
	float left[] = { 0, 0, 0 , 1, 0, 0 , 0, 0, 0 };
	CvMat* ShiftLeftKernel = cvCreateMat(3, 3, CV_32F);;
	cvSetData(ShiftLeftKernel,  left, 3*sizeof(float));
	cvFilter2D(Image, retImg, ShiftLeftKernel);
	
	cvReleaseMat(&ShiftLeftKernel);
	
	return new icImage(retImg);
}


icImage* icImage::Normalize() const {
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
	return new icImage(retImg);
}

//icImage* icImage::ResizeThenGabor(int Frequency, int Orientation, double Scale) const {
//	GaborImage *gab = new GaborImage();
//	
//	Img retImg = (IplImage*) cvClone(Image);
//	Img temp   = cvCreateImage(cvSize(Image->width*Scale, Image->height*Scale), IPL_DEPTH_32F, 1);
//	cvResize(Image, temp, CV_INTER_CUBIC);
//
//	Img temp2 = gab->GaborTransform(temp, Frequency, Orientation);
//	
//	cvResize(temp2, retImg, CV_INTER_CUBIC);
//	
//	delete gab;
//	cvReleaseImage(&temp);
//	cvReleaseImage(&temp2);	
//	
//	return new icImage(retImg);
//
//}

icImage* icImage::ReScale(double S) const {
	if (S >= 1) {
		return Clone();
	}
	
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
	
	return new icImage(retImg);
}

icImage* icImage::LocalMax(int Aperture) const {
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
					CV_IMAGE_ELEM(Image, float, y+j, x+i) = Acc;			
			//					CV_MAT_ELEM(retImg, float, x + i, y + j) = Acc;
		}
	}
	return new icImage(retImg);
}

icImage* icImage::LocalMin(int Aperture) const {
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
					CV_IMAGE_ELEM(Image, float, y+j, x+i) = Acc;
			//					CV_MAT_ELEM(retImg, float, x + i, y + j) = Acc;
		}
	}
	return new icImage(retImg);
}

icImage* icImage::LocalAvg(int Aperture) const {
	int width = Image->width;
	int height = Image->height;
	
	Img retImg = (IplImage*) cvClone(Image);
	
	for (int x = Aperture; x < width - Aperture; x++) {
		for (int y = Aperture; y < height - Aperture; y++) {
			float Acc = 0;
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++) {
					Acc += CV_IMAGE_ELEM(Image, float, y+j, x+i);
				}
			
            Acc = Acc/((float)Aperture*Aperture);
			for (int i = -Aperture; i <= Aperture; i++)
				for (int j = -Aperture; j <= Aperture; j++)
					//					*((float*)(retImg->imageData + retImg->widthStep*(y + j))[x + i]) = Acc;
					CV_IMAGE_ELEM(Image, float, y+j, x+i) = Acc;
			//					CV_MAT_ELEM(retImg, float, x + i, y + j) = Acc;
		}
	}
	return new icImage(retImg);
}




void icImage::Save(std::string FileName) const {
	cvSaveImage(FileName.c_str(), Image);
}
