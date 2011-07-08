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
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <iCub/iKin/iKinFwd.h>

#include "CameraiCub.h"

typedef enum {Camera640, Camera320} Cameratype;


class StereoGeometry
{
private:

  Cameratype cameraType;

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
  cv::Mat RTb2w, RTw2b; //RT from board to world and vice versa
  cv::Mat RTwr2cr, RTcr2wr; //RT from wrong camera position to correct camera position and vice versa


  //Port to read encoders
  std::string inputHeadPortName;
  std::string inputTorsoPortName;
  std::string outputBBLeftPortName;
  std::string outputBBRightPortName;
  std::string outputCheckPortName;
  BufferedPort<yarp::os::Bottle> inputHeadPort;
  BufferedPort<yarp::os::Bottle> inputTorsoPort;
  BufferedPort<yarp::os::Bottle> outputBBLeftPort;
  BufferedPort<yarp::os::Bottle> outputBBRightPort;
  BufferedPort<yarp::os::Bottle> outputCheckPort;

  //only for matching
  Ptr<DescriptorMatcher> descriptorMatcher;

public:
  StereoGeometry(std::string moduleName, Cameratype type);
  virtual
  ~StereoGeometry();

  void changeCalibration(int w);

  bool matchingGabor(cv::Mat &gaborsLeft, cv::Mat &gaborsRight, std::vector<cv::DMatch> &matches );
  bool matching(cv::Mat &descLeft, cv::Mat &descRight, std::vector<cv::DMatch> &matches, DescriptorType type);
  bool detectChessBoard(cv::Size patternsize, float squaresize,  cv::Mat &image, cv::Mat &outImage, std::vector<cv::Point2f> &corners);
  void estimateRTfromImages(cv::Mat &imageLeft, cv::Mat &imageRight, cv::Mat &outImageLeft, cv::Mat &outImageRight);
  void estimatePmatrix(cv::Mat &K, cv::Mat &RTworld2camera, cv::Mat &P);
  void estimatePmatrix(cv::Mat &K, cv::Mat &Rworld2camera, cv::Mat &Tworld2camera, cv::Mat &P);

  bool loadCalibration(std::string filename = "calibrationMatrices.yaml");
  bool saveCalibration(std::string filename = "calibrationMatrices.yaml");
  void undistortImage(cv::Mat& image2undist, cv::Mat &undistortedImage, std::string camera);
  cv::Point2f undistortPoint(cv::Point2f &point2undistort, std::string camera);
  void undistortPoint(cv::Mat &point2undistort, cv::Mat &pointundistorted, std::string camera);

  void triangulatePoint(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d);
  void triangulatePointChessboard(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d);
  void triangulatePointLeftCamera(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d);

  void segmentOnDepth(std::vector<cv::KeyPoint> &keysLeft, std::vector<cv::KeyPoint> &keysRight, std::vector<cv::DMatch> &matches, int selectedFeature,
                      std::vector<int> &selectedIndexes, std::vector<cv::Point3f> &selectedPoints3d, cv::Point3f &lookAtPoint3d);

};

#endif /* STEREOGEOMETRY_H_ */
