/* DisparityMapper.cpp
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-12
 * Class definition file for the DisparityMapper class.
 * This class is the class that handles the whole methodology of applying the Depth Perception algorithm.
 */
#include "StdAfx.h"

// standard constructor
// initializes all needed values, not the images since we do not know how many we need
DisparityMapper::DisparityMapper(void) {
	printDebug("StandardConstructor called");
	m_leftImage = m_rightImage = NULL;

	m_shiftHorizontalStart = m_shiftHorizontalEnd = m_shiftHorizontalStep = 0;
	m_shiftVerticalStart = m_shiftVerticalEnd = m_shiftVerticalStep = 0;
	m_imageCount = 0;

	m_horizontalOffset = m_verticalOffset = NULL;

	m_likelihoodDistribution = NULL;
	m_likelihoodSigma = .75;

	m_warpImage = m_differenceImage = m_likelihoodImage = m_filteredLikelihoodImage = NULL;
	m_maxActivationMap = m_horizontalDisparityMap = m_verticalDisparityMap = NULL;

	fltr = new LinFilter(this);
}

// TODO maybe add more constructors

// destructor
// free up all memory
DisparityMapper::~DisparityMapper(void) {
	if(m_leftImage) cvReleaseImage(&m_leftImage);
	if(m_rightImage) cvReleaseImage(&m_rightImage);

	if(m_warpImage) for(int i=0;i<m_imageCount;i++) cvReleaseImage(& m_warpImage[i]);
	if(m_differenceImage) for(int i=0;i<m_imageCount;i++) cvReleaseImage(& m_differenceImage[i]);
	if(m_likelihoodImage) for(int i=0;i<m_imageCount;i++) cvReleaseImage(& m_likelihoodImage[i]);
	if(m_filteredLikelihoodImage) for(int i=0;i<m_imageCount;i++) cvReleaseImage(& m_filteredLikelihoodImage[i]);

	if(m_maxActivationMap) cvReleaseImage(&m_maxActivationMap);
	if(m_horizontalDisparityMap) cvReleaseImage(&m_horizontalDisparityMap);
	if(m_verticalDisparityMap) cvReleaseImage(&m_verticalDisparityMap);

	if(fltr) free(fltr);

	printDebug("Destruction finished!");
}

// set the left image using another IplImage
// and convert it to 32bit if necessary 
void DisparityMapper::setLeftImage(IplImage *img) {
	if(img != NULL) {
		if(m_leftImage) cvReleaseImage(&m_leftImage);
		if(img->depth == 32) m_leftImage = img;
		else {
			// convert image to 32 bit
			m_leftImage = convertTo32F(img);
			if(m_leftImage == NULL) printError("The left image could not be converted to a 32bit image)!");
			else {
				char help[200];
				sprintf(help, "The left image was set.\twidth: %d height:%d", m_leftImage->width, m_leftImage->height);
				printDebug(help);
			}
		}
	}
	else
		printError("The left image received was NULL!");
}

// set the right image using another IplImage
// and convert it to 32bit if necessary 
void DisparityMapper::setRightImage(IplImage *img) {
	if(img != NULL){
		if(m_rightImage) cvReleaseImage(&m_rightImage);
		if(img->depth == 32) m_rightImage = img;
		else {
			// convert image to 32 bit
			m_rightImage = convertTo32F(img);
			if(m_rightImage == NULL) printError("The right image could not be converted to a 32bit image)!");
			else {
				char help[200];
				sprintf(help, "The right image was set.\twidth: %d height:%d", m_rightImage->width, m_rightImage->height);
				printDebug(help);
			}
		}
	}
	else
		printError("The right image received was NULL!");
}

// load the left image from a filename
// it loads the file into a grayscale image and then tries to store it in the object (thereby converting it)
void DisparityMapper::loadLeftImage(char *file) {
	// load left image (as grayscale)
	IplImage *help = cvLoadImage(file, 0); 
	if(!help) {
		char help[200];
		sprintf(help, "Could not load image file: %s", file);
		printError(help);
	} else 
		setLeftImage(help);
}

// load the right image from a filename
// it loads the file into a grayscale image and then tries to store it in the object (thereby converting it)
void DisparityMapper::loadRightImage(char *file) {
	// load left image (as grayscale)
	IplImage *help = cvLoadImage(file, 0); 
	if(!help) {
		char help[200];
		sprintf(help, "Could not load image file: %s", file);
		printError(help);
	} else 
		setRightImage(help);
}

// sets the ranges for the shifing and also an array with the corresponding shifts for each image
void DisparityMapper::setShiftingRange(int horizStart, int horizEnd, int horizStep,
									   int vertStart, int vertEnd, int vertStep) {
	int hImgCnt, vImgCnt;

	// check parameters for validity
	if(horizEnd >= horizStart && horizStep > 0) {
		m_shiftHorizontalStart = horizStart;
		m_shiftHorizontalEnd = horizEnd;
		m_shiftHorizontalStep = horizStep;
		hImgCnt = (horizEnd-horizStart)/horizStep + 1;
	}else m_shiftHorizontalStart = m_shiftHorizontalEnd = m_shiftHorizontalStep = 0;

	// check parameters for validity
	if(vertEnd >= vertStart && vertStep > 0) {
		m_shiftVerticalStart = vertStart;
		m_shiftVerticalEnd = vertEnd;
		m_shiftVerticalStep = vertStep;
		vImgCnt = (vertEnd-vertStart)/vertStep + 1;
	}else m_shiftHorizontalStart = m_shiftHorizontalEnd = m_shiftHorizontalStep = 0;

	m_imageCount = hImgCnt * vImgCnt;

	// set the offsets in the array
	m_horizontalOffset = new int[m_imageCount];
	m_verticalOffset = new int[m_imageCount];
	int i, help, velp;
	for(i=0, help = m_shiftHorizontalStart, velp = m_shiftVerticalStart; i < m_imageCount ;i++) {
		if(help > m_shiftHorizontalEnd) {
			help = m_shiftHorizontalStart;
			velp += m_shiftVerticalStep;
		}
		m_horizontalOffset[i] = help;
		m_verticalOffset[i] = velp;
		help += m_shiftHorizontalStep;
	}

	char str[99];
	sprintf(str, "Shifing Range set to (%d, %d, %d)/(%d, %d, %d)!", 
		m_shiftHorizontalStart, m_shiftHorizontalEnd, m_shiftHorizontalStep,
		m_shiftVerticalStart, m_shiftVerticalEnd, m_shiftVerticalStep);
	printDebug(str);
}

// sets the parameter needed for the likelihood calculation
void DisparityMapper::setLikelihoodParameters(char *distribution, double sigma) {
	m_likelihoodDistribution = distribution;
	m_likelihoodSigma = sigma;
	char str[99];
	sprintf(str, "Likelihood parameter set to %f", m_likelihoodSigma);
	printDebug(str);
}

/////////////////////////////////////////////
// main entry function
// does the whole methodical run of the algorithm
void DisparityMapper::generateDisparityMap() {
	// TODO check if all parameters are okay...
	bool loop = false;
	// allocate memory for the images (if not done so before)
	if(!m_warpImage) {
		loop = true;
		m_warpImage = new IplImage*[m_imageCount];
		for(int i=0;i < m_imageCount;i++) m_warpImage[i] = NULL;
	}
	if(!m_differenceImage) {
		loop = true;
		m_differenceImage = new IplImage*[m_imageCount];
		for(int i=0;i < m_imageCount;i++) m_differenceImage[i] = NULL;
	}
	if(!m_likelihoodImage) {
		loop = true;
		m_likelihoodImage = new IplImage*[m_imageCount];
		for(int i=0;i < m_imageCount;i++) m_likelihoodImage[i] = NULL;
	}
	if(!m_filteredLikelihoodImage) {
		loop = true;
		m_filteredLikelihoodImage = new IplImage*[m_imageCount];
		for(int i=0;i < m_imageCount;i++) m_filteredLikelihoodImage[i] = NULL;
	}
	if(loop)
		for(int i=0;i < m_imageCount;i++) {
			if(!m_warpImage[i]) m_warpImage[i] = cvCreateImage(cvGetSize(m_rightImage), IPL_DEPTH_32F, 1);
			if(!m_differenceImage[i]) m_differenceImage[i] = cvCreateImage(cvGetSize(m_rightImage), IPL_DEPTH_32F, 1);
			if(!m_likelihoodImage[i]) m_likelihoodImage[i] = cvCreateImage(cvGetSize(m_rightImage), IPL_DEPTH_32F, 1);
			if(!m_filteredLikelihoodImage[i]) m_filteredLikelihoodImage[i]= cvCreateImage(cvGetSize(m_rightImage), IPL_DEPTH_32F, 1);
		}

	// start computatioin
	printDebug("Starting computation...");
	for(int i=0;i < m_imageCount;i++) {
		// calculate
		computeWarpImage(i);
		computeDifferenceImage(i);
		computeLikelihoodImage(i);
		// applyFilter
		computeFilteredLikelihoodImage(i);
	}

	// allocate maps if not done so before
	if(!m_maxActivationMap) m_maxActivationMap = cvCreateImage(cvSize(m_rightImage->width, m_rightImage->height), IPL_DEPTH_32F, 1);
	if(!m_horizontalDisparityMap) m_horizontalDisparityMap = cvCreateImage(cvSize(m_rightImage->width, m_rightImage->height), IPL_DEPTH_32F, 1);
	if(!m_verticalDisparityMap) m_verticalDisparityMap = cvCreateImage(cvSize(m_rightImage->width, m_rightImage->height), IPL_DEPTH_32F, 1);

	// reset maps
	cvSetZero(m_horizontalDisparityMap);
	cvSetZero(m_verticalDisparityMap);
	
	// calculate maps
	computeMaxActivationMap();
	
	printDebug("Computation finished!");
}

// compute one warp image using the offsets set before
void DisparityMapper::computeWarpImage(int i) {
	cvSetZero(m_warpImage[i]);

	// Set the image ROI to the size of the new image
	cvSetImageROI(m_warpImage[i],
		cvRect((m_horizontalOffset[i] < 0) ? 0 : m_horizontalOffset[i],
			(m_verticalOffset[i] < 0) ? 0 : m_verticalOffset[i],
			m_warpImage[i]->width - abs(m_horizontalOffset[i]),
			m_warpImage[i]->height - abs(m_verticalOffset[i]))
	);

	cvSetImageROI(m_rightImage, 
		cvRect((m_horizontalOffset[i] < 0) ? -m_horizontalOffset[i] : 0,
			(m_verticalOffset[i] < 0) ? -m_verticalOffset[i] : 0, 
			m_rightImage->width - abs(m_horizontalOffset[i]),
			m_rightImage->height - abs(m_verticalOffset[i]))
	);
	
	// copy the image
	cvResize(m_rightImage, m_warpImage[i]);

	// Reset the ROI 
	cvResetImageROI(m_rightImage);
	cvResetImageROI(m_warpImage[i]);
}

// generate the difference images by simple subtraction
void DisparityMapper::computeDifferenceImage(int i) {
	cvSetZero(m_differenceImage[i]);
	cvSub(m_leftImage, m_warpImage[i], m_differenceImage[i]);
			cvAbs(m_differenceImage[i], m_differenceImage[i]);
}

// computes the likelihood images
void DisparityMapper::computeLikelihoodImage(int i) {
	bool gaussian = false;
	double val;

	// set method to use
	if(!strcmp(m_likelihoodDistribution, "gaussian")) gaussian = true;
	
	cvCopyImage(m_differenceImage[i], m_likelihoodImage[i]);	
	
	if(gaussian) {	// gaussian	
		// square
		val = -1/(2*(m_likelihoodSigma*m_likelihoodSigma));
		cvPow(m_likelihoodImage[i] , m_likelihoodImage[i], 2);
	} else {		// laplace
		val = -1/(2*abs(m_likelihoodSigma));
		cvAbs(m_likelihoodImage[i] , m_likelihoodImage[i] );
	}
	// rescale
	cvScale(m_likelihoodImage[i], m_likelihoodImage[i], val);

	// exponential function
	cvExp(m_likelihoodImage[i], m_likelihoodImage[i]);
}

// applies the filter to the image ...
void DisparityMapper::computeFilteredLikelihoodImage(int i) {
	cvCopyImage(m_likelihoodImage[i], m_filteredLikelihoodImage[i]);
	fltr->applyFilter(i);
}

// computes the maximum activation map as well as the horizontal and vertical disparity maps
void DisparityMapper::computeMaxActivationMap() {
	//for every image
	for(int i = 0;i < m_imageCount; i++) {
		// check whole image for max
		for(int y=0;y < m_filteredLikelihoodImage[i]->height;y++) {
			for(int x=0;x < m_filteredLikelihoodImage[i]->width;x++) {
				float *maxpixel = &((float*)(m_maxActivationMap->imageData + m_maxActivationMap->widthStep * y))[x * m_maxActivationMap->nChannels];
				float *pixel = &((float*)(m_filteredLikelihoodImage[i]->imageData + m_filteredLikelihoodImage[i]->widthStep * y))[x * m_filteredLikelihoodImage[i]->nChannels];
				if(*pixel > *maxpixel) {
					*maxpixel = *pixel;
					// set horizontal and vertical offset
					float *horizontal = &((float*)(m_horizontalDisparityMap->imageData + m_horizontalDisparityMap->widthStep * y))[x * m_horizontalDisparityMap->nChannels];
					*horizontal = (float) m_horizontalOffset[i];
					float *vertical = &((float*)(m_verticalDisparityMap->imageData + m_verticalDisparityMap->widthStep * y))[x * m_verticalDisparityMap->nChannels];
					*vertical = (float) m_verticalOffset[i];
				}
			}
		}
	}
}


// for displaying it is better to have a normalized (horizontal) disparity map
void DisparityMapper::normalizeDisparityMap() {
	// change scale ...
	double min, max;
	cvMinMaxLoc(m_horizontalDisparityMap, &min, &max);
	double slope = 1.0f/(max-min);
	double intercept = -slope*min;
	cvConvertScale(m_horizontalDisparityMap, m_horizontalDisparityMap, slope, intercept);
}

/////////////////////////////////////////////
// output functions for easier debug and error messages
void DisparityMapper::printError(char *err) {
	printf("[DisparityMapper] ERROR: %s\n", err);
}

void DisparityMapper::printDebug(char *dbg) {
	printf("[DisparityMapper] DEBUG: %s\n", dbg);
}

////////////////////////////////////////////
// static converting functions
IplImage* DisparityMapper::convertTo32F(const IplImage *img) {
	IplImage *help = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);
	cvConvertScale(img, help, (float)1.0/255.0, 0);
	return help;
}
