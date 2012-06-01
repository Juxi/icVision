/* DisparityMapper.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-12
 * Class declaration file for the DepthTestDisplay class.
 * This class is the encapsulation for a visualization of the various steps of the algorithm
 */
#pragma once

#include <string>

class DepthTestDisplay
{
private:
	std::string m_mainWindowName;
	IplImage *m_rightImage;
	IplImage *m_leftImage;
	IplImage **m_warpImages;
	IplImage **m_diffImages;
	IplImage **m_likelihoodImages;
	IplImage **m_filteredLikelihoodImages;
	IplImage *m_maxDisparityImage, *m_horizontalDisparityMap, *m_verticalDisparityMap;
	int m_nImageCount, m_nImageSize, m_nDisplayImageIndex;
	int m_nShowImages; //bitmask to define which images to show ...
	// 1: warped images
	// 2: differential images
	// 4: likelihood images
	// 8: filtered likelihood images
	// 16: disparity map...

	IplImage *m_dispImage;

public:
	// constructor
	DepthTestDisplay(void);
	~DepthTestDisplay(void);

	// update
	void updateDisplay(void);

	// set functions
	void setLeftImage(IplImage *leftImage);
	void setRightImage(IplImage *rightImage);
	void setWarpImage(IplImage *img);
	void setWarpImages(IplImage **images, int nCnt);
	void setDiffImage(IplImage *img);
	void setDiffImages(IplImage **images, int nCnt);
	void setLikelihoodImage(IplImage *img);
	void setLikelihoodImages(IplImage **images, int nCnt);
	void setFilteredLikelihoodImage(IplImage *img);
	void setFilteredLikelihoodImages(IplImage **images, int nCnt);
	void setMaxDisparityImage(IplImage *image);
	void setHorizontalDisparityMap(IplImage *image);
	void setVerticalDisparityMap(IplImage *image);

	// rotate images
	void rotateImagesLeft(void);
	void rotateImagesRight(void);
	void rotateUp(void);
	void rotateDown(void);	
};


