/*
 * CVUtils.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: migliore
 */

#include "CVUtils.h"

CVUtils::CVUtils()
{
  //Do you want save images?
  saveImages = false;
  //Do you want use undistorted images?
  useUndistImages = false;
  //Do you want recompute F?
  recomputeF = true;

  //Object counter
  objId = 0;

  //Load calibration data
  loadCalibrationMatrix();

  //Load color statistics
  loadColorStats();

}

CVUtils::~CVUtils()
{
  //Save the calibration data!
  saveCalibrationMatrix();
}


Mat CVUtils::fundfromcameras(Mat &camera_left, Mat &camera_right, Mat &R, Mat &T){

  Mat invKl = camera_left.inv();
  Mat invKr = camera_right.inv();
  Mat invKrT = invKr.t();

  double t1 = T.at<float>(0,0), t2 = T.at<float>(1,0), t3 = T.at<float>(2,0);

  Mat tx = (Mat_<float>(3,3) << 0, -t3, t2, t3, 0, -t1, -t2, t1, 0);

  Mat F = invKrT*tx*R*invKl;

  return F;
}



void CVUtils::fundfromimages(Mat &camera_left, Mat &camera_right){

  Size patternsize(8,6); //interior number of corners
  vector<Point2f> corners_left; //this will be filled by the detected corners
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


  //CALIB_CB_FAST_CHECK saves a lot of time on images
  //that don't contain any chessboard corners
  bool found_left = findChessboardCorners(imageLeft_gray, patternsize, corners_left,
          CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
          + CALIB_CB_FAST_CHECK);

  if(found_left)
    cornerSubPix(imageLeft_gray, corners_left, Size(5, 5), Size(-1, -1),
      TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

  drawChessboardCorners(outputImageLeft, patternsize, Mat(corners_left), found_left);

  vector<Point2f> corners_right; //this will be filled by the detected corners

  bool found_right = findChessboardCorners(imageRight_gray, patternsize, corners_right,
          CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
          + CALIB_CB_FAST_CHECK);

  if(found_right)
    cornerSubPix(imageRight_gray, corners_right, Size(5, 5), Size(-1, -1),
      TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

  drawChessboardCorners(outputImageRight, patternsize, Mat(corners_right), found_right);


  if(found_left && found_right){

    imagePoints_left.push_back(corners_left);
    imagePoints_right.push_back(corners_right);
    objectPoints.push_back(singleObjectPoints);

 //   cout<<cameraMatrix_left.type()<<endl;

    double reproj_error = stereoCalibrate(objectPoints, imagePoints_left, imagePoints_right,
          cameraMatrix_left, distCoeffs_left, cameraMatrix_right, distCoeffs_right, imageLeft_gray.size(), R, T, E, F,
          cvTermCriteria(CV_TERMCRIT_ITER+
          CV_TERMCRIT_EPS, 100, 1e-5),
          CV_CALIB_FIX_INTRINSIC +
          CV_CALIB_FIX_K1+ CV_CALIB_FIX_K2 + CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);
//    cout<<cameraMatrix_left.type()<<endl;

    //Mat R_matrix, R_vec, T_vec;
    //solvePnP(Mat(objectPoints), Mat(imagePoints_left), cameraMatrix_left, distCoeffs_left, R_vec, T_vec, false);

    //reconvert everything to float
    cameraMatrix_left.convertTo(cameraMatrix_left, CV_32F);
    cameraMatrix_right.convertTo(cameraMatrix_right, CV_32F);
    distCoeffs_left.convertTo(distCoeffs_left, CV_32F);
    distCoeffs_right.convertTo(distCoeffs_right, CV_32F);

    R.convertTo(R, CV_32F);
    T.convertTo(T, CV_32F);


    Mat extRT = Mat::eye(4,4,CV_32F);
    Mat extRT_R = extRT( Rect(0, 0, 3, 3) );
    R.copyTo(extRT_R);
    extRT.at<float>(0,3) = T.at<float>(0,0)/1000;
    extRT.at<float>(1,3) = T.at<float>(0,1)/1000;
    extRT.at<float>(2,3) = T.at<float>(0,2)/1000;

    //cout<<extRT<<endl;


   // F.convertTo(F, CV_32F);

   // cout<<F<<endl;

    Mat Fext = fundfromcameras(cameraMatrix_left, cameraMatrix_right, R, T);
    //cout<<Fext<<endl;

    F = Fext;

    //    Rodrigues(R_vec, R_matrix);

//    cout<<R_matrix<<endl;
 //   cout<<T_vec<<endl;

    //Apply a Kalman filter where the R matrix is converted to euler angles
    // Convert T and R to vector
    //     const Mat& correct(const Mat& measurement);
    // StatePost
    // Convert state to T and R matrix

  //  cout<<R<<endl<<endl;
  //  cout<<T<<endl<<endl;


  }

  // else If there is no checkboard in the scene
//  Ptr<FeatureDetector> detector = FeatureDetector::create("GFTT");
////  Ptr<FeatureDetector> detector = new GoodFeaturesToTrackDetector(1000, 0.04, 1., 3, false, 0.04 );
//  Ptr<DescriptorExtractor> descriptor = new SurfDescriptorExtractor( 6, 3, false );
//  Ptr<DescriptorMatcher> descriptorMatcher =  DescriptorMatcher::create( "BruteForce" /*"FlannBased"*/);
//
//  vector<KeyPoint> keypoints_left, keypoints_right;
//  Mat descr_left, descr_right;
//  vector<DMatch> matches;
//
//  //Convert image to graylevel
//  cvtColor(image_left, imageLeft_gray, CV_BGR2GRAY);
//  cvtColor(image_right, imageRight_gray, CV_BGR2GRAY);
//
//  detector->detect(imageLeft_gray, keypoints_left);
//  detector->detect(imageRight_gray, keypoints_right);
//  //Compute the descriptor for the left and the right Images (this is usefull for 3D reconstruction)
//  descriptor->compute(imageLeft_gray, keypoints_left, descr_left);
//  descriptor->compute(imageRight_gray, keypoints_right, descr_right);
//
//  descriptorMatcher->match(descr_left, descr_right, matches);
//
////  drawKeypoints(image_left,keypoints_left, image_left, CV_RGB(255,0,0));
////  drawKeypoints(image_right,keypoints_right, image_right, CV_RGB(255,0,0));
//  Mat resultImage;
//  drawMatches(image_left, keypoints_left, image_right, keypoints_right, matches, resultImage,CV_RGB(255,0,0), CV_RGB(0,0,255));

//  if(saveImages){
//    char numberImage[4];
//    stringstream filename;
//    sprintf(numberImage, "%.04d", frame);
//    filename<<"/home/migliore/Desktop/tmp/matching/match"<<numberImage<<".png";
//
//    cout<<"Saving Matching images"<<endl;
//    vector<int> params;
//    params.push_back(CV_IMWRITE_PNG_COMPRESSION);
//    bool answer =  imwrite(filename.str(), resultImage,params);
//  }

}


void CVUtils::triangulatePoint(float p_l_x,float p_l_y, float p_r_x, float p_r_y,
    Mat& R, Mat& T, Mat & dist_l, Mat & dist_r,
    Mat& cameraMatrix_l, Mat& cameraMatrix_r,
    Mat& point3DLeft, Mat& point3DRight) {


    Mat point_l_distort = (Mat_<float>(2,1) << (p_l_x - cameraMatrix_l.at<float>(0,2))/cameraMatrix_l.at<float>(0,0),
                                               (p_l_y - cameraMatrix_l.at<float>(1,2))/cameraMatrix_l.at<float>(1,1));
    Mat point_r_distort = (Mat_<float>(2,1) << (p_r_x - cameraMatrix_r.at<float>(0,2))/cameraMatrix_r.at<float>(0,0),
                                               (p_r_y - cameraMatrix_r.at<float>(1,2))/cameraMatrix_r.at<float>(1,1));


    Mat point_l_undist, point_r_undist;
    point_l_distort.copyTo(point_l_undist);
    point_r_distort.copyTo(point_r_undist);


    //Undistort points
    undistortPoint(point_l_undist, dist_l);
    undistortPoint(point_r_undist, dist_r);


    Mat points_l = (Mat_<float>(3,1) << point_l_undist.at<float>(0,0), point_l_undist.at<float>(1,0), 1);
    Mat points_r = (Mat_<float>(3,1) << point_r_undist.at<float>(0,0), point_r_undist.at<float>(1,0), 1);

    //cout<<"POINTS:"<<endl;
    //cout<< p_l_x <<" "<< p_l_y<<endl;
    //cout<< p_r_x <<" "<< p_r_y<<endl;
   // printMatrix(points_l);
   // printMatrix(points_r);

    //cout<<"MATRIX"<<endl;

//    %--- Extend the normalized projections in homogeneous coordinates
//    xt = [xt;ones(1,size(xt,2))];    points_l
//    xtt = [xtt;ones(1,size(xtt,2))]; points_r
//
//    %--- Number of points:
//    N = 1;
//
//    %--- Rotation matrix corresponding to the rigid motion between left and right cameras:
//    R = rodrigues(om);
//
//
//    %--- Triangulation of the rays in 3D space:
//
//    u = R * xt;
      Mat u = R*points_l;
 //     printMatrix(u);

//    n_xt2 = dot(xt,xt);
//    n_xtt2 = dot(xtt,xtt);


      float n_xt2 = points_l.dot(points_l);
      float n_xtt2 = points_r.dot(points_r);

//    T_vect = repmat(T, [1 N]);  //Since it is only a point T is ok
//
//    DD = n_xt2 .* n_xtt2 - dot(u,xtt).^2;

      float udot = u.dot(points_r);
//      cout<<udot<<endl;

      float DD = n_xt2*n_xtt2 - udot*udot;

//      cout<<DD<<endl;

//    dot_uT = dot(u,T_vect);
//    dot_xttT = dot(xtt,T_vect);
//    dot_xttu = dot(u,xtt);

      float dot_uT = u.dot(T);
      float dot_xttT = points_r.dot(T);
      float dot_xttu = u.dot(points_r);

//    NN1 = dot_xttu.*dot_xttT - n_xtt2 .* dot_uT;
//    NN2 = n_xt2.*dot_xttT - dot_uT.*dot_xttu;

      float NN1 = dot_xttu*dot_xttT - n_xtt2* dot_uT;
      float NN2 = n_xt2*dot_xttT - dot_uT*dot_xttu;

//      cout<<NN1<<" "<<NN2<<endl;

//    Zt = NN1./DD;
//    Ztt = NN2./DD;

      float Zt = NN1/DD;
      float Ztt = NN2/DD;
//
//    X1 = xt .* repmat(Zt,[3 1]);
//    X2 = R'*(xtt.*repmat(Ztt,[3,1])  - T_vect);

      Mat X1 = points_l*Zt;
      Mat X2 = R.t()* (points_r*Ztt - T);

//    %--- Left coordinates:
//    XL = 1/2 * (X1 + X2);

      point3DLeft = 0.5 * (X1+X2);

//    %--- Right coordinates:
//    XR = R*XL + T_vect;

      point3DRight = R*point3DLeft+T;


}

void CVUtils::triangulatePoint(Point2f pl, Point2f pr, Mat& point3DLeft, Mat& point3DRight){
  triangulatePoint(pl.x, pl.y, pr.x, pr.y, R, T, distCoeffs_left, distCoeffs_right, cameraMatrix_left, cameraMatrix_right, point3DLeft, point3DRight);
}


void CVUtils::undistortPoint(Mat &point, Mat &dist){
  //Undistort
  float k1 = dist.at<float>(0,0);
  float k2 = dist.at<float>(0,1);
  float k3 = dist.at<float>(0,4);
  float p1 = dist.at<float>(0,2);
  float p2 = dist.at<float>(0,3);

  Mat x;
  point.copyTo(x);            //                 % initial guess


  for (int kk=0; kk<20; kk++){

       //r_2 = sum(x.^2);
       float r_2 = x.at<float>(0,0)*x.at<float>(0,0)+x.at<float>(1,0)*x.at<float>(1,0);

       //k_radial =  1 + k1 * r_2 + k2 * r_2.^2 + k3 * r_2.^3;
       float k_radial = 1 + k1 * r_2 + k2 * r_2*r_2 + k3 * r_2*r_2*r_2;

       //delta_x = [2*p1*x(1,:).*x(2,:) + p2*(r_2 + 2*x(1,:).^2);
       Mat delta_x = (Mat_<float>(2,1) <<  2*p1*x.at<float>(0,0)*x.at<float>(1,0) + p2*(r_2 + 2*x.at<float>(0,0)*x.at<float>(0,0)),
                                           p1 * (r_2 + 2*x.at<float>(1,0)*x.at<float>(1,0))+2*p2*x.at<float>(0,0)*x.at<float>(1,0));

       //x = (xd - delta_x)./(ones(2,1)*k_radial);
       Mat tmp = (point - delta_x)/k_radial;
       tmp.copyTo(x);

  }

  x.copyTo(point);

}

void CVUtils::undistortPoint(Point2f &point, Mat &dist){
  //Undistort
  float k1 = dist.at<float>(0,0);
  float k2 = dist.at<float>(0,1);
  float k3 = dist.at<float>(0,4);
  float p1 = dist.at<float>(0,2);
  float p2 = dist.at<float>(0,3);

  Point2f x = Point2f(point.x, point.y);            //                 % initial guess


  for (int kk=0; kk<20; kk++){

       //r_2 = sum(x.^2);
       float r_2 = x.x*x.x+x.y*x.y;

       //k_radial =  1 + k1 * r_2 + k2 * r_2.^2 + k3 * r_2.^3;
       float k_radial = 1 + k1 * r_2 + k2 * r_2*r_2 + k3 * r_2*r_2*r_2;

       //delta_x = [2*p1*x(1,:).*x(2,:) + p2*(r_2 + 2*x(1,:).^2);
       Point2f delta_x = Point2f(  2*p1*x.x*x.y + p2*(r_2 + 2*x.x*x.x),
                                   p1 * (r_2 + 2*x.y*x.y)+2*p2*x.x*x.y);

       //x = (xd - delta_x)./(ones(2,1)*k_radial);
       Point2f tmp = Point2f( (point.x - delta_x.x)/k_radial, (point.y - delta_x.y)/k_radial);
       x = Point2f(tmp.x, tmp.y);

  }

  point = Point2f(x.x, x.y);

}

void CVUtils::colorSegmentation(Mat &image, Mat &m, Mat &icov, Mat &binaryImage){


  Mat imagetmp;
  cvtColor(image, imagetmp, CV_BGR2HSV_FULL);

  Mat resultingImage = Mat::zeros(image.rows, image.cols, CV_64FC1);
  for(int r = 0; r<imagetmp.rows; r++){
         for(int c = 0; c<imagetmp.cols; c++){

             Mat color2test = (Mat_<double>(3,1) << (int)imagetmp.at<Vec3b>(r,c)[0],(int)imagetmp.at<Vec3b>(r,c)[1],(int)imagetmp.at<Vec3b>(r,c)[2]);

             resultingImage.at<double>(r,c) = Mahalanobis(color2test, m, icov);
         }
  }

  //cout<<resultingImage<<endl;
  Mat dst;
  compare(resultingImage, 0.02, binaryImage, CMP_LE); //0.03


  Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3) );
  morphologyEx(binaryImage, binaryImage, CV_MOP_ERODE, element);
  morphologyEx(binaryImage, binaryImage, CV_MOP_ERODE, element);
  morphologyEx(binaryImage, binaryImage, CV_MOP_DILATE, element);
  morphologyEx(binaryImage, binaryImage, CV_MOP_DILATE, element);

}

void CVUtils::detectPossibleColoredObject(Mat &image, Mat& outputMask, vector<ColoredRect> &listofrect, Mat& outputImage){

  outputMask = Mat::zeros(image.rows, image.cols, CV_8UC1);

  int boxcounter = 0;
  for(uint cc = 0; cc<colorsStat.size(); cc+=2){

    Mat binaryImage;
    Scalar color;

    switch (cc) {
      case 0: color = CV_RGB(255,0,0);
        break;
      case 2: color = CV_RGB(0,255,0);
        break;
      case 4: color = CV_RGB(0,0,255);
        break;
      default:
        break;
    }

    colorSegmentation(image, colorsStat[cc], colorsStat[cc+1], binaryImage);

  // bitwise_or(outputMask, binaryImage, outputMask);

    vector<vector <Point> > contours_obj;
    vector<Vec4i> hierarchy_obj;

    findContours( binaryImage, contours_obj, hierarchy_obj, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    if( contours_obj.size() > 0 )
    {
       // iterate through all the top-level contours,
       for(int idx = 0; idx >= 0; idx = hierarchy_obj[idx][0] )
       {
          Rect objRect =  boundingRect( Mat(contours_obj[idx]) );

          //Draw contours only for debugging
         // drawContours( outputImage, contours_obj, idx, color, CV_FILLED, 8, hierarchy_obj );

          //Increase the BB to capture the background
          objRect = Rect(max(0,objRect.x-5), max(0,objRect.y-5), min(image.cols - objRect.x + 5, objRect.width+10), min(image.rows - objRect.y + 5, objRect.height+10));

          if(objRect.area() > 650){
              Mat roi(outputMask, objRect);
              roi =  Scalar(255);
              listofrect.push_back(ColoredRect(objRect, color, contours_obj[idx]));
              boxcounter++;
          }
       }
    }
  }


}



void CVUtils::refineMatches(
        const vector<KeyPoint>& keypoints_left,
        const vector<KeyPoint>& keypoints_right,
        vector<DMatch>& matches,
        vector<vector <DMatch> > &listOfMatches,
        double eps){


  vector<Point2f> points_left, points_right;
  KeyPoint::convert(keypoints_left, points_left);
  KeyPoint::convert(keypoints_right, points_right);
  vector<Vec3f> right_epilines;
  vector<Vec3f> left_epilines;

  computeCorrespondEpilines(Mat(points_left), 1, F, right_epilines);
  computeCorrespondEpilines(Mat(points_right), 2, F, left_epilines);

  matches.clear();

  for(uint i = 0; i < listOfMatches.size(); i++){

      vector<DMatch> localList = listOfMatches[i];
      Vec3f line2use = right_epilines[i];


      DMatch bestMatch;
    //  float minEpiDistance = FLT_MAX;

      for(uint j = 0; j < localList.size(); j++){

          DMatch possibleMatch = localList[j];
          Vec3f leftEpiline = left_epilines[possibleMatch.queryIdx];
          Point2f point2test = points_left[possibleMatch.trainIdx];

          float value = fabs(point2test.x*leftEpiline[0]+point2test.y*leftEpiline[1]+leftEpiline[2]);

          if(value<eps && possibleMatch.distance < bestMatch.distance){
              bestMatch = possibleMatch;
          }

      }

      if(bestMatch.queryIdx != -1 && bestMatch.trainIdx != -1 ){
          matches.push_back(bestMatch);
      }
  }

}

void CVUtils::findConstrainedCorrespondences(
              const vector<KeyPoint>& keypoints_left,
                const vector<KeyPoint>& keypoints_right,
                const Mat& descriptors_left,
                const Mat& descriptors_right,
                vector<DMatch>& matches,
                float eps, double ratio){

  vector<Point2f> points_left, points_right;
  KeyPoint::convert(keypoints_left, points_left);
  KeyPoint::convert(keypoints_right, points_right);
  vector<Vec3f> right_epilines;
  vector<Vec3f> left_epilines;

  computeCorrespondEpilines(Mat(points_left), 1, F, right_epilines);
  computeCorrespondEpilines(Mat(points_right), 2, F, left_epilines);



  for (uint i = 0; i < points_left.size(); i++){

      Vec3f line2use = right_epilines[i];
      Mat d_l = descriptors_left.row(i);

      double distMin = DBL_MAX;
      double distMax = -1;
      int idxright = -1;

      for(uint j = 0; j < points_right.size(); j++){

          Point2f point2test = points_right[j];
          Mat d_r = descriptors_right.row(j);

          float value = fabs(point2test.x*line2use[0]+point2test.y*line2use[1]+line2use[2]);

         // cout<<value<<endl;
          if(value<eps){

//              double dist = norm(d_l-d_r);

              //BRIEF distance
              Mat distanceMatrix;
              bitwise_xor(d_l, d_r, distanceMatrix);
              Scalar disttmp = sum(distanceMatrix);
              double dist = disttmp[0];


              distMax = max(dist, distMax);

              if(dist<distMin){ // && dist<0.4*distMax){
                  distMin = dist;
                  idxright = j;
              }

          }

      }

      if(distMin <= ratio*distMax && idxright != -1){
          matches.push_back(DMatch(i,idxright,distMin));
      }

  }


  /*ONLY FOR DEBUGGING
  for(uint i = 0; i < right_epilines.size(); i++){

      Vec3f line2plot = right_epilines[i];

      float x_borderleft = 0;
      float y_borderleft = -line2plot[2]/line2plot[1];
      if(y_borderleft<0){
          y_borderleft = 0;
          x_borderleft = -line2plot[2]/line2plot[0];
      }

      float x_borderright = imageLeft.cols;
      float y_borderright = (-line2plot[2]-line2plot[0]*x_borderright)/line2plot[1];
      if(y_borderleft > imageLeft.rows){
          y_borderleft = imageLeft.rows;
          x_borderleft = (-line2plot[2]-line2plot[1]*y_borderright)/line2plot[0];
      }

      Point2f pt1 = Point2f(x_borderleft, y_borderleft);
      Point2f pt2 = Point2f(x_borderright, y_borderright);

      line(outputImageRight, pt1, pt2, CV_RGB(255,0,0));
  }*/


}

void CVUtils::findConstrainedCorrespondences(
                      const vector<KeyPoint>& keypoints_left,
                      const vector<KeyPoint>& keypoints_right,
                      const Mat& imageLeft,
                      const Mat& imageRight,
                      vector<DMatch>& matches,
                      Size windowsize,
                      float threshold){

  vector<Point2f> points_left, points_right;
   KeyPoint::convert(keypoints_left, points_left);
   KeyPoint::convert(keypoints_right, points_right);
   vector<Vec3f> right_epilines;
   vector<Vec3f> left_epilines;

   computeCorrespondEpilines(Mat(points_left), 1, F, right_epilines);
   computeCorrespondEpilines(Mat(points_right), 2, F, left_epilines);


 /* ONLY FOR DEBUGGING*/
/*   for(uint i = 0; i < right_epilines.size(); i++){

       Vec3f line2plot = right_epilines[i];

       float x_borderleft = 0;
       float y_borderleft = -line2plot[2]/line2plot[1];
       if(y_borderleft<0){
           y_borderleft = 0;
           x_borderleft = -line2plot[2]/line2plot[0];
       }

       float x_borderright = imageLeft.cols;
       float y_borderright = (-line2plot[2]-line2plot[0]*x_borderright)/line2plot[1];
       if(y_borderleft > imageLeft.rows){
           y_borderleft = imageLeft.rows;
           x_borderleft = (-line2plot[2]-line2plot[1]*y_borderright)/line2plot[0];
       }

       Point2f pt1 = Point2f(x_borderleft, y_borderleft);
       Point2f pt2 = Point2f(x_borderright, y_borderright);

       line(outputImageRight, pt1, pt2, CV_RGB(255,0,0));
   }*/

   /*ONLY FOR DEBUGGING*
     for(uint i = 0; i < left_epilines.size(); i++){

         Vec3f line2plot = left_epilines[i];

         float x_borderleft = 0;
         float y_borderleft = -line2plot[2]/line2plot[1];
         if(y_borderleft<0){
             y_borderleft = 0;
             x_borderleft = -line2plot[2]/line2plot[0];
         }

         float x_borderright = imageLeft.cols;
         float y_borderright = (-line2plot[2]-line2plot[0]*x_borderright)/line2plot[1];
         if(y_borderleft > imageLeft.rows){
             y_borderleft = imageLeft.rows;
             x_borderleft = (-line2plot[2]-line2plot[1]*y_borderright)/line2plot[0];
         }

         Point2f pt1 = Point2f(x_borderleft, y_borderleft);
         Point2f pt2 = Point2f(x_borderright, y_borderright);

         line(outputImageLeft, pt1, pt2, CV_RGB(255,0,0));
     }*/


   for (uint i = 0; i < points_left.size(); i++){

       Point pleft = points_left[i];
       Vec3f line2use = right_epilines[i];
       Rect roileftRect( max(pleft.x - windowsize.width, 0),
                         max(pleft.y - windowsize.height, 0),
                         min(2*windowsize.width+1, imageLeft.cols - pleft.x ),
                         min(2*windowsize.height+1 , imageLeft.rows - pleft.y ));

       Mat templ_left(imageLeft, roileftRect);

       rectangle(outputImageLeft, roileftRect, CV_RGB(0,0,0));

       int jidx = -1;
       float maxCorrelation = FLT_MIN;

       for (uint j = 0; j < points_right.size(); j++){

           Point pright = points_right[j];


           float value = fabs(pright.x*line2use[0]+pright.y*line2use[1]+line2use[2]);

           if (value < threshold){

               Rect roirightRect(max(pright.x - windowsize.width, 0),
                                 max(pright.y- windowsize.height, 0),
                                 min(2*windowsize.width+1, imageRight.cols-pright.x ),
                                 min(2*windowsize.height+1, imageRight.rows-pright.y ));

               rectangle(outputImageRight, roirightRect, CV_RGB(0,0,0));


               Mat templ_right(imageRight, roirightRect);
               Mat result;

               if (roileftRect.area() > roirightRect.area()){
                   matchTemplate(templ_right, templ_left, result, CV_TM_SQDIFF);
               }
               else{
                   matchTemplate(templ_left, templ_right, result, CV_TM_SQDIFF);
               }


               if( result.at<float>(0,0) > maxCorrelation){

                   maxCorrelation = result.at<float>(0,0);
                   jidx = j;

               }

           }

       }

       if(jidx != -1 ){
           matches.push_back(DMatch(i,jidx, maxCorrelation));
       }
   }


}


void CVUtils::estimateDisparityImage(Mat& disparityImage){
  //Rectification

  Rect roi1, roi2;
  Mat Q;
  Size img_size = imageLeft.size();
  Mat R1, P1, R2, P2;

  Mat camleft, distleft, camright, distright;
  Mat Rtmp, Ttmp;

  cameraMatrix_left.convertTo(camleft,CV_64FC1);
  cameraMatrix_right.convertTo(camright,CV_64FC1);
  distCoeffs_left.convertTo(distleft,CV_64FC1);
  distCoeffs_right.convertTo(distright,CV_64FC1);
  R.convertTo(Rtmp, CV_64FC1);
  T.convertTo(Ttmp, CV_64FC1);

  stereoRectify( camleft, distleft, camright, distright, img_size, Rtmp, Ttmp, R1, R2, P1, P2, Q, -1, -1, img_size, &roi1, &roi2 );
  Mat map11, map12, map21, map22;
  initUndistortRectifyMap(cameraMatrix_left, distCoeffs_left, R1, P1, img_size, CV_16SC2, map11, map12);
  initUndistortRectifyMap(cameraMatrix_right, distCoeffs_right, R2, P2, img_size, CV_16SC2, map21, map22);

  Mat img1r, img2r, img1r_gray, img2r_gray,outLeft_rect,outRight_rect, outStereo_rect;
  remap(imageLeft, img1r, map11, map12, INTER_LINEAR);
  remap(imageRight, img2r, map21, map22, INTER_LINEAR);

  StereoSGBM sgbm;
  sgbm.preFilterCap = 63;
  sgbm.SADWindowSize = 1;

  int cn = img1r.channels();

  sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
  sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
  sgbm.minDisparity = 0;
  sgbm.numberOfDisparities = 64;
  sgbm.uniquenessRatio = 10;
  sgbm.speckleWindowSize = 100;
  sgbm.speckleRange = 32;
  sgbm.disp12MaxDiff = 1;
  sgbm.fullDP = true;

  Mat disp;
  //Mat img1p, img2p, dispp;
  //copyMakeBorder(img1, img1p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
  //copyMakeBorder(img2, img2p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);

  sgbm(img1r, img2r, disp);

  disp.convertTo(disparityImage, CV_8U, 255/(64*16.));

}

/*
 * Perform data association beween left and right camera
 */
int CVUtils::dataAssociation(vector<ColoredRect> &left_rectList, vector<ColoredRect> &right_rectList, vector<WorldObject> &object_list){

  //matches between blobs
  vector<DMatch> blobs_matches;

  //Iterate objects in left camera
  for(uint i = 0; i<left_rectList.size(); i++){

      ColoredRect currentLeftRect = left_rectList[i];

      Scalar colorleft = currentLeftRect.getColor();
      Mat leftobject = Mat(currentLeftRect.getContour());

      //Estimate shape moments
      Moments leftmoments = moments(Mat(currentLeftRect.getContour()));
      Point2f massCenter_left = Point2f(leftmoments.m10/leftmoments.m00, leftmoments.m01/leftmoments.m00);


      //Epipolar geometry
      vector<Vec3f> right_epilines;
      vector<Point2f> points_left;
      undistortPoint(massCenter_left, distCoeffs_left);
      points_left.push_back(massCenter_left);
      computeCorrespondEpilines(Mat(points_left), 1, F, right_epilines);

      Vec3f line2test = right_epilines[0];

      /*ONLY FOR DEBUG */
      for(uint ii = 0; ii < right_epilines.size(); ii++){

           Vec3f line2plot = right_epilines[ii];

           float x_borderleft = 0;
           float y_borderleft = -line2plot[2]/line2plot[1];
           if(y_borderleft<0){
               y_borderleft = 0;
               x_borderleft = -line2plot[2]/line2plot[0];
           }

           float x_borderright = imageLeft.cols;
           float y_borderright = (-line2plot[2]-line2plot[0]*x_borderright)/line2plot[1];
           if(y_borderleft > imageLeft.rows){
               y_borderleft = imageLeft.rows;
               x_borderleft = (-line2plot[2]-line2plot[1]*y_borderright)/line2plot[0];
           }

           Point2f pt1 = Point2f(x_borderleft, y_borderleft);
           Point2f pt2 = Point2f(x_borderright, y_borderright);

           line(outputImageRight, pt1, pt2, CV_RGB(255,0,0));
       }


      //A possible match is a point (mass center) belonging to the epiline
      double minDistance = DBL_MAX;
      double diffArea = -1;
      int idxright = -1;

      for(uint j = 0; j<right_rectList.size(); j++){

          ColoredRect currentRightRect = right_rectList[j];

          Scalar colorright = currentRightRect.getColor();

          //Compare colors
          if(colorleft[0] == colorright[0] && colorleft[1] == colorright[1] && colorleft[2] == colorright[2]){

              Moments rightmoments = moments(Mat(currentRightRect.getContour()));
              Point2f massCenter_right = Point2f(rightmoments.m10/rightmoments.m00, rightmoments.m01/rightmoments.m00);
              undistortPoint(massCenter_right, distCoeffs_right);

              //Estimate the distance from the epiline
              float dist2line = fabs(line2test[0]*massCenter_right.x + line2test[1]*massCenter_right.y + line2test[2]);

              //if distance is less then a threshold, the dimension are similar and the x and y position are not so far
              if (dist2line < 5 &&
                 // abs(currentLeftRect.getRect().width - currentRightRect.getRect().width)< 20 &&
                 // abs(currentLeftRect.getRect().height - currentRightRect.getRect().height)< 20 &&
                  abs(massCenter_left.x - massCenter_right.x)< 200 &&
                  abs(massCenter_left.y- massCenter_right.y) < 200){

                Mat rightobject = Mat(currentRightRect.getContour());
                double distance =  matchShapes(leftobject, rightobject, CV_CONTOURS_MATCH_I3, 0);

                //Match also the shape
                if(distance < minDistance){
                    minDistance = distance;
                    idxright = j;
                }
              }

          }

      }

      //If a matching is found
      if(idxright != -1){
    //    cout<<i<<" with "<<idxright<<" and distance "<<minDistance<<endl;
        char numberobj[2];
        stringstream left,right;
        sprintf(numberobj, "%d->%d", i, idxright);
        left<<numberobj;

        //Only for debugging
        putText(outputImageLeft, left.str(), currentLeftRect.getBBTopLeftCorner(), FONT_HERSHEY_DUPLEX , 0.5, CV_RGB(0,0,0));

        sprintf(numberobj, "%d->%d", idxright, i);
        right<<numberobj;
        //Only for debugging
        putText(outputImageRight, right.str(), right_rectList[idxright].getBBTopLeftCorner(), FONT_HERSHEY_DUPLEX , 0.5, CV_RGB(0,0,0));

        //Add the blob match in blobs
        blobs_matches.push_back(DMatch(idxright, i, minDistance)); //query first and after train
      }

  }

  //Now we have a list of pairs representing the matching
 // object_list.clear(); // TODO remove it!
  int newobjs = 0;

  //We can start to work on objects
  vector<Point3f> listof3Dpoints;
  for(uint i = 0; i<blobs_matches.size(); i++){

      ColoredRect leftbb = left_rectList[blobs_matches[i].trainIdx];
      ColoredRect rightbb = right_rectList[blobs_matches[i].queryIdx];

      Moments leftmoments = moments(Mat(leftbb.getContour()));
      Point2d massCenter_left = Point2d(leftmoments.m10/leftmoments.m00, leftmoments.m01/leftmoments.m00);

      Moments rightmoments = moments(Mat(rightbb.getContour()));
      Point2d massCenter_right = Point2d(rightmoments.m10/rightmoments.m00, rightmoments.m01/rightmoments.m00);

      Mat point3dLeft, point3dRight;

 //     double orientation = 180*(0.5*atan(2*leftmoments.mu11/(leftmoments.mu20 - leftmoments.mu02)))/CV_PI;

      RotatedRect leftRRect = leftbb.getRotatedRect();
      RotatedRect rightRRect = rightbb.getRotatedRect();

      Size2f areaDist = leftRRect.size - rightRRect.size;
      double areaL2Dist = abs(areaDist.width*areaDist.width - areaDist.height*areaDist.height);

      //This triangulate points is based on center mass (TODO improve it)
     //  cout<<"Center left "<< massCenter_left.x<<", "<<massCenter_left.y<<" - "<< massCenter_right.x<< ", "<<massCenter_right.y<<endl;
      triangulatePoint(massCenter_left, massCenter_right, point3dLeft, point3dRight);




      Point3f poseWrtLeft;
      poseWrtLeft.x = point3dLeft.at<float>(0,0);
      poseWrtLeft.y = point3dLeft.at<float>(1,0);
      poseWrtLeft.z = point3dLeft.at<float>(2,0);

      Mat rotatedpoint3Dpoint;
      Point3f poseWrtWorld;


      //If we have the rototraslation from Left to World
      if( RT_left.empty() ){
    	  cout<<"ERROR"<<endl;
      }

      Mat hompoint3dLeft = (Mat_<float>(4,1) << point3dLeft.at<float>(0,0)/1000, point3dLeft.at<float>(1,0)/1000 , point3dLeft.at<float>(2,0)/1000 , 1);
      rotatedpoint3Dpoint = RT_left*hompoint3dLeft;
      // cout<<rotatedpoint3Dpoint<<endl;

      poseWrtWorld.x = rotatedpoint3Dpoint.at<float>(0,0)/rotatedpoint3Dpoint.at<float>(3,0);
      poseWrtWorld.y = rotatedpoint3Dpoint.at<float>(1,0)/rotatedpoint3Dpoint.at<float>(3,0);
      poseWrtWorld.z = rotatedpoint3Dpoint.at<float>(2,0)/rotatedpoint3Dpoint.at<float>(3,0);



      //only for debugging
      cout<<"Estimated points left blob "<<blobs_matches[i].trainIdx<<" position wrt world"<<poseWrtWorld<<endl;

      /** inizio test **/
       Mat point3dLeftnew, point3dRightnew;
       triangulatePointNew(massCenter_left, massCenter_right, point3dLeftnew, point3dRightnew);

       //*** fine tst **/

      //Data association between stored object and new objects founded
      bool associated = false;
      double mindistance = DBL_MAX;
      int indexoo= -1;

      for(uint oo = 0; oo < object_list.size(); oo++){

          //update invisibility counter
          object_list[oo].updateNotchangedCounter();

          //associate old object with new measurements

         // cout << object_list[oo].getObjPosition().x << " " << object_list[oo].getObjPosition().y << " " << object_list[oo].getObjPosition().z << endl;
          //
          double distvalue = object_list[oo].matchColoredRects(leftbb, rightbb, poseWrtWorld);
          if( distvalue < mindistance){
              mindistance = distvalue;
              indexoo = oo;
          }
          //mindistance = min(mindistance, distvalue);
         // cout<<distvalue<<endl; TODO change it with the position prediction

      }

      //TODO this data association is one way! Is based on a distance between the stored and the current object, cosindering also the colors
      if (mindistance < 0.1 && 	  poseWrtWorld.x < 0 && poseWrtWorld.x > -1){
          associated = true;
          //Only for debugging
          cout<<"Associated with "<<object_list[indexoo].getId()<<" with distance "<<mindistance<<endl;
          //Update the pose
          object_list[indexoo].changeData(leftbb, rightbb, poseWrtWorld);
      }

      //If there are no associations create a new object
      if( !associated && !RT_left.empty() && poseWrtWorld.x < 0 && poseWrtWorld.x > -1){
          cout<<"Not associated because distance is "<<mindistance<<" , I am creating the new object "<<objId<<endl;
          object_list.push_back(WorldObject(leftbb, rightbb, objId, poseWrtWorld));
          objId++;
          newobjs++;
      }

      if (poseWrtWorld.x > 0 || poseWrtWorld.x < -1){
    	  cout<<"*** Object rejected because its pose is "<<poseWrtWorld<<" id "<<objId<<endl;
      }

  }

  return newobjs;

}

void CVUtils::setUpImageStereoImages(Mat& imageL, Mat& imageR){
  imageLeft = imageL;
  imageRight = imageR;

  //Convert images to Graylevel
  cvtColor(imageLeft, imageLeft_gray, CV_BGR2GRAY);
  cvtColor(imageRight, imageRight_gray, CV_BGR2GRAY);


  if(useUndistImages){
    //Undistort images
    undistort(imageLeft, imageLeft_undist, cameraMatrix_left, distCoeffs_left);
    undistort(imageRight, imageRight_undist, cameraMatrix_right, distCoeffs_right);

    imageLeft = imageLeft_undist;
    imageRight = imageRight_undist;

  }

  //Copy images to allow visualization
  imageLeft.copyTo(outputImageLeft);
  imageRight.copyTo(outputImageRight);

  if( recomputeF ){
      fundfromimages(cameraMatrix_left, cameraMatrix_right);
  }

}

void CVUtils::setUpCamera2World(Mat& left2world, Mat& right2world){

  RT_left = left2world;
  RT_right = right2world;
  iRT_left = left2world.inv();
  iRT_right = right2world.inv();

  //Create camera matrix

  P_left = Mat::zeros(3,4, iRT_left.type());

  Mat iR_left = iRT_left(Rect(0,0,3,3));
  Mat iT_left =  iRT_left(Rect(3,0,1,3));
  Mat P_left_tmpR = P_left(Rect(0,0,3,3));
  Mat P_left_tmpT = P_left(Rect(3,0,1,3));
  Mat tmplR = cameraMatrix_left*iR_left;
  Mat tmplT = cameraMatrix_left*iT_left;
  tmplR.copyTo(P_left_tmpR);
  tmplT.copyTo(P_left_tmpT);

  P_right = Mat::zeros(3,4,iRT_right.type());

  Mat iR_right = iRT_right(Rect(0,0,3,3));
  Mat iT_right =  iRT_right(Rect(3,0,1,3));
  Mat P_right_tmpR = P_right(Rect(0,0,3,3));
  Mat P_right_tmpT = P_right(Rect(3,0,1,3));
  Mat tmprR = cameraMatrix_right*iR_right;
  Mat tmprT = cameraMatrix_right*iT_right;
  tmprR.copyTo(P_right_tmpR);
  tmprT.copyTo(P_right_tmpT);

}


bool CVUtils::saveImage(Mat& image2save, string directory, int framecounter){
  char numberImage[6];
  stringstream filename;
  sprintf(numberImage, "%.06d", framecounter);
  filename<<directory<<"im_"<<numberImage<<".ppm";

  vector<int> params;
  params.push_back(CV_IMWRITE_PXM_BINARY);
  return imwrite(filename.str(), image2save ,params);
}

/*
 * Detect colored objects
 */
int CVUtils::detectObjects(vector<WorldObject> &obj_list){

  int numberOfNewObject = 0;
  if(imageLeft.empty() || imageRight.empty()){
      return 0;
  }

  //Only for debug
  //  object_list.clear();

  //List of possible object represented as a rectangle with color
  vector<ColoredRect> left_rectList, right_rectList;
  Mat maskLeft, maskRight;

  //Detect colore object in left and right camera
  detectPossibleColoredObject(imageLeft, maskLeft, left_rectList, outputImageLeft);
  cout<<"Found "<<left_rectList.size()<<" possible objects in left image"<<endl;
  detectPossibleColoredObject(imageRight, maskRight, right_rectList, outputImageRight);
  cout<<"Found "<<right_rectList.size()<<" possible objects in right image"<<endl;

  //Perform data association between left and right camera
  numberOfNewObject =  dataAssociation(left_rectList, right_rectList, obj_list);

  //For each object estimate position and orientation
  for(uint oo = 0; oo<obj_list.size(); oo++){

      bool isASphere = true;
      char numberobj[2];
      stringstream left,right;
      sprintf(numberobj, "%.02d", obj_list[oo].getId());

      //left image
      left<<numberobj;

      ColoredRect leftbb = obj_list[oo].getColoredRect('l');
      RotatedRect leftRRect = leftbb.getRotatedRect();

     // findObjectContour(imageLeft, imageLeft_gray, leftbb, outputImageLeft);

      float angle =  leftRRect.angle;
      float height = leftRRect.size.height;
      float width = leftRRect.size.width;
      float cosangle = cos((angle/180)*CV_PI);
      float sinangle = sin((angle/180)*CV_PI);

      if(abs(height - width) > 20)
        isASphere = false;


      //Divide the h and w
      height = height/2;
      width = width/2;


      //define 4 points
      Point2f pt1 = Point2f(-width * cosangle +  leftRRect.center.x, -width * sinangle +  leftRRect.center.y);
      Point2f pt2 = Point2f(width * cosangle +  leftRRect.center.x, width * sinangle +  leftRRect.center.y);
      Point2f pt3 = Point2f(height * sinangle +  leftRRect.center.x, -height * cosangle +  leftRRect.center.y);
      Point2f pt4 = Point2f(-height * sinangle +  leftRRect.center.x, height * cosangle +  leftRRect.center.y);


      line(outputImageLeft, pt1, pt2, leftbb.getColor());
      line(outputImageLeft, pt3, pt4, leftbb.getColor());
      ellipse(outputImageLeft, leftRRect, leftbb.getColor());
      rectangle(outputImageLeft, leftbb.getRect(), leftbb.getColor());
      putText(outputImageLeft, left.str(), leftbb.getBBCenter(), FONT_HERSHEY_DUPLEX , 0.5, leftbb.getColor());

      //Right Image
      right<<numberobj;

      ColoredRect rightbb = obj_list[oo].getColoredRect('r');
      RotatedRect rightRRect = rightbb.getRotatedRect();

      angle =  rightRRect.angle;
      height = rightRRect.size.height;
      width = rightRRect.size.width;
      cosangle = cos((angle/180)*CV_PI);
      sinangle = sin((angle/180)*CV_PI);

      //Divide the h and w
      height = height/2;
      width = width/2;

      //define 4 points
      pt1 = Point2f(-width * cosangle +  rightRRect.center.x, -width * sinangle +  rightRRect.center.y);
      pt2 = Point2f(width * cosangle +  rightRRect.center.x, width * sinangle +  rightRRect.center.y);
      pt3 = Point2f(height * sinangle +  rightRRect.center.x, -height * cosangle +  rightRRect.center.y);
      pt4 = Point2f(-height * sinangle +  rightRRect.center.x, height * cosangle +  rightRRect.center.y);

      line(outputImageRight, pt1, pt2, rightbb.getColor());
      line(outputImageRight, pt3, pt4, rightbb.getColor());
      ellipse(outputImageRight, rightRRect, rightbb.getColor());
      rectangle(outputImageRight, rightbb.getRect(), rightbb.getColor());
      putText(outputImageRight, right.str(), rightbb.getBBCenter(), FONT_HERSHEY_DUPLEX , 0.5, rightbb.getColor());

      if(abs(height - width) > 20)
        isASphere = false;

      if(!isASphere){
          obj_list[oo].setShape(CYLINDER);
      }
      else{
          obj_list[oo].setShape(SPHERE);
      }


  }

  return numberOfNewObject;

}


bool CVUtils::saveCalibrationMatrix(){
  FileStorage fs("calibrationMatrices.yaml", CV_STORAGE_WRITE);
  if (!fs.isOpened())
    return false;


  fs<<"Intrinsics_left"<<cameraMatrix_left;
  fs<<"Intrinsics_right"<<cameraMatrix_right;
  fs<<"Distorsion_left"<<distCoeffs_left;
  fs<<"Distorsion_right"<<distCoeffs_right;

  fs<<"R"<<R;
  fs<<"T"<<T;
  fs<<"F"<<F;

  fs.release();
}

bool CVUtils::loadCalibrationMatrix(){

   FileStorage fs( "calibrationMatrices.yaml", CV_STORAGE_READ );

   fs["Intrinsics_left"]>>cameraMatrix_left;
   fs["Intrinsics_right"]>>cameraMatrix_right;
   fs["Distorsion_left"]>>distCoeffs_left;
   fs["Distorsion_right"]>>distCoeffs_right;

   fs["R"]>>R;
   fs["T"]>>T;
   fs["F"]>>F;

   fs.release();

   return true;
}

bool CVUtils::loadColorStats(){

  /*Color statistics*/
    FileStorage fs( "colorMatrices.yaml", CV_STORAGE_READ );
    int colorsDim;

    colorsStat.clear();

    fs["NumberOfColors"] >> colorsDim;

    cout<<"Number of learned colors "<<colorsDim<<endl;

    char numberColor[4];

    for(int i=0; i<2*colorsDim; i+=2){
        Mat meanColor, covColor, icovColor;
        int numCol = i/2;
        stringstream meanName, covName;
        sprintf(numberColor, "%.04d", numCol);
        meanName<<"meanMatrix"<<numberColor;
        covName<<"covMatrix"<<numberColor;
        fs[meanName.str()] >> meanColor;
        fs[covName.str()] >> covColor;
        //Save the inverse of the covariance matrix
        icovColor = covColor.inv();

        colorsStat.push_back(meanColor);
        colorsStat.push_back(icovColor);
    }
    fs.release();

    return true;
}

void CVUtils::findObjectContour(Mat &image, Mat &graylevelimage, ColoredRect &rect, Mat &outputImage){

  //Select the subimage
  Mat localGrayImage(graylevelimage,rect.getRect());

  Mat edges;
  Canny(localGrayImage, edges, 1, 3, 3, false);

  saveImage(edges, "/home/migliore/Desktop/tmp/test/", 0);

  //
  Mat whiteMask = 255*Mat::ones(image.size(), edges.type());
  Mat localWhiteMask(whiteMask, rect.getRect());
  edges.copyTo(localWhiteMask);

  image.copyTo(outputImage, whiteMask);

}

void CVUtils::triangulatePointNew(Point2f pl, Point2f pr, Mat& point3DLeft, Mat& point3DRight){

//  Mat P3_left = P_left.row(2);
//  Mat P2_left = P_left.row(1);
//  Mat P1_left = P_left.row(0);
//
//  Mat P3_right = P_right.row(2);
//  Mat P2_right = P_right.row(1);
//  Mat P1_right = P_right.row(0);
//
//  Mat A = Mat::zeros(4,4, P_left.type());
//
//  Mat A1 = A.row(0); //A(Rect(0,0,3,1));
//  Mat A2 = A.row(1);
//  Mat A3 = A.row(2);
//  Mat A4 = A.row(3);
//
//  A1 = P3_left*pl.x - P1_left;
//  A2 = P3_left*pl.y - P2_left;
//  A3 = P3_right*pr.x - P1_right;
//  A4 = P3_right*pr.y - P2_right;
//
//  A1 = A1/norm(A1);
//  A2 = A2/norm(A2);
//  A3 = A3/norm(A3);
//  A4 = A4/norm(A4);
//
//  Mat A2solve = A.t()*A;
//
//  Mat eigenvalue, eigenvector;
//  int highindex, lowindex;
//  highindex = 0;
//  lowindex = 0;
//
//  eigen(A2solve,eigenvalue,eigenvector, lowindex, highindex);
//
//  cout<<eigenvalue<<endl;
//  cout<<eigenvector<<endl;
//  cout<<highindex<<" "<<lowindex<<endl;
//  Mat result = eigenvector.row(highindex - lowindex + 1)
//  cout<<eigenvector.row(highindex - lowindex + 1)<<endl;

  //TODO test

//  //Estimate a point
//  Mat matpleft = (Mat_<float>(3,1) << pl.x, pl.y, 1);
//  Mat dirleft = cameraMatrix_left.inv()*matpleft;
//  dirleft = dirleft/norm(dirleft);
//  Mat matpright = (Mat_<float>(3,1) << pr.x, pr.y, 1);
//  Mat dirright = cameraMatrix_right.inv()*matpright;
//  dirright = dirright/norm(dirright);
//
//  Mat center = Mat::zeros(3,1, CV_32FC1);
//
//  //Center of the camera
//  Mat pl_1 = RT_left*center;
//  Mat pl_2 = RT_left*dirleft;
//  Mat pr_1 = RT_right*center;
//  Mat pr_2 = RT_right*dirleft;
//
//  float U = pl_2.at<float>(0,0);
//  float V = pl_2.at<float>(1,0);
//  float W = pl_2.at<float>(2,0);
//  float T = 1;
//
//  //Definition of Pi(X_2) matrix
//  Mat Pi_X = (Mat_<float>(6,4) <<  1,  0,  0, -U,
//                                 0,  1,  0, -V,
//                                 0,  0,  1, -W,
//                                 0, -W,  V,  0,
//                                 W,  0, -U,  0,
//                                -V,  U,  0,  0);
//
//  Mat line_left = Pi_X*pl_1;
//
//  U = pr_2.at<float>(0,0);
//  V = pr_2.at<float>(1,0);
//  W = pr_2.at<float>(2,0);
//  T = 1;
//
//  //Definition of Pi(X_2) matrix
//  Pi_X = (Mat_<float>(6,4) <<  1,  0,  0, -U,
//                               0,  1,  0, -V,
//                               0,  0,  1, -W,
//                               0, -W,  V,  0,
//                               W,  0, -U,  0,
//                              -V,  U,  0,  0);
//
//  Mat line_right = Pi_X*pr_1;


  //Another test in the hat

  Mat matpleft = (Mat_<float>(2,1) << pl.x, pl.y);
  Mat matpright = (Mat_<float>(2,1) << pr.x, pr.y);

  CvMat __points1 = matpleft, __points2 = matpright;

  CvMat* P1 = &(CvMat)P_left;
  CvMat* P2 = &(CvMat)P_right;

  float _d[1000] = {0.0f};
  Mat outTM(4,1,CV_32FC1,_d);
  CvMat* out = &(CvMat)outTM;

  //using cvTriangulate with the created structures
   cvTriangulatePoints(P1,P2,&__points1,&__points2,out);

   cout<<outTM<<endl;
}
