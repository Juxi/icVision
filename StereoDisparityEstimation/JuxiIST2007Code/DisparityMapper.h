/* DisparityMapper.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-10
 * Class declaration file for the DepthYARP class.
 * This class is the encapsulation for a YARP module that runs the Depth Perception algorithm.
 */
#pragma once

class DisparityMapper {
// variables
private:
	// the starting images (left and right eye)
	IplImage *m_leftImage, *m_rightImage;

	// counter for the images (used in every step)
	int m_imageCount;

	// ranges and steps for the shifting in horizontal and vertical direction
	int m_shiftHorizontalStart, m_shiftHorizontalEnd, m_shiftHorizontalStep;
	int m_shiftVerticalStart, m_shiftVerticalEnd, m_shiftVerticalStep;

	int *m_horizontalOffset, *m_verticalOffset;

	char *m_likelihoodDistribution;
	double m_likelihoodSigma;

	// the filter used to do the filtering of the neighborhood images
	Filter *fltr;

	// the refernces to the images for the various steps
	IplImage **m_warpImage;
	IplImage **m_differenceImage;
	IplImage **m_likelihoodImage;
	IplImage **m_filteredLikelihoodImage;

	IplImage *m_maxActivationMap;
	IplImage *m_horizontalDisparityMap, *m_verticalDisparityMap;

	// computation functions for the various steps of the method
	void computeWarpImage(int i);
	void computeDifferenceImage(int i);
	void computeLikelihoodImage(int i);
	void computeFilteredLikelihoodImage(int i);
	void computeMaxActivationMap(void);
	
	// output functions
	void printError(char *err);
	void printDebug(char *dbg);

public:
	// constructor & distructor
	DisparityMapper(void);
	~DisparityMapper(void);

	// load start images from files
	void loadLeftImage(char *file);
	void loadRightImage(char *file);

	// access methods
	void setLeftImage(IplImage *img);
	void setRightImage(IplImage *img);
	void setFilter(Filter *flt);

	// setting internal parameters
	void setShiftingRange(int horizStart, int horizEnd, int horizStep = 2,
						int vertStart = 0, int vertEnd = 0, int vertStep = 2);
	void setLikelihoodParameters(char *distribution, double sigma = .75);
	

	// simple retrieving functions for the intermediate steps
	int getImageCount(void) {return m_imageCount; };
	IplImage* getLeftImage(void) {return m_leftImage; };
	IplImage* getRightImage(void) {return m_rightImage; };
	IplImage** getWarpImages(void) {return m_warpImage; };
	IplImage** getDifferenceImages(void) {return m_differenceImage; };
	IplImage** getLikelihoodImages(void) {return m_likelihoodImage; };
	IplImage** getFilteredLikelihoodImages(void) {return m_filteredLikelihoodImage; };
	Filter* getFilter(void) { return fltr; }

	// retrievin functions for the "results"
	IplImage* getHorizontalDisparityMap(void) {return m_horizontalDisparityMap; };
	IplImage* getVerticalDisparityMap(void) {return m_verticalDisparityMap; };
	IplImage* getMaxActivationMap(void) {return m_maxActivationMap; };


	// main entry function
	void generateDisparityMap();

	// to normalize the disparity map (0-1 float)
	void normalizeDisparityMap(void);

	// a static function to concert 8u images to 32f images
	static IplImage* convertTo32F(const IplImage*);
};