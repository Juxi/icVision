/*
 * SaliencyMap.h
 *
 *  Created on: Jul 4, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef SALIENCYMAP_H_
#define SALIENCYMAP_H_

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include <opencv2/opencv.hpp>
#include "envLib/envision_lib.h"

class SaliencyMap
{
private:

  cv::Mat outputLeft, outputRight;

  //Send Winner Points
  std::string namePointLeftOutPort;
  yarp::os::BufferedPort<yarp::os::Bottle > PointLeftOutPort;
  std::string namePointRightOutPort;
  yarp::os::BufferedPort<yarp::os::Bottle > PointRightOutPort;

public:
  SaliencyMap();
  virtual
  ~SaliencyMap();

  int detectSaliencyPoint(cv::Mat &imLeft, cv::Mat &imRight, std::vector<cv::KeyPoint> &keysLeft, std::vector<cv::KeyPoint> &keysRight, std::vector<cv::DMatch> &matches);
  void inhibitSaliencyMap(cv::Point2f &pointleft, cv::Point2f &pointright, int numberOfFeatureLeft, int numberOfFeatureRight);
  bool interrupt();
  bool close();

  cv::Mat &getLeftMap(){return outputLeft;}
  cv::Mat &getRightMap(){return outputRight;}

  std::string getPortName(int flag){ if(!flag) return namePointLeftOutPort; else return namePointRightOutPort;}

  bool move;

};

#endif /* SALIENCYMAP_H_ */
