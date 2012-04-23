/* DisparityMapper.cpp
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-12
 * Class declaration file for the DepthTestDisplay class.
 * This class is the encapsulation for a visualization of the various steps of the algorithm
 */
#include "StdAfx.h"
#include "DepthTestDisplay.h"

DepthTestDisplay::DepthTestDisplay(void)
{
	m_rightImage = m_leftImage = m_maxDisparityImage = m_horizontalDisparityMap = m_verticalDisparityMap = NULL;
	m_warpImages = m_diffImages = m_likelihoodImages = m_filteredLikelihoodImages = NULL;
	m_nImageCount = m_nDisplayImageIndex = 0;
	m_nImageSize = 210;
	m_nShowImages = 0x01 | 0x02 | 0x04; /* | 0x08 | 0x10 // set bitmask*/
	m_mainWindowName = "Main Window";
	// get desktop sizes (maybe later?)
	int width = 800, height = 950;
	cvNamedWindow(m_mainWindowName.c_str(), 0);
	cvResizeWindow(m_mainWindowName.c_str(), width, height);

	m_dispImage = cvCreateImage( cvSize(width, height), 8, 3 );
	cvShowImage(m_mainWindowName.c_str(), m_dispImage);
}

void DepthTestDisplay::setLeftImage(IplImage *leftImage) {
	if(m_leftImage) cvReleaseImage(&m_leftImage);
	m_leftImage = cvCloneImage(leftImage);	
	//updateDisplay();
}

void DepthTestDisplay::setRightImage(IplImage *rightImage) {
	if(m_rightImage) cvReleaseImage(&m_rightImage);
	m_rightImage = cvCloneImage(rightImage);
	//updateDisplay();
}

void DepthTestDisplay::setDiffImage(IplImage *img) {
	IplImage **images = new IplImage*[1];
	images[0] = img;
	setDiffImages(images, 1);
}

void DepthTestDisplay::setDiffImages(IplImage **images, int cnt) {
	if(m_nImageCount > 0) {
		// release all pictures
		if(m_diffImages)
			for(int i = 0;i < m_nImageCount;i++)
				if(m_diffImages[i]) cvReleaseImage(&m_diffImages[i]);
	}
	// load all pictures
	m_nImageCount = cnt;
	m_nDisplayImageIndex = std::max((int)cnt/2-1, 0);
	m_diffImages = new IplImage*[m_nImageCount];
	for(int i = 0;i < m_nImageCount;i++)
		m_diffImages[i] = cvCloneImage(images[i]);
	//updateDisplay();
}

void DepthTestDisplay::setWarpImage(IplImage *img) {
	IplImage **images = new IplImage*[1];
	images[0] = img;
	setWarpImages(images, 1);
}

void DepthTestDisplay::setWarpImages(IplImage **images, int cnt) {
	if(m_nImageCount > 0) {
		// release all pictures
		if(m_warpImages)
			for(int i = 0;i < m_nImageCount;i++)
				if(m_warpImages[i]) cvReleaseImage(&m_warpImages[i]);
	}
	// load all pictures
	m_nImageCount = cnt;
	m_warpImages = new IplImage*[m_nImageCount];
	for(int i = 0;i < m_nImageCount;i++)
		m_warpImages[i] = cvCloneImage(images[i]);
	//updateDisplay();
}

void DepthTestDisplay::setLikelihoodImage(IplImage *img) {
	IplImage **images = new IplImage*[1];
	images[0] = img;
	setLikelihoodImages(images, 1);
}

void DepthTestDisplay::setLikelihoodImages(IplImage **images, int cnt) {
	if(m_nImageCount > 0) {
		// release all pictures
		if(m_likelihoodImages)
			for(int i = 0;i < m_nImageCount;i++)
				if(m_likelihoodImages[i]) cvReleaseImage(&m_likelihoodImages[i]);
	}
	// load all pictures
	m_nImageCount = cnt;
	m_likelihoodImages = new IplImage*[m_nImageCount];
	for(int i = 0;i < m_nImageCount;i++)
		m_likelihoodImages[i] = cvCloneImage(images[i]);
	//updateDisplay();
}

void DepthTestDisplay::setFilteredLikelihoodImage(IplImage *img) {
	IplImage **images = new IplImage*[1];
	images[0] = img;
	setFilteredLikelihoodImages(images, 1);
}

void DepthTestDisplay::setFilteredLikelihoodImages(IplImage **images, int cnt) {
	if(m_nImageCount > 0) {
		// release all pictures
		if(m_filteredLikelihoodImages)
			for(int i = 0;i < m_nImageCount;i++)
				if(m_filteredLikelihoodImages[i]) cvReleaseImage(&m_filteredLikelihoodImages[i]);
	}
	// load all pictures
	m_nImageCount = cnt;
	m_filteredLikelihoodImages = new IplImage*[m_nImageCount];
	for(int i = 0;i < m_nImageCount;i++)
		m_filteredLikelihoodImages[i] = cvCloneImage(images[i]);
	//updateDisplay();
}

void DepthTestDisplay::setMaxDisparityImage(IplImage *image) {
	m_maxDisparityImage = cvCloneImage(image);
	//updateDisplay();
}

void DepthTestDisplay::setHorizontalDisparityMap(IplImage *image) {
	m_horizontalDisparityMap = cvCloneImage(image);
}

void DepthTestDisplay::setVerticalDisparityMap(IplImage *image) {
	m_verticalDisparityMap = cvCloneImage(image);
}

void DepthTestDisplay::updateDisplay(void){
	int horizImages = 41, vertImages = 7, stepWidth = 2;
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.75, 0.75, 0, 1);
	cvSetZero(m_dispImage);
	IplImage *tempImage = cvCreateImage( cvSize(m_rightImage->width, m_rightImage->height), 8, 3 );
	// display the two stereo images
	if(m_leftImage) {
		cvSetImageROI(m_dispImage, cvRect(100, 20, m_nImageSize, m_nImageSize));
		cvConvertImage(m_leftImage, tempImage);
		cvResize(tempImage, m_dispImage);
		cvResetImageROI(m_dispImage);
		cvPutText(m_dispImage, "Left Image", cvPoint(55+m_nImageSize/2, 35+m_nImageSize), &font, cvScalar(255,255,255));
	}
	if(m_rightImage) {
		cvSetImageROI(m_dispImage, cvRect(m_dispImage->width - m_nImageSize - 100, 20, m_nImageSize, m_nImageSize));
		cvConvertImage(m_rightImage, tempImage);
		cvResize(tempImage, m_dispImage);
		cvResetImageROI(m_dispImage);
		cvPutText(m_dispImage, "Right Image", cvPoint((m_dispImage->width - m_nImageSize - 100)+m_nImageSize/2-40, 35+m_nImageSize), &font, cvScalar(255,255,255));
	}

	cvInitFont(&font,CV_FONT_HERSHEY_PLAIN, 0.65, 0.65, 0, 1);
	int gap = (m_dispImage->width - 3*m_nImageSize)/4;
	if(m_nImageCount > 0) {
		// find out the shifted pixel data :)
		int l = -(int)vertImages/2;
		int endk = (int)horizImages/2;
		int k = -endk;
		for(int i = 0;i < m_nDisplayImageIndex;i++)
			if(++k > endk) {
				k = -endk;
				l++;
			}

		int coY = 0, coX = 0;
		for(int i = 0;i < std::min(3, m_nImageCount);i++) {
			if(k > endk) {
				k = -endk;
				l++;
			}
#ifdef _DEBUG
			printf("#%d: k/l: %d/%d\r", i, k, l);
#endif
			coY = 40 + (m_nImageSize + 20);
			
			if(m_nImageCount < 3)
				if(!i) coX = 100;
				else coX = m_dispImage->width - m_nImageSize - 100;
			else
				coX += gap;

			// put text
			char help[20];			
			sprintf(help, "%2d/%2d", k*stepWidth, l*stepWidth);
			cvPutText(m_dispImage, help, cvPoint(coX + 5, coY - 5), &font, cvScalar(255,255,255));

			if(m_nShowImages & 1 && m_warpImages && m_warpImages[m_nDisplayImageIndex+i]) {
				// draw warped images
				cvSetImageROI(m_dispImage, cvRect(coX, coY, m_nImageSize, m_nImageSize));
				cvConvertImage(m_warpImages[m_nDisplayImageIndex+i], tempImage);
				cvResize(tempImage, m_dispImage);
				cvResetImageROI(m_dispImage);
				// draw ouline
				cvRectangle(m_dispImage, cvPoint(coX, coY), cvPoint(coX + m_nImageSize, coY + m_nImageSize), cvScalar(255,0,0), 1);
				// put text
				cvPutText(m_dispImage, "Warped", cvPoint(3, coY - 5), &font, cvScalar(255,255,255));

				// next image offset
				coY += (m_nImageSize + 20);
			}

			//coY = 40 + (m_nImageSize + 20) * 2;
			if(m_nShowImages & 2 && m_diffImages && m_diffImages[m_nDisplayImageIndex+i]) {
				// draw difference images
				cvSetImageROI(m_dispImage, cvRect(coX, coY, m_nImageSize, m_nImageSize));
				cvConvertImage(m_diffImages[m_nDisplayImageIndex+i], tempImage);
				cvResize(tempImage, m_dispImage);
				cvResetImageROI(m_dispImage);
				// draw ouline
				cvRectangle(m_dispImage, cvPoint(coX, coY), cvPoint(coX + m_nImageSize, coY + m_nImageSize), cvScalar(0,255,0), 1);
				// put text
				cvPutText(m_dispImage, "Diff", cvPoint(3, coY - 5), &font, cvScalar(255,255,255));

				// next image offset
				coY += (m_nImageSize + 20);
			}

			if(m_nShowImages & 4 && m_likelihoodImages && m_likelihoodImages[m_nDisplayImageIndex+i]) {
				// draw likelihood images
				cvSetImageROI(m_dispImage, cvRect(coX, coY, m_nImageSize, m_nImageSize));
				cvConvertImage(m_likelihoodImages[m_nDisplayImageIndex+i], tempImage);
				cvResize(tempImage, m_dispImage);
				cvResetImageROI(m_dispImage);
				// draw ouline
				cvRectangle(m_dispImage, cvPoint(coX, coY), cvPoint(coX + m_nImageSize, coY + m_nImageSize), cvScalar(0,0,255), 1);
				// put text
				cvPutText(m_dispImage, "Likelihood", cvPoint(3, coY - 5), &font, cvScalar(255,255,255));

				// next image offset
				coY += (m_nImageSize + 20);
			}

			if(m_nShowImages & 8 && m_filteredLikelihoodImages && m_filteredLikelihoodImages[m_nDisplayImageIndex+i]) {
				// draw likelihood images
				cvSetImageROI(m_dispImage, cvRect(coX, coY, m_nImageSize, m_nImageSize));
				cvConvertImage(m_filteredLikelihoodImages[m_nDisplayImageIndex+i], tempImage);
				cvResize(tempImage, m_dispImage);
				cvResetImageROI(m_dispImage);
				// draw ouline
				cvRectangle(m_dispImage, cvPoint(coX, coY), cvPoint(coX + m_nImageSize, coY + m_nImageSize), cvScalar(120,120,120), 1);
				// put text
				cvPutText(m_dispImage, "Filtered Likelihood", cvPoint(3, coY - 5), &font, cvScalar(255,255,255));

				// next image offset
				coY += (m_nImageSize + 20);
			}

			if(m_nShowImages & 16 && ((i==0 && m_maxDisparityImage) || (i==1 && m_horizontalDisparityMap) || (i==2 && m_verticalDisparityMap))) {
				// draw max disparity map 
				cvSetImageROI(m_dispImage, cvRect(coX, coY, m_nImageSize, m_nImageSize));
				if(i == 0) cvConvertImage(m_maxDisparityImage, tempImage);
				if(i == 1) cvConvertImage(m_horizontalDisparityMap, tempImage);
				if(i == 2) cvConvertImage(m_verticalDisparityMap, tempImage);
				cvResize(tempImage, m_dispImage);
				cvResetImageROI(m_dispImage);
				// draw ouline
				cvRectangle(m_dispImage, cvPoint(coX, coY), cvPoint(coX + m_nImageSize, coY + m_nImageSize), cvScalar(120,120,120), 1);
				// put text
				if(i == 0) cvPutText(m_dispImage, "Max Activation Map", cvPoint(coX + 5, coY - 5), &font, cvScalar(255,255,255));
				if(i == 1) cvPutText(m_dispImage, "Horizontal Disparity Map", cvPoint(coX + 5, coY - 5), &font, cvScalar(255,255,255));
				if(i == 2) cvPutText(m_dispImage, "Vertical Disparity Map", cvPoint(coX + 5, coY - 5), &font, cvScalar(255,255,255));
				// next image offset
				coY += (m_nImageSize + 20);
			}
			coX += m_nImageSize;
			k++;
		}
		// center of the panel
		coX = 400 - m_nImageSize/2;

		
		cvRelease((void**)&tempImage);
	}

	cvShowImage(m_mainWindowName.c_str(), m_dispImage);
}

void DepthTestDisplay::rotateImagesLeft(void) {
	if(m_nDisplayImageIndex > 0) {
		m_nDisplayImageIndex--;
		updateDisplay();
	}
}

void DepthTestDisplay::rotateImagesRight(void) {
	if(m_nDisplayImageIndex < m_nImageCount - 3) {
		m_nDisplayImageIndex++;
		updateDisplay();
	}
}

void DepthTestDisplay::rotateUp(void) { 
	if(m_nShowImages > 8) m_nShowImages = (m_nShowImages >> 1);
	printf("showImages: %d\n\n", m_nShowImages); 
}

void DepthTestDisplay::rotateDown(void) { 
	if(m_nShowImages < 16) m_nShowImages = (m_nShowImages << 1);
	printf("showImages: %d\n\n", m_nShowImages);
}


DepthTestDisplay::~DepthTestDisplay(void) {
	if(m_rightImage) cvReleaseImage(&m_rightImage);
	if(m_leftImage) cvReleaseImage(&m_leftImage);
	// clear all images
	for(int i = 0;i < m_nImageCount; i++) {
		if(m_warpImages[i]) cvReleaseImage(&m_warpImages[i]);
		if(m_diffImages[i]) cvReleaseImage(&m_diffImages[i]);
		if(m_likelihoodImages[i]) cvReleaseImage(&m_likelihoodImages[i]);
		if(m_filteredLikelihoodImages[i]) cvReleaseImage(&m_likelihoodImages[i]);
	}
	if(m_maxDisparityImage) cvReleaseImage(&m_maxDisparityImage);
	if(m_horizontalDisparityMap) cvReleaseImage(&m_horizontalDisparityMap);
	if(m_verticalDisparityMap) cvReleaseImage(&m_verticalDisparityMap);
	cvReleaseImage(&m_dispImage);
	cvDestroyWindow(m_mainWindowName.c_str());
}
