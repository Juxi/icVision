/*
 * ColoredRect.cpp
 *
 *  Created on: Mar 1, 2011
 *      Author: migliore
 */

#include "ColoredRect.h"


//ColoredRect::ColoredRect()
//{
//  // TODO Auto-generated constructor stub
//}
//
//
//ColoredRect::ColoredRect(Rect &rect, Scalar &col, vector<Point> &cont)
//{
//  rectangle = rect;
//  color = col;
//  contour = cont;
//  momentsAreCalculated = false;
//  huMomentsAreCalculated = false;
//  // TODO Auto-generated constructor stub
//
//}
//
//ColoredRect::~ColoredRect()
//{
//  // TODO Auto-generated destructor stub
//}
//
//void ColoredRect::estimateMoments(Moments &moms, double h[7] ){
//
//  if(!momentsAreCalculated){
//    moms = moments(Mat(contour));
//    shapeMoments = moms;
//
//    momentsAreCalculated = true;
//
//  }
//  else
//    moms = shapeMoments;
//
//  if(!huMomentsAreCalculated && h != NULL){
//    HuMoments(moms, h);
//    for(int i=0;i<7;i++)
//      huMoments[i] = h[i];
//
//    huMomentsAreCalculated = true;
//  }
//  else
//    h = huMoments;
//
//}
//
//Point2d ColoredRect::getBBCenter(){
//
//  if(!momentsAreCalculated)
//    shapeMoments = moments(Mat(contour));
//
//  return Point2d(shapeMoments.m10/shapeMoments.m00, shapeMoments.m01/shapeMoments.m00);
//     //Point2d(rectangle.x + rectangle.width/2, rectangle.y + rectangle.height/2 );
//
//}
//
//Point2d ColoredRect::getBBTopLeftCorner(){
//
//  return Point2d(rectangle.x, rectangle.y );
//
//}
