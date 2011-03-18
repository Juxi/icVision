/*
 * ColoredRect.h
 *
 *  Created on: Mar 1, 2011
 *      Author: migliore
 */

#ifndef COLOREDRECT_H_
#define COLOREDRECT_H_

#include <opencv2/opencv.hpp>

using namespace cv;

class ColoredRect
{

private:
  Rect rectangle;
  Scalar color;
  vector<Point> contour;
  Moments shapeMoments;
  double huMoments[7];
  bool momentsAreCalculated;
  bool huMomentsAreCalculated;

public:
  ColoredRect();
  ColoredRect(Rect &rect, Scalar &col, vector<Point> &cont);
  Rect getRect(){return rectangle; }
  RotatedRect getRotatedRect(){return minAreaRect(Mat(contour));
}
  Scalar getColor(){return color; }
  vector<Point> getContour(){return contour; }
  Point getPointOnContour(int i){if(i<(int)contour.size()) return contour[i]; else return Point(-1,-1);}

  void estimateMoments(Moments &moms, double h[7] = NULL);
  Point2d getBBCenter();
  Point2d getBBTopLeftCorner();

  virtual
  ~ColoredRect();
};

#endif /* COLOREDRECT_H_ */
