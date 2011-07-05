/*
 * GeoUtils.cpp
 *
 *  Created on: May 25, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#include "GeoUtils.h"

GeoUtils::GeoUtils()
{
  // TODO Auto-generated constructor stub

}

GeoUtils::~GeoUtils()
{
  // TODO Auto-generated destructor stub
}


void GeoUtils::setUpCameraTransformation(Mat& left2world, Mat& right2world){

  RT_l2w = left2world;
  RT_r2w = right2world;

 // cout<<RT_left<<endl;

  RT_w2l = left2world.inv();
  RT_w2r = right2world.inv();

  //Create the camera matrix 3x4

  //Left camera is the reference frame
  P_left = Mat::zeros(3,4, cameraMatrix_left.type());
  //Mat iR_left = iRT_left(Rect(0,0,3,3));
  //Mat iT_left =  iRT_left(Rect(3,0,1,3));
  Mat iR_left = Mat::eye(3,3, cameraMatrix_left.type());
  Mat iT_left =  Mat::zeros(3,1, cameraMatrix_left.type());
  Mat P_left_tmpR = P_left(Rect(0,0,3,3));
  Mat P_left_tmpT = P_left(Rect(3,0,1,3));
  Mat tmplR = cameraMatrix_left*iR_left;
  Mat tmplT = cameraMatrix_left*iT_left;
  tmplR.copyTo(P_left_tmpR);
  tmplT.copyTo(P_left_tmpT);

  //Right camera wrt left camera
  P_right = Mat::zeros(3,4,cameraMatrix_right.type());
  //Mat iR_right = iRT_right(Rect(0,0,3,3));
  //Mat iT_right =  iRT_right(Rect(3,0,1,3)); //Convertion from m->mm ?
  Mat iR_right = R.clone();
  Mat iT_right = T.clone();
  Mat P_right_tmpR = P_right(Rect(0,0,3,3));
  Mat P_right_tmpT = P_right(Rect(3,0,1,3));
  Mat tmprR = cameraMatrix_right*iR_right;
  Mat tmprT = cameraMatrix_right*iT_right;
  tmprR.copyTo(P_right_tmpR);
  tmprT.copyTo(P_right_tmpT);


}


Mat GeoUtils::fundfromcameras(){

  Mat invKl = cameraMatrix_left.inv();
  Mat invKr = cameraMatrix_right.inv();
  Mat invKrT = invKr.t();

  double t1 = T.at<float>(0,0), t2 = T.at<float>(1,0), t3 = T.at<float>(2,0);

  Mat tx = (Mat_<float>(3,3) << 0, -t3, t2, t3, 0, -t1, -t2, t1, 0);

  Mat F = invKrT*tx*R*invKl;

  return F;
}



Mat GeoUtils::fundfromimages(){

  F  = fundfromcameras();
  return F;


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

void GeoUtils::estimateRTFromImages(){

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
    Mat Rlefttmp, Rlefttmp_vec;
    Mat Tlefttmp;

    //Solve ransac
    solvePnPRansac(singleObjectPoints, corners_left, cameraMatrix_left, distCoeffs_left, Rlefttmp_vec, Tlefttmp);

    //reconvert everything to float
    cameraMatrix_left.convertTo(cameraMatrix_left, CV_32F);
    cameraMatrix_right.convertTo(cameraMatrix_right, CV_32F);
    distCoeffs_left.convertTo(distCoeffs_left, CV_32F);
    distCoeffs_right.convertTo(distCoeffs_right, CV_32F);

    R.convertTo(R, CV_32F);
    T.convertTo(T, CV_32F);


    Rodrigues(Rlefttmp_vec, Rlefttmp);
    cout<<Rlefttmp<<endl;
    cout<<Tlefttmp<<endl;
    //This transforms a point in checkboard coordination to camera coordinates
    Mat extRT_left = Mat::eye(4,4,CV_32F);

    for(int i = 0; i<3; i++)
        for(int j = 0; j<3; j++)
                extRT_left.at<float>(i,j) = (float)Rlefttmp.at<double>(i,j);

    extRT_left.at<float>(0,3) = (float)Tlefttmp.at<double>(0,0);
    extRT_left.at<float>(1,3) = (float)Tlefttmp.at<double>(1,0);
    extRT_left.at<float>(2,3) = (float)Tlefttmp.at<double>(2,0);

    //cout<<extRT_left<<endl;
    //From camera to checkboard
    RT_l2c = extRT_left.inv();
    RT_l2c.convertTo(RT_l2c, CV_32F);

  }
}

void GeoUtils::triangulatePoint(float p_l_x,float p_l_y, float p_r_x, float p_r_y, Mat& point3DLeft, Mat& point3DRight) {


    Mat point_l_distort = (Mat_<float>(2,1) << (p_l_x - cameraMatrix_left.at<float>(0,2))/cameraMatrix_left.at<float>(0,0),
                                               (p_l_y - cameraMatrix_left.at<float>(1,2))/cameraMatrix_left.at<float>(1,1));
    Mat point_r_distort = (Mat_<float>(2,1) << (p_r_x - cameraMatrix_right.at<float>(0,2))/cameraMatrix_right.at<float>(0,0),
                                               (p_r_y - cameraMatrix_right.at<float>(1,2))/cameraMatrix_right.at<float>(1,1));


    Mat point_l_undist, point_r_undist;
    point_l_distort.copyTo(point_l_undist);
    point_r_distort.copyTo(point_r_undist);


    //Undistort points
    undistortPoint(point_l_undist, distCoeffs_left);
    undistortPoint(point_r_undist, distCoeffs_right);


    Mat points_l = (Mat_<float>(3,1) << point_l_undist.at<float>(0,0), point_l_undist.at<float>(1,0), 1);
    Mat points_r = (Mat_<float>(3,1) << point_r_undist.at<float>(0,0), point_r_undist.at<float>(1,0), 1);

//    %--- Triangulation of the rays in 3D space:
      Mat u = R*points_l;

      float n_xt2 = points_l.dot(points_l);
      float n_xtt2 = points_r.dot(points_r);
      float udot = u.dot(points_r);
      float DD = n_xt2*n_xtt2 - udot*udot;
      float dot_uT = u.dot(T);
      float dot_xttT = points_r.dot(T);
      float dot_xttu = u.dot(points_r);
      float NN1 = dot_xttu*dot_xttT - n_xtt2* dot_uT;
      float NN2 = n_xt2*dot_xttT - dot_uT*dot_xttu;
      float Zt = NN1/DD;
      float Ztt = NN2/DD;

      Mat X1 = points_l*Zt;
      Mat X2 = R.t()* (points_r*Ztt - T);

//    %--- Left coordinates:
      point3DLeft = 0.5 * (X1+X2);

//    %--- Right coordinates:
      point3DRight = R*point3DLeft+T;


}

void GeoUtils::triangulatePoint(Point2f pl, Point2f pr, Mat& point3DLeft, Mat& point3DRight){
  triangulatePoint(pl.x, pl.y, pr.x, pr.y, point3DLeft, point3DRight);
}


void GeoUtils::undistortPoint(Mat &point, Mat &dist){
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

void GeoUtils::undistortPoint(Point2f &point, Mat &dist){
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

void GeoUtils::triangulatePointNew(Point2f pl, Point2f pr, Point3f& point3D){

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
//  Mat dirleft = (Mat_<float>(4,1) << (pl.x- cameraMatrix_left.at<float>(0,2))/cameraMatrix_left.at<float>(0,0), (pl.y- cameraMatrix_left.at<float>(1,2))/cameraMatrix_left.at<float>(1,1), 1/sqrt(cameraMatrix_left.at<float>(0,0)*cameraMatrix_left.at<float>(0,0)+cameraMatrix_left.at<float>(1,1)+cameraMatrix_left.at<float>(1,1)), 1 );
//  Mat dirright = (Mat_<float>(4,1) << (pr.x- cameraMatrix_right.at<float>(0,2))/cameraMatrix_right.at<float>(0,0), (pr.y- cameraMatrix_right.at<float>(1,2))/cameraMatrix_right.at<float>(1,1), 1/sqrt(cameraMatrix_right.at<float>(0,0)*cameraMatrix_right.at<float>(0,0)+cameraMatrix_right.at<float>(1,1)+cameraMatrix_right.at<float>(1,1)), 1 );
//
//  cout<<dirleft<<endl;
//  cout<<dirright<<endl;
//
//  Mat center = Mat::zeros(4,1, CV_32FC1);
//  center.at<float>(3,0) = 1;
//
//   //Center of the camera
//  Mat pl_1 = RT_left*center;
//  Mat pl_2 = RT_left*dirleft;
//  Mat pr_1 = RT_right*center;
//  Mat pr_2 = RT_right*dirright;
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
//  Mat pl_h = (Mat_<float>(4,1) << pl_1.at<float>(0,0), pl_1.at<float>(1,0), pl_1.at<float>(2,0), 1);
//  Mat line_left = Pi_X*pl_h;
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
//  Mat pr_h = (Mat_<float>(4,1) << pr_1.at<float>(0,0), pr_1.at<float>(1,0), pr_1.at<float>(2,0), 1);
//
//  Mat line_right = Pi_X*pr_h;
//
//  cout<<pl_1<<endl;
//  cout<<pl_2<<endl;
//  cout<<pr_1<<endl;
//  cout<<pr_2<<endl;

  //Another test in the hat

  undistortPoint(pl, distCoeffs_left);
  undistortPoint(pr, distCoeffs_right);

  Mat matpleft = (Mat_<float>(2,1) << pl.x, pl.y);
  Mat matpright = (Mat_<float>(2,1) << pr.x, pr.y);

  Mat doubleleft, doubleright;

  matpleft.convertTo(doubleleft, CV_64FC1);
  matpright.convertTo(doubleright, CV_64FC1);

  CvMat __points1 = doubleleft, __points2 = doubleright;
  CvMat __newpoints1 = doubleleft, __newpoints2 = doubleright;

  Mat doubleP_left, doubleP_right;
  P_left.convertTo(doubleP_left, CV_64FC1);
  P_right.convertTo(doubleP_right, CV_64FC1);

  CvMat* P1 = &(CvMat)doubleP_left;
  CvMat* P2 = &(CvMat)doubleP_right;

  float _d[1000] = {0.0f};
  Mat outTM(4,1,CV_64FC1,_d);
  CvMat* out = &(CvMat)outTM;

  //using cvTriangulate with the created structures
  //cvCorrectMatches(&(CvMat)F, &__points1, &__points2, &__newpoints1, &__newpoints2);

  cvTriangulatePoints(P1,P2,&__points1,&__points2,out);

  cout<<matpleft<<endl;
  cout<<matpright<<endl;
  outTM = outTM/outTM.at<double>(3,0);

  Mat tmpoutTM = outTM.clone();

  outTM.at<double>(0,0)/=1000;
  outTM.at<double>(1,0)/=1000;
  outTM.at<double>(2,0)/=1000;

  cout<<outTM<<endl;

  outTM.convertTo(outTM, CV_32FC1);
  Mat result = RT_l2w*outTM;
  cout<<result<<endl;

  //ok... now the position wrt checkboard

  tmpoutTM.convertTo(tmpoutTM, CV_32FC1);
  cout<<tmpoutTM<<endl;
  Mat result2 = RT_l2c*tmpoutTM;

  cout<<result2<<endl;

  result2.at<float>(0,0)/=1000;
  result2.at<float>(1,0)/=1000;
  result2.at<float>(2,0)/=1000;

  Mat finalresult = result2/result2.at<float>(3,0);
  cout<<finalresult<<endl;


  Mat convertedresults =  (Mat_<float>(3,1) << finalresult.at<float>(2,0)-0.85, finalresult.at<float>(1,0)-0.24, -(finalresult.at<float>(0,0)-0.04));
  cout<<convertedresults<<endl;

  point3D.x = finalresult.at<float>(2,0)-0.91; //83cm
  point3D.y = finalresult.at<float>(1,0)-0.24; //24 cm
  point3D.z = -(finalresult.at<float>(0,0)-0.01); //4cm
}

bool GeoUtils::saveCalibrationMatrix(){
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

bool GeoUtils::loadCalibrationMatrix(){

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

void GeoUtils::lrDataAssociation(vector<ColoredRect> &l_listofrect, vector<ColoredRect> &r_listofrect, vector<DMatch> &matching, float epipolarDistance){
  //matches between blobs
  vector<DMatch> blobs_matches;

  //Iterate objects in left camera
  for(uint i = 0; i<l_listofrect.size(); i++){

      ColoredRect currentLeftRect = l_listofrect[i];

      Scalar colorleft = currentLeftRect.getColor();
      Mat leftobject = Mat(currentLeftRect.getContour());

      //Estimate shape moments
      Moments leftmoments = moments(Mat(currentLeftRect.getContour()));
      Point2f massCenter_left = Point2f(leftmoments.m10/leftmoments.m00, leftmoments.m01/leftmoments.m00);


      //Epipolar geometry
      vector<Vec3f> right_epilines;
      vector<Point2f> points_left;

      //undistort mass center
      undistortPoint(massCenter_left, distCoeffs_left);
      points_left.push_back(massCenter_left);

      //Estimate Epipolar Line
      computeCorrespondEpilines(Mat(points_left), 1, F, right_epilines);
      Vec3f line2test = right_epilines[0];

      //A possible match is a point (mass center) belonging to the Epipolar line
      double minDistance = DBL_MAX;
      double diffArea = -1;
      int idxright = -1;

      for(uint j = 0; j<l_listofrect.size(); j++){

          ColoredRect currentRightRect = r_listofrect[j];
          Scalar colorright = currentRightRect.getColor();

          //Compare color between rectangles
          if(currentLeftRect == currentRightRect){

              Moments rightmoments = moments(Mat(currentRightRect.getContour()));
              Point2f massCenter_right = Point2f(rightmoments.m10/rightmoments.m00, rightmoments.m01/rightmoments.m00);

              //undistort masscenter
              undistortPoint(massCenter_right, distCoeffs_right);

              //Estimate the distance from the epiline
              float dist2line = fabs(line2test[0]*massCenter_right.x + line2test[1]*massCenter_right.y + line2test[2]);

              if (dist2line < epipolarDistance /*5*/&&
                  //the two poins should be in particular zone of the image
                  abs(massCenter_left.x - massCenter_right.x)< imageLeft.cols/3 /*200*/&&
                  abs(massCenter_left.y- massCenter_right.y) < imageLeft.rows/3 /*200*/){

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


//        //If a matching is found
//        if(idxright != -1){
//      //    cout<<i<<" with "<<idxright<<" and distance "<<minDistance<<endl;
//          char numberobj[2];
//          stringstream left,right;
//          sprintf(numberobj, "%d->%d", i, idxright);
//          left<<numberobj;
//
//          //Only for debugging
//          putText(outputImageLeft, left.str(), currentLeftRect.getBBTopLeftCorner(), FONT_HERSHEY_DUPLEX , 0.5, CV_RGB(0,0,0));
//
//          sprintf(numberobj, "%d->%d", idxright, i);
//          right<<numberobj;
//          //Only for debugging
//          putText(outputImageRight, right.str(), right_rectList[idxright].getBBTopLeftCorner(), FONT_HERSHEY_DUPLEX , 0.5, CV_RGB(0,0,0));
//
//          //Add the blob match in blobs
//          blobs_matches.push_back(DMatch(idxright, i, minDistance)); //query first and after train
//        }
//
    }

}

