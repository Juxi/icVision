/*
 * SaliencyMap.h
 *
 *  Created on: Jul 4, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef SALIENCYMAP_H_
#define SALIENCYMAP_H_

#include <opencv2/opencv.hpp>
#include "envLib/envision_lib.h"

class SaliencyMap
{
  private:
    bool move;
    cv::Mat outputLeft, outputRight;

  public:
    SaliencyMap();
    virtual
    ~SaliencyMap();

  int detectSaliencyPoint(cv::Mat &imLeft, cv::Mat &imRight, std::vector<cv::KeyPoint> &keysLeft, std::vector<cv::KeyPoint> &keysRight, std::vector<cv::DMatch> &matches);

  cv::Mat &getLeftMap(){return outputLeft;}
  cv::Mat &getRightMap(){return outputRight;}

};

#endif /* SALIENCYMAP_H_ */
