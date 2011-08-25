/*
 * CVUtils.h
 *
 *  Created on: Mar 10, 2011
 *      Author: migliore
 */

#ifndef CVUTILS_H_
#define CVUTILS_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <string>

#include "WorldObject.h"
#include "ColoredRect.h"


using namespace std;
using namespace cv;

class CVUtils
{
  private:

    /*Object id creator*/
    int objId;
    /*Colors Statistics*/
    vector<Mat> colorsStat;

    /*Image support*/
    Mat imageLeft, imageLeft_undist, imageLeft_gray;
    Mat imageRight, imageRight_undist, imageRight_gray;
    Mat outputImageLeft, outputImageRight;

    /*Camera Parameters*/
    Mat cameraMatrix_left, cameraMatrix_right;
    Mat distCoeffs_left, distCoeffs_right;
    Mat R, T, E;
    Mat F;

    Mat RT_left, RT_right;

    bool saveImages, useUndistImages, recomputeF;


    Mat fundfromcameras(Mat &camera_left, Mat &camera_right, Mat &R, Mat &T);
    void fundfromimages(Mat &camera_left, Mat &camera_right);
    void triangulatePoint(float p_l_x,float p_l_y, float p_r_x, float p_r_y,
         Mat& R, Mat& T, Mat & dist_l, Mat & dist_r,
         Mat& cameraMatrix_l, Mat& cameraMatrix_r,
         Mat& point3DLeft, Mat& point3DRight);
    void triangulatePoint(Point2f pl, Point2f pr, Mat& point3DLeft, Mat& point3DRight);

    void undistortPoint(Mat &point, Mat &dist);
    void undistortPoint(Point2f &point, Mat &dist);
    void colorSegmentation(Mat &image, Mat &m, Mat &icov, Mat &binaryImage);
    void detectPossibleColoredObject(Mat &image, Mat& outputMask, vector<ColoredRect> &listofrect, Mat &outputImage);
    void findConstrainedCorrespondences(
                     const vector<KeyPoint>& keypoints_left,
                     const vector<KeyPoint>& keypoints_right,
                     const Mat& descriptors_left,
                     const Mat& descriptors_right,
                     vector<DMatch>& matches,
                     float eps, double ratio);

    void findConstrainedCorrespondences(
                         const vector<KeyPoint>& keypoints_left,
                         const vector<KeyPoint>& keypoints_right,
                         const Mat& imageLeft,
                         const Mat& imageRight,
                         vector<DMatch>& matches,
                         Size window,
                         float threshold);

    void refineMatches(
         const vector<KeyPoint>& keypoints_left,
         const vector<KeyPoint>& keypoints_right,
         vector<DMatch>& matches,
         vector<vector <DMatch> > &listOfMatches,
         double eps);

    void estimateDisparityImage(Mat& disparityImage);
    int dataAssociation(vector<ColoredRect> &left_rectList, vector<ColoredRect> &right_rectList, vector<WorldObject> &object_list);
    bool saveImage(Mat& image2save, string directory, int framecounter);

    void findObjectContour(Mat &image, Mat &graylevelimage, ColoredRect &rect, Mat &outputImage);

    bool saveCalibrationMatrix();
    bool loadCalibrationMatrix();
    bool loadColorStats();

  public:
    CVUtils();
    void setUpImageStereoImages(Mat& imageL, Mat& imageR);
    void setUpCamera2World(Mat& left2world, Mat& right2world);
    int detectObjects(vector<WorldObject> &obj_list);
    Mat& getOutputImage(const char* camera){ return strcmp(camera, "left")?outputImageRight:outputImageLeft; /*0 if they are equals */}

    virtual
    ~CVUtils();
};

#endif /* CVUTILS_H_ */
