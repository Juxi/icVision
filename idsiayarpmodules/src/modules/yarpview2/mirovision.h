#ifndef MIROVISION_ISDEF
#define MIROVISION_ISDEF
#include <QtGui>
#include <vector>

#include "blob.h"
// #include "dot.h" // currently not used
#include "visionrobot.h"
#include "visionball.h"

class MiroVision
{
public:
	MiroVision();
	~MiroVision();
	enum VIEW_IMAGE_MODE {RGB,RED,GREEN,BLUE,HUE,SATURATION,VALUE};
	void findRobotsInImage(QImage& image);
	const VisionBall& getBall(){return ball;};
	const std::vector<VisionRobot>& getBlueRobots(){return blueRobots;};
	const std::vector<VisionRobot>& getYellowRobots(){return yellowRobots;};
	void normalize_pixel (int red, int green, int blue, double* n_red, double* n_green, double* n_blue);
	bool robot_pixel_green (int red, int green, int blue);
	bool robot_pixel_yellow (int red, int green, int blue);
	bool robot_pixel_blue (int red, int green, int blue);
	QImage detectedImage;
	bool testPixelRGB(int x,int y,int red,int green,int blue,const std::vector<Blob> &blobs,const DotParam& param);
	bool testPixelHSV(int x,int y,int hue,int saturation,int value,const std::vector<Blob> &blobs,const DotParam& param);
	bool testPixel(int x,int y,int red,int green,int blue,int hue,int saturation,int value,const std::vector<Blob> &blobs,const DotParam& param);
	void usePixel(int x,int y,std::vector<Blob> &blobs,const DotParam& param);
	void drawValidBlobs(const std::vector<Blob>& blobs,const DotParam& param,QImage& image);
	void drawAllBlobs(const std::vector<Blob>& blobs,const DotParam& param,QImage& image);
	void drawBlob(const Blob& blob,const DotParam& param,QImage& image) const;
	void validateBlobs(std::vector<Blob>& blobs,const DotParam& param);
	void printBlobs(std::vector<Blob>& blobs,const DotParam& param);
	int nearestBlob(std::vector<Blob> blobs, float x,float y);
	int getFreeID(const std::vector<VisionRobot>& robots);
	float calculateOrientation(float fromX, float fromY,float toX,float toY);
	void eraseNonValidBlobs(std::vector<Blob>& blobs,const DotParam& param);
	void updateRobots(std::vector<VisionRobot>& robots,std::vector<Blob>& mainBlobs,std::vector<Blob>& orientBlobs); 
	void updateBall(VisionBall& ball,std::vector<Blob>& ballBlobs);
	void drawRobots(const std::vector<VisionRobot>& robots,QImage& image);
	void drawBall(const VisionBall& ball,QImage& image);
	bool viewPixels;
	VIEW_IMAGE_MODE viewImage;
private:
	void print_all_robots();
	int ball_pixel (int red, int green, int blue);
	
	bool viewDetectedPixels;
	bool verbose;
	bool bDrawAllBlobs;
	bool bDrawBlobs;
	bool bDrawRobots;
	bool bTestRGB;
	bool bTestHSV;
	
	int nBlueBots; 
	int nYellowBots;
	
	VisionBall ball;
	DotParam ballParam;
	DotParam yellowParam;
	DotParam blueParam;
	DotParam greenParam;
	
	//std::vector<Dot> blueDots;
	//std::vector<Dot> yellowDots;
	//std::vector<Dot> greenDots;
	std::vector<VisionRobot> blueRobots;
	std::vector<VisionRobot> yellowRobots; 
 
	
	QRgb ROBOT_BLUE_PIXEL;
	QRgb ROBOT_YELLOW_PIXEL;
	QRgb ROBOT_GREEN_PIXEL;
	QRgb ROBOT_RED_PIXEL;
	QRgb ROBOT_BLACK_PIXEL;
	
	int lastHeight;
	int lastWidth;

	int robotDistMax;
	int greenDistMax;
	FILE* trackingfp;
	unsigned int sequencenum;

};

#endif