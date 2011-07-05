/*
 * GeoUtils.h
 *
 *  Created on: May 25, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef GEOUTILS_H_
#define GEOUTILS_H_

#include "WorldObject.h"

class GeoUtils
{
  private:
    /*Image support*/
    Mat imageLeft, imageLeft_undist, imageLeft_gray;
    Mat imageRight, imageRight_undist, imageRight_gray;
    Mat outputImageLeft, outputImageRight;

    /*Camera Parameters*/
    Mat cameraMatrix_left, cameraMatrix_right;
    Mat distCoeffs_left, distCoeffs_right;
    Mat R, T, E;
    Mat F;
    Mat P_left, P_right;

    /*Transformation*/
    Mat RT_l2w, RT_r2w; //Left and Right to World
    Mat RT_w2l, RT_w2r; //Left and Right to World
    Mat RT_l2c, RT_r2c; //Left and Right to Chessboard

  public:
    GeoUtils();
    virtual ~GeoUtils();

    void setUpCameraTransformation(Mat& left2world, Mat& right2world);
    Mat fundfromcameras();
    Mat fundfromimages();
    void estimateRTFromImages();
    void triangulatePoint(float p_l_x,float p_l_y, float p_r_x, float p_r_y, Mat& point3DLeft, Mat& point3DRight);
    void triangulatePoint(Point2f pl, Point2f pr, Mat& point3DLeft, Mat& point3DRight);
    void triangulatePointNew(Point2f pl, Point2f pr, Point3f& point3D);

    void undistortPoint(Mat &point, Mat &dist);
    void undistortPoint(Point2f &point, Mat &dist);

    bool saveCalibrationMatrix();
    bool loadCalibrationMatrix();

    void lrDataAssociation(vector<ColoredRect> &l_listofrect, vector<ColoredRect> &r_listofrect, vector<DMatch> &matching, float epipolarDistance = 5);
};

#endif /* GEOUTILS_H_ */
