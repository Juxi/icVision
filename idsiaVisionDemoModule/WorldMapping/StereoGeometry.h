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

typedef enum {Camera640, Camera320} Cameratype;


class StereoGeometry
{
private:
  cv::Mat K_left, d_left; //intrinsic parameters camera left
  cv::Mat K_right, d_right; //intrinsic parameters camera right
  cv::Mat R, T; //Rotation and translation matrices between camera left and right
  cv::Mat F; //Fundamental matrix
  cv::Mat E; //Essential matrix
  cv::Mat RTb2cl, RTcl2b; //RT from chessboard to camera left and vice versa
  cv::Mat RTb2cr, RTcr2b; //RT from chessboard to camera right and vice versa
  cv::Mat P_left, P_right; //Left and Right camera matrices
  cv::Mat RTcl2w, RTw2cl; // RT from camera left to world and vice versa
  cv::Mat RTcr2w, RTw2cr; // RT from camera right to world and vice versa

  //only for matching
  Ptr<DescriptorMatcher> descriptorMatcher;

public:
  StereoGeometry(Cameratype type);
  virtual
  ~StereoGeometry();

  bool matchingGabor(cv::Mat &gaborsLeft, cv::Mat &gaborsRight, std::vector<cv::DMatch> &matches );
  bool matching(cv::Mat &descLeft, cv::Mat &descRight, std::vector<cv::DMatch> &matches, DescriptorType type);
  bool detectChessBoard(cv::Size patternsize, float squaresize,  cv::Mat &image, cv::Mat &outImage, std::vector<cv::Point2f> &corners);
  void estimateRTfromImages(cv::Mat &imageLeft, cv::Mat &imageRight, cv::Mat &outImageLeft, cv::Mat &outImageRight);
  void estimatePmatrix(cv::Mat &K, cv::Mat &RTworld2camera, cv::Mat &P);

  bool loadCalibration(std::string filename = "calibrationMatrices.yaml");
  bool saveCalibration(std::string filename = "calibrationMatrices.yaml");
  cv::Mat undistortImage(cv::Mat& image2undist, std::string camera);
  cv::Point2f undistortPoint(cv::Point2f &point2undistort, std::string camera);
  void undistortPoint(cv::Mat &point2undistort, cv::Mat &pointundistorted, std::string camera);

  void triangulatePoint(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d);
  void triangulatePointChessboard(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d);
  void triangulatePointLeftCamera(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d);

};

#endif /* STEREOGEOMETRY_H_ */
