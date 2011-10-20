/*
 * ColoredRect.h
 *
 *  Created on: Mar 1, 2011
 *      Author: migliore
 *  Description: This class is extension of a classic rectangle to
 *  represent a colored rect (useful to assign a color to an object bounding box)
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
  //Constructor
  ColoredRect();
  //Constructor receiving a rectangle, a color expressed with CV::Scalar, a list of points of the object
  ColoredRect(Rect &rect, Scalar &col, vector<Point> &cont);
  //Return the revctangle
  Rect getRect(){return rectangle; }
  //Return a rotated rectangle
  RotatedRect getRotatedRect(){return minAreaRect(Mat(contour));}
  //Return the color of the rectangle
  Scalar getColor(){return color; }
  //Return the point belonging to the countour of the object
  vector<Point> getContour(){return contour; }
  //Return a particular point i of the contour
  Point getPointOnContour(int i){if(i<(int)contour.size()) return contour[i]; else return Point(-1,-1);}
  //Estimate the moments of the shape contained in the rectangle
  void estimateMoments(Moments &moms, double h[7] = NULL);
  //Return the center of the rectangle/boundingbox
  Point2d getBBCenter();
  //Return the top left corner of the rectangle
  Point2d getBBTopLeftCorner();

  //Compare two colored rect
  bool operator==(ColoredRect &other);

  virtual
  ~ColoredRect();
};

#endif /* COLOREDRECT_H_ */
