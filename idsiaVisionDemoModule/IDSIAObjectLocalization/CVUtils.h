/*
 * CVUtils.h
 *
 *  Created on: Mar 10, 2011
 *  Author: migliore
 *  Descrition: This file contains the functions needed for image processing.
 *  A list of utils that allows to localize color object in the image
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

    Mat RT_left, RT_right, iRT_left, iRT_right;
    Mat RT_left_checkboard, RT_c2r;

    Mat P_left, P_right;
    Mat P_left_tmp, P_right_tmp;


    bool saveImages, useUndistImages, recomputeF;

    //Estimate the fundamental matrix from the camera parameters
    Mat fundfromcameras(Mat &camera_left, Mat &camera_right, Mat &R, Mat &T);
    //Estimate the fundamental matrix from the images
    void fundfromimages(Mat &camera_left, Mat &camera_right);
    //Triangulate points, given two 2D points in left and right camera estimate the corresponding 3D point
    void triangulatePoint(float p_l_x,float p_l_y, float p_r_x, float p_r_y,
         Mat& R, Mat& T, Mat & dist_l, Mat & dist_r,
         Mat& cameraMatrix_l, Mat& cameraMatrix_r,
         Mat& point3DLeft, Mat& point3DRight);
    //Triangulate points, given two 2D points in left and right camera estimate the corresponding 3D point
    void triangulatePoint(Point2f pl, Point2f pr, Mat& point3DLeft, Mat& point3DRight);
    //Triangulate points, given two 2D points in left and right camera estimate the corresponding 3D point
    void triangulatePointNew(Point2f pl, Point2f pr, Point3f& point3D);


    //Undistort the point given the point and the undistort parameters
    void undistortPoint(Mat &point, Mat &dist);
    //Undistort the point given the point and the undistort parameters
    void undistortPoint(Point2f &point, Mat &dist);
    //Segment the image on hte base of the color given the mean and covariance of the color and returning a binary image
    void colorSegmentation(Mat &image, Mat &m, Mat &icov, Mat &binaryImage);
    //Detect possible colored Object in image returning a list of colored rectangles
    void detectPossibleColoredObject(Mat &image, Mat& outputMask, vector<ColoredRect> &listofrect, Mat &outputImage);
    //Find the correspondences constrained to the stereo rules between the left and the right keypoints, given their descriptors and returing back the list of matches
    void findConstrainedCorrespondences(
                     const vector<KeyPoint>& keypoints_left,
                     const vector<KeyPoint>& keypoints_right,
                     const Mat& descriptors_left,
                     const Mat& descriptors_right,
                     vector<DMatch>& matches,
                     float eps, double ratio);
    //Find the correspondences constrained to the stereo rules between the left and the right keypoints, given their descriptors and returing back the list of matches
    void findConstrainedCorrespondences(
                         const vector<KeyPoint>& keypoints_left,
                         const vector<KeyPoint>& keypoints_right,
                         const Mat& imageLeft,
                         const Mat& imageRight,
                         vector<DMatch>& matches,
                         Size window,
                         float threshold);

    //Remove wrong matches
    void refineMatches(
         const vector<KeyPoint>& keypoints_left,
         const vector<KeyPoint>& keypoints_right,
         vector<DMatch>& matches,
         vector<vector <DMatch> > &listOfMatches,
         double eps);

    //Estimate disparity map
    void estimateDisparityImage(Mat& disparityImage);
    //Given the left and the right image associate to each couple a possible object, if there is no correspondence create a new object or remove it
    int dataAssociation(vector<ColoredRect> &left_rectList, vector<ColoredRect> &right_rectList, vector<WorldObject> &object_list);

    //Find the contour of possible colored objects
    void findObjectContour(Mat &image, Mat &graylevelimage, ColoredRect &rect, Mat &outputImage);

    //Save the calibration matrix
    bool saveCalibrationMatrix();
    //Load the calibration matrix
    bool loadCalibrationMatrix();
    //Load the statistics of colors
    bool loadColorStats();

  public:
    CVUtils();
    //Function to save the image in a particular directory
    bool saveImage(Mat& image2save, string directory, int framecounter);
    //Initialize the Stereo Image
    void setUpImageStereoImages(Mat& imageL, Mat& imageR);
    //Initialize the transformation from left camera to world and the transformation from right camera to world
    void setUpCamera2World(Mat& left2world, Mat& right2world);
    //Detect the object considering the obj_list, if there is a new object add it to the list
    int detectObjects(vector<WorldObject> &obj_list);
    //Get an ouput image for the left or right camera
    Mat& getOutputImage(const char* camera){ return strcmp(camera, "left")?outputImageRight:outputImageLeft; /*0 if they are equals */}

    virtual
    ~CVUtils();
};

#endif /* CVUTILS_H_ */
