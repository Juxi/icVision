/*
 * StereoGeometry.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#include "StereoGeometry.h"

using namespace std;
using namespace cv;

StereoGeometry::StereoGeometry()
{
  // TODO Auto-generated constructor stub

}

StereoGeometry::~StereoGeometry()
{
  // TODO Auto-generated destructor stub
}

bool StereoGeometry::matchingGabor(Mat &gaborsLeft, Mat &gaborsRight, std::vector<cv::DMatch> &matches ){

  Mat multresult;
  float testvalue;
  DMatch match;
  bool found;

  for(int i=0; i<gaborsLeft.rows; i++){

      testvalue = FLT_MIN;
      found = false;

      for(int j=0; j<gaborsRight.rows; j++){

          Mat leftrow = gaborsLeft.row(i);
          Mat rightrow = gaborsRight.row(j);
          multiply(leftrow, rightrow, multresult);
          Scalar similarity = sum(multresult);

          if(similarity[0]>testvalue){
              testvalue = similarity[0];
              match.queryIdx = i;
              match.trainIdx = j;
              match.distance = similarity[0];
              found = true;
          }

      }

      if(found && testvalue > 0.95){
          matches.push_back(match);
      }


  }

  return true;
}
