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
  RTb2cl = Mat::eye(4,4,CV_32FC1);
  RTcl2b = Mat::eye(4,4,CV_32FC1);
  P_left = Mat::zeros(4,3,CV_32FC1);
  P_right = Mat::zeros(4,3,CV_32FC1);
  RTcl2w = Mat::eye(4,4,CV_32FC1);
  RTw2cl = Mat::eye(4,4,CV_32FC1);

  loadCalibration("../conf/calibrationMatrices.yaml");


}

StereoGeometry::~StereoGeometry()
{
  // TODO Auto-generated destructor stub
  saveCalibration("../conf/calibrationMatrices.yaml");

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

      if(found && testvalue > 0.9){
          matches.push_back(match);
      }


  }

  return true;
}

bool StereoGeometry::matching(Mat &descLeft, Mat &descRight, vector<cv::DMatch> &matches, DescriptorType type){

  if(type == GABOR){
      return matchingGabor(descLeft, descRight, matches );
  }
  else if(type == DBRIEF){
      if(descriptorMatcher == NULL)
        descriptorMatcher =  DescriptorMatcher::create("BruteForce-HammingLUT");
  }
  else{
      if(descriptorMatcher == NULL)
        descriptorMatcher =  DescriptorMatcher::create("FlannBased");
  }

  descriptorMatcher->match(descLeft, descRight, matches);

  return true;

}

bool StereoGeometry::detectChessBoard(Size patternsize, float squaresize,  Mat &image, Mat &outImage, vector<Point2f> &corners){
  //CALIB_CB_FAST_CHECK saves a lot of time on images
  //that don't contain any chessboard corners
  bool found = findChessboardCorners(image, patternsize, corners,
      CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
      + CALIB_CB_FAST_CHECK);

  if(found)
    cornerSubPix(image, corners, Size(5, 5), Size(-1, -1),
        TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));


  drawChessboardCorners(outImage, patternsize, Mat(corners), found);

  return found;
}

void StereoGeometry::estimateRTfromImages(Mat &imageLeft, Mat &imageRight, Mat &outImageLeft, Mat &outImageRight){

  Mat grayImageLeft, grayImageRight;
  if(imageLeft.channels() > 1)
    cvtColor(imageLeft, grayImageLeft, CV_BGR2GRAY);
  else
    grayImageLeft = imageLeft.clone();

  if(imageRight.channels() > 1)
    cvtColor(imageRight, grayImageRight, CV_BGR2GRAY);
  else
    grayImageRight = imageRight.clone();

  //Search pattern in the scene
  Size patternsize(8,6); //interior number of corners
  float squareSize = 30;

  vector<vector<Point2f> > imagePoints_left;
  vector<vector<Point2f> > imagePoints_right;
  vector<vector<Point3f> > objectPoints;

  //Initialize the real points
  vector<Point3f> singleObjectPoints;
  int numberOfPoints = patternsize.width*patternsize.height;
  singleObjectPoints.resize(numberOfPoints);
  for( int i = 0; i < patternsize.height; i++ )
    for( int j = 0; j < patternsize.width; j++ )
      singleObjectPoints[i*patternsize.width + j] = Point3f(i*squareSize, j*squareSize, 0);

  vector<Point2f> corners_left; //this will be filled by the detected corners
  bool found_left = detectChessBoard(patternsize, squareSize,  grayImageLeft, outImageLeft,  corners_left);

  vector<Point2f> corners_right; //this will be filled by the detected corners
  bool found_right = detectChessBoard(patternsize, squareSize,  grayImageRight, outImageRight,  corners_right);

  if(found_left && found_right){

      cout<<"Found chessboard on left and right camera"<<endl;
      imagePoints_left.push_back(corners_left);
      imagePoints_right.push_back(corners_right);
      objectPoints.push_back(singleObjectPoints);

      //Calibrate the position of right and left camera (maintaining fixed K and d)
      double reproj_error = stereoCalibrate(objectPoints, imagePoints_left, imagePoints_right,
          K_left, d_left, K_right, d_right, grayImageLeft.size(), R, T, E, F,
          cvTermCriteria(CV_TERMCRIT_ITER+
              CV_TERMCRIT_EPS, 100, 1e-5),
              CV_CALIB_FIX_INTRINSIC +
              CV_CALIB_FIX_K1+ CV_CALIB_FIX_K2 + CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);


      //Estimate chessboard position wrt camera left
      Mat Rlefttmp, Rlefttmp_vec;
      Mat Tlefttmp;
      //Solve ransac
      //solvePnPRansac(singleObjectPoints, corners_left, K_left, d_left, Rlefttmp_vec, Tlefttmp); //BUG2FIX
      solvePnP(singleObjectPoints, corners_left, K_left, d_left, Rlefttmp_vec, Tlefttmp);
      Rodrigues(Rlefttmp_vec, Rlefttmp);


      //Convert everything to float
      K_left.convertTo(K_left, CV_32F);
      K_right.convertTo(K_right, CV_32F);
      d_left.convertTo(d_left, CV_32F);
      d_right.convertTo(d_right, CV_32F);
      R.convertTo(R, CV_32F);
      T.convertTo(T, CV_32F);
      Rlefttmp.convertTo(Rlefttmp, CV_32F);
      Tlefttmp.convertTo(Tlefttmp, CV_32F);


      //This update the transformation from chessnboard coordination to camera coordinates
      for(int i = 0; i<3; i++)
        for(int j = 0; j<3; j++)
          RTb2cl.at<float>(i,j) = Rlefttmp.at<float>(i,j);

      RTb2cl.at<float>(0,3) = Tlefttmp.at<float>(0,0);
      RTb2cl.at<float>(1,3) = Tlefttmp.at<float>(1,0);
      RTb2cl.at<float>(2,3) = Tlefttmp.at<float>(2,0);

      //From camera to chessboard
      RTcl2b = RTb2cl.inv();

      //If you want estimate the F matrix using the calibration matrices and the R,T estimated
      //Mat Fext = fundfromcameras(K_left, K_right, R, T);
      //F = Fext;

  }

}


bool StereoGeometry::loadCalibration(string filename){
  FileStorage fs( filename, CV_STORAGE_READ );

  fs["Intrinsics_left"]>>K_left;
  fs["Intrinsics_right"]>>K_right;
  fs["Distorsion_left"]>>d_left;
  fs["Distorsion_right"]>>d_right;

  fs["R"]>>R;
  fs["T"]>>T;
  fs["F"]>>F;

  fs.release();

  return true;
}

bool StereoGeometry::saveCalibration(string filename){
  FileStorage fs(filename, CV_STORAGE_WRITE);
  if (!fs.isOpened())
    return false;


  fs<<"Intrinsics_left"<<K_left;
  fs<<"Intrinsics_right"<<K_right;
  fs<<"Distorsion_left"<<d_left;
  fs<<"Distorsion_right"<<d_right;

  fs<<"R"<<R;
  fs<<"T"<<T;
  fs<<"F"<<F;

  fs.release();
}

Mat StereoGeometry::undistortImage(Mat& image2undist){
  //TODO
}


Mat StereoGeometry::estimatePmatrix(Mat &K, Mat &RTworld2camera){

  Mat P = Mat::zeros(3,4, K.type());
 // Mat iR_left = Mat::eye(3,3, RTcamera2world.type());
 // Mat iT_left =  Mat::zeros(3,1, RTcamera2world.type());
  Mat iR_left = RTworld2camera(Rect(0,0,3,3));
  Mat iT_left =  RTworld2camera(Rect(3,0,1,3)); //Convertion from m->mm ?

  Mat P_tmpR = P(Rect(0,0,3,3));
  Mat P_tmpT = P(Rect(3,0,1,3));
  Mat tmplR = K*iR_left;
  Mat tmplT = K*iT_left;
  tmplR.copyTo(P_tmpR);
  tmplT.copyTo(P_tmpT);

  return P;
}

