/*
 * CameraiCub.h
 *
 *  Created on: Jul 1, 2011
 *      Author: icub
 */

#ifndef CAMERAICUB_H_
#define CAMERAICUB_H_

#include <opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include "GaborDescriptor.h"

typedef enum{HARRIS, GFTT, FAST10, DeSURF} FeatureType;
typedef enum{DSURF, DBRIEF, GABOR} DescriptorType;

class CameraiCub {
private:
	std::string nameflag; //Camera left or right?
	std::string inputPortName;
	std::string outputPortName;
	std::string outputHarrisPortName;

	yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> > inputPort;
	yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> > outputPort;
	yarp::os::BufferedPort<yarp::os::Bottle > outputHarrisPort;

	bool thereIsImage;
	yarp::sig::ImageOf<yarp::sig::PixelBgr> *imagein;
	yarp::sig::ImageOf<yarp::sig::PixelBgr> imageout;
	IplImage *iplimagein, *iplimageout;
	IplImage *iplimageinGrayLevel;
	cv::Mat imageMat;

	//For feature detector
	cv::Ptr<cv::FeatureDetector> detector;
    std::vector<cv::KeyPoint> keypoints;

    cv::Ptr<cv::DescriptorExtractor> descriptor;
    cv::Mat descMatrix;

    //Gabor Descriptor
    GaborDescriptor gaborDescriptor;
    cv::Mat gaborDescMatrix;


public:
	CameraiCub(std::string moduleName, std::string flag = "left");
	~CameraiCub();
    //void execute();

	bool connect(std::string port2connect);
	bool interrupt();
	bool close();

	bool getImageOnOutputPort();
	bool getFeaturesOnOutputPort(FeatureType type = HARRIS);
	bool getGaborDescriptorsOnOutputPort();
	bool getDescriptorsOnOutputPort(DescriptorType type = DBRIEF);

	std::vector<cv::KeyPoint>& getKeypoints(){return keypoints;}
	Mat& getGaborDescriptors(){return gaborDescMatrix;}
	Mat& getDescriptors(){return descMatrix;}

	Mat& getImage(){ imageMat = Mat(iplimagein,false); return imageMat;}


};

#endif /* CAMERAICUB_H_ */
