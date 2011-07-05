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


class StereoGeometry
{
public:
  StereoGeometry();
  virtual
  ~StereoGeometry();

  bool matchingGabor(cv::Mat &gaborsLeft, cv::Mat &gaborsRight, std::vector<cv::DMatch> &matches );

};

#endif /* STEREOGEOMETRY_H_ */
