/*
 * StereoGeometry.h
 *
 *  Created on: Jul 4, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef STEREOGEOMETRY_H_
#define STEREOGEOMETRY_H_

#include <opencv2/opencv.hpp>
#include "CameraiCub.h"


class StereoGeometry
{
	private:
		cv::Mat K_left, d_left; //intrinsic parameters camera left
		cv::Mat K_right, d_right; //intrinsic parameters camera right
		cv::Mat R, T; //Rotation and translation matrices between camera left and right
		cv::Mat F; //Fundamental matrix
		cv::Mat E; //Essential matrix
		cv::Mat RTb2cl, RTcl2b; //RT from chessboard to camera left and vice versa
		cv::Mat P_left, P_right; //Left and Right camera matrices
		cv::Mat RTcl2w, RTw2cl; // RT from camera left to world and vice versa

		//only for matching
		Ptr<DescriptorMatcher> descriptorMatcher;

	public:
	  StereoGeometry();
	  virtual
	  ~StereoGeometry();

	  bool matchingGabor(cv::Mat &gaborsLeft, cv::Mat &gaborsRight, std::vector<cv::DMatch> &matches );
	  bool matching(cv::Mat &descLeft, cv::Mat &descRight, std::vector<cv::DMatch> &matches, DescriptorType type);
	  bool detectChessBoard(cv::Size patternsize, float squaresize,  cv::Mat &image, cv::Mat &outImage, std::vector<cv::Point2f> &corners);
	  void estimateRTfromImages(cv::Mat &imageLeft, cv::Mat &imageRight, cv::Mat &outImageLeft, cv::Mat &outImageRight);
//	  cv::Mat &estimatePmatrix(cv::Mat K, cv::Mat RTcamera2world);

	  bool loadCalibration(std::string filename = "calibrationMatrices.yaml");
	  bool saveCalibration(std::string filename = "calibrationMatrices.yaml");
	  cv::Mat& undistortImage(cv::Mat& image2undist);
};

#endif /* STEREOGEOMETRY_H_ */
