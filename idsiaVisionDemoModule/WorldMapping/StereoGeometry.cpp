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

StereoGeometry::StereoGeometry(string moduleName, Cameratype type)
{

	cameraType = type;

	RTb2cl = Mat::eye(4,4,CV_32FC1);
	RTcl2b = Mat::eye(4,4,CV_32FC1);
	P_left = Mat::zeros(4,3,CV_32FC1);
	P_right = Mat::zeros(4,3,CV_32FC1);
	RTcl2w = Mat::eye(4,4,CV_32FC1);
	RTw2cl = Mat::eye(4,4,CV_32FC1);

	if(type == Camera640)
		loadCalibration("../conf/calibrationMatrices640.yaml");
	else
		loadCalibration("../conf/calibrationMatrices320.yaml");


	//loadCalibration("calibrationMatrices.yaml");

	RTb2w = Mat::eye(4,4,CV_32FC1);

	RTb2w.at<float>(0,0) = 0;  RTb2w.at<float>(0,1) = 0;  RTb2w.at<float>(0,2) = 1;  RTb2w.at<float>(0,3) = -0.85;
	RTb2w.at<float>(1,1) = 0;  RTb2w.at<float>(1,1) = 1;  RTb2w.at<float>(1,2) = 0;  RTb2w.at<float>(1,3) = -0.18;
	RTb2w.at<float>(2,1) = -1;  RTb2w.at<float>(2,1) = 0;  RTb2w.at<float>(2,2) = 0;  RTb2w.at<float>(2,3) = 0.03;

	RTw2b = RTb2w.inv();


	cout<<"Opening ports for reading encoders "<< endl;

	inputHeadPortName = "/"+moduleName+"/stereo/head:i";
	inputTorsoPortName =  "/"+moduleName+"/stereo/torso:i";
	outputBBLeftPortName =  "/"+moduleName+"/stereo/bb_left_out";
	outputBBRightPortName =  "/"+moduleName+"/stereo/bb_right_out";
	outputCheckPortName =  "/"+moduleName+"/stereo/check_out";

	//Input head port
	if(! inputHeadPort.open( inputHeadPortName.c_str() )){
		cerr<<"Unable to open port "+inputHeadPortName<<endl;
	}
	cout<<"Opened port "+inputHeadPortName<<endl;

	//Input torso port
	if(! inputTorsoPort.open( inputTorsoPortName.c_str() )){
		cerr<<"Unable to open port "+inputTorsoPortName<<endl;
	}
	cout<<"Opened port "+inputTorsoPortName<<endl;

	//Output BB port
	if(! outputBBLeftPort.open( outputBBLeftPortName.c_str() )){
		cerr<<"Unable to open port "+outputBBLeftPortName<<endl;
	}
	cout<<"Opened port "+outputBBLeftPortName<<endl;

	//Output BB port
	if(! outputBBRightPort.open( outputBBRightPortName.c_str() )){
		cerr<<"Unable to open port "+outputBBRightPortName<<endl;
	}
	cout<<"Opened port "+outputBBRightPortName<<endl;

	//Output Check port
	if(! outputCheckPort.open( outputCheckPortName.c_str() )){
		cerr<<"Unable to open port "+outputCheckPortName<<endl;
	}
	cout<<"Opened port "+outputCheckPortName<<endl;
}

StereoGeometry::~StereoGeometry()
{

	/* if(cameraType == Camera640)
    saveCalibration("../conf/calibrationMatrices640.yaml");
  else
    saveCalibration("../conf/calibrationMatrices320.yaml");
	 */

}


void StereoGeometry::changeCalibration(int w){
	if( w == 640 && cameraType == Camera320){
		cameraType = Camera640;
		loadCalibration("../conf/calibrationMatrices640.yaml");
	}
	else if( w == 320 && cameraType == Camera640){
		cameraType = Camera320;
		loadCalibration("../conf/calibrationMatrices320.yaml");
	}

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

		/*
      if(cameraType == Camera640)
         saveCalibration("../conf/calibrationMatrices640.yaml");
       else
         saveCalibration("../conf/calibrationMatrices320.yaml");
		 */

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

void StereoGeometry::undistortImage(Mat& image2undist, Mat& undistortedImage, string camera){

	image2undist.copyTo(undistortedImage);

	if( camera.compare("left") == 0)
		undistort(image2undist, undistortedImage, K_left, d_left);
	else if( camera.compare("right") == 0)
		undistort(image2undist, undistortedImage, K_right, d_right);
	else
		cout<<"I can not undistort the image"<<endl;


}


void StereoGeometry::estimatePmatrix(Mat &K, Mat &RTworld2camera, Mat &P){

	P = Mat::zeros(3,4, K.type());
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
}

void StereoGeometry::estimatePmatrix(Mat &K, Mat &Rworld2camera, Mat &Tworld2camera, Mat &P){

	P = Mat::zeros(3,4, K.type());
	// Mat iR_left = Mat::eye(3,3, RTcamera2world.type());
	// Mat iT_left =  Mat::zeros(3,1, RTcamera2world.type());
	Mat iR_left = Rworld2camera;
	Mat iT_left =  Tworld2camera; //Convertion from m->mm ?

	Mat P_tmpR = P(Rect(0,0,3,3));
	Mat P_tmpT = P(Rect(3,0,1,3));
	Mat tmplR = K*iR_left;
	Mat tmplT = K*iT_left;
	tmplR.copyTo(P_tmpR);
	tmplT.copyTo(P_tmpT);
}


Point2f StereoGeometry::undistortPoint(Point2f &point2undistort, string camera){
	Mat dist;

	if( camera.compare("left") == 0)
		dist = d_left;
	else if( camera.compare("right") == 0)
		dist = d_right;
	else{
		cout<<"ERROR"<<endl;
		return Point2f(-1,-1);
	}

	float k1 = dist.at<float>(0,0);
	float k2 = dist.at<float>(0,1);
	float k3 = dist.at<float>(0,4);
	float p1 = dist.at<float>(0,2);
	float p2 = dist.at<float>(0,3);

	Point2f x = Point2f(point2undistort.x, point2undistort.y);            // % initial guess

	for (int kk=0; kk<20; kk++){

		//r_2 = sum(x.^2);
		float r_2 = x.x*x.x+x.y*x.y;

		//k_radial =  1 + k1 * r_2 + k2 * r_2.^2 + k3 * r_2.^3;
		float k_radial = 1 + k1 * r_2 + k2 * r_2*r_2 + k3 * r_2*r_2*r_2;

		//delta_x = [2*p1*x(1,:).*x(2,:) + p2*(r_2 + 2*x(1,:).^2);
		Point2f delta_x = Point2f(  2*p1*x.x*x.y + p2*(r_2 + 2*x.x*x.x),
				p1 * (r_2 + 2*x.y*x.y)+2*p2*x.x*x.y);

		//x = (xd - delta_x)./(ones(2,1)*k_radial);
		Point2f tmp = Point2f( (point2undistort.x - delta_x.x)/k_radial, (point2undistort.y - delta_x.y)/k_radial);
		x = Point2f(tmp.x, tmp.y);

	}

	return x;
}

void StereoGeometry::undistortPoint(Mat &point2undistort, Mat &pointundistorted, string camera){
	Point2f pointtmp;
	pointtmp.x = point2undistort.at<float>(0,0);
	pointtmp.y = point2undistort.at<float>(1,0);

	Point2f pointResult = undistortPoint(pointtmp, camera);
	pointundistorted = Mat::zeros(point2undistort.rows, point2undistort.cols, point2undistort.type());

	pointundistorted.at<float>(0,0) = pointResult.x;
	pointundistorted.at<float>(1,0) = pointResult.y;

}

void StereoGeometry::triangulatePoint(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d){

	Point2f pl_und =  undistortPoint(pl, "left");
	Point2f pr_und =  undistortPoint(pr, "right");

	Mat matpleft = (Mat_<float>(2,1) << pl_und.x, pl_und.y);
	Mat matpright = (Mat_<float>(2,1) << pr_und.x, pr_und.y);

	Mat doubleleft, doubleright;
	matpleft.convertTo(doubleleft, CV_64FC1);
	matpright.convertTo(doubleright, CV_64FC1);

	CvMat __points1 = doubleleft, __points2 = doubleright;
	CvMat __newpoints1 = doubleleft, __newpoints2 = doubleright;

	/*
=======
  /*
>>>>>>> 1d81e393cb89b09196236014e5f9e720e7300be4
  if(P_left.empty())
    estimatePmatrix(K_left, RTw2cl, P_left);
  if(P_right.empty())
    estimatePmatrix(K_right, RTw2cr, P_right);
	 */


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

	//Normalize the point
	outTM = outTM/outTM.at<double>(3,0);
	Mat tmpoutTM = outTM.clone();
	//Conversion from mm to m?!?
	outTM.at<double>(0,0)/=1000;
	outTM.at<double>(1,0)/=1000;
	outTM.at<double>(2,0)/=1000;
	outTM.convertTo(outTM, CV_32FC1);
	Mat result = RTcl2w*outTM;

	point3d.x = result.at<float>(0,0)/result.at<float>(3,0);
	point3d.y = result.at<float>(1,0)/result.at<float>(3,0);
	point3d.z = result.at<float>(2,0)/result.at<float>(3,0);


}

void StereoGeometry::triangulatePointChessboard(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d){

	Point2f pl_und =  undistortPoint(pl, "left");
	Point2f pr_und =  undistortPoint(pr, "right");

	Mat matpleft = (Mat_<float>(2,1) << pl_und.x, pl_und.y);
	Mat matpright = (Mat_<float>(2,1) << pr_und.x, pr_und.y);

	Mat doubleleft, doubleright;
	matpleft.convertTo(doubleleft, CV_64FC1);
	matpright.convertTo(doubleright, CV_64FC1);

	CvMat __points1 = doubleleft, __points2 = doubleright;
	CvMat __newpoints1 = doubleleft, __newpoints2 = doubleright;

	/*
=======
  /*
>>>>>>> 1d81e393cb89b09196236014e5f9e720e7300be4
  if(P_left.empty())
      estimatePmatrix(K_left, RTw2cl, P_left);
    if(P_right.empty())
      estimatePmatrix(K_right, RTw2cr, P_right);
<<<<<<< HEAD
	 */

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

	//Normalize the point
	outTM = outTM/outTM.at<double>(3,0);
	Mat tmpoutTM = outTM.clone();

	//from camera to chessboard
	tmpoutTM.convertTo(tmpoutTM, CV_32FC1);
	Mat result2 = RTcl2b*tmpoutTM;

	//Conversion from mm to m?!?
	result2.at<float>(0,0)/=1000;
	result2.at<float>(1,0)/=1000;
	result2.at<float>(2,0)/=1000;
	Mat finalresult = result2/result2.at<float>(3,0);

	//TODO FIX THIS board 2 world
	point3d.x = finalresult.at<float>(2,0)-0.85; //~84cm
	point3d.y = finalresult.at<float>(1,0)-0.18; //~18 cm
	point3d.z = -(finalresult.at<float>(0,0)-0.03); //~3cm

	//Mat test = RTb2w*finalresult;
	//cout<<point3d<<endl;
	//cout<<test<<endl;

}

void StereoGeometry::triangulatePointLeftCamera(cv::Point2f &pl, cv::Point2f &pr, Point3f &point3d){
	//This function is the same included in the toolbox of Bouguet

	Mat point_l_distort = (Mat_<float>(2,1) << (pl.x - K_left.at<float>(0,2))/K_left.at<float>(0,0),
			(pl.y - K_left.at<float>(1,2))/K_left.at<float>(1,1));
	Mat point_r_distort = (Mat_<float>(2,1) << (pr.x - K_right.at<float>(0,2))/K_right.at<float>(0,0),
			(pr.y - K_right.at<float>(1,2))/K_right.at<float>(1,1));


	Mat point_l_undist, point_r_undist;

	//Undistort points
	undistortPoint(point_l_distort, point_l_undist, "left");
	undistortPoint(point_r_distort, point_r_undist, "right");


	Mat points_l = (Mat_<float>(3,1) << point_l_undist.at<float>(0,0), point_l_undist.at<float>(1,0), 1);
	Mat points_r = (Mat_<float>(3,1) << point_r_undist.at<float>(0,0), point_r_undist.at<float>(1,0), 1);

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

	Mat point3DLeft = 0.5 * (X1+X2);

	//Transform in meters
	point3d.x = point3DLeft.at<float>(0,0)/1000;
	point3d.y = point3DLeft.at<float>(1,0)/1000;
	point3d.z = point3DLeft.at<float>(2,0)/1000;


	//TODO for testing

	/*

  Mat point2dtest = K_left*point3DLeft;
  point2dtest.at<float>(0,0)/=point2dtest.at<float>(2,0);
  point2dtest.at<float>(1,0)/=point2dtest.at<float>(2,0);
  point2dtest.at<float>(2,0)/=point2dtest.at<float>(2,0);

  point2dtest.at<float>(0,0)-= point_l_distort.at<float>(0,0);
  point2dtest.at<float>(1,0)-= point_l_distort.at<float>(1,0);

	 */

	//   cout<<point3D<<endl;
	//    %--- Right coordinates:
	//    XR = R*XL + T_vect;
	//Mat point3DRight = R*point3DLeft+T;
}



void StereoGeometry::segmentOnDepth(vector<cv::KeyPoint> &keysLeft, vector<KeyPoint> &keysRight, vector<DMatch> &matches, int selectedFeature,
		vector<int> &selectedIndexes, vector<Point3f> &selectedPoints3d, Point3f &lookAtPoint3d, Rect &objRect_left, Rect &objRect_right){

	Point2f pointLeft, pointRight;
	Point3f point3d, selectedPoint3d;

	//Estimate depth of the selected point
	//cout<<"Estimate new object"<<endl;

	//  triangulatePointLeftCamera(keysLeft[matches[selectedFeature].queryIdx].pt, keysRight[matches[selectedFeature].trainIdx].pt, selectedPoint3d);

	Mat fakeR = Mat::eye(3,3,R.type());
	Mat fakeT = Mat::zeros(3,1,T.type());
	estimatePmatrix(K_left, fakeR, fakeT, P_left);
	estimatePmatrix(K_right, R, T, P_right);

	cout<<"Triangulating point "<<keysLeft[matches[selectedFeature].queryIdx].pt<<" "<<keysRight[matches[selectedFeature].trainIdx].pt<<endl;
	//triangulatePoint(keysLeft[matches[selectedFeature].queryIdx].pt, keysRight[matches[selectedFeature].trainIdx].pt, selectedPoint3d);
	triangulatePointChessboard(keysLeft[matches[selectedFeature].queryIdx].pt, keysRight[matches[selectedFeature].trainIdx].pt, selectedPoint3d);

	lookAtPoint3d = selectedPoint3d;
	int isValid = 0;

	if(lookAtPoint3d.x < 0 &&
			lookAtPoint3d.y < 1.5 && lookAtPoint3d.x > -1.5 &&
			lookAtPoint3d.z < 1 && lookAtPoint3d.z > 0.5){

		isValid = 1;

	}

	//cout<<"***********************"<<endl;
	for(int i = 0; i<matches.size(); i++){
		pointLeft = keysLeft[matches[i].queryIdx].pt;
		pointRight = keysRight[matches[i].trainIdx].pt;

		//estimate the point wrt camera left
		//triangulatePointLeftCamera(pointLeft, pointRight, point3d);
		//triangulatePoint(pointLeft, pointRight, point3d);
		triangulatePointChessboard(pointLeft, pointRight, point3d);

		if(point3d.x < 0){

			if( abs(point3d.x-selectedPoint3d.x) < 0.05 &&
					abs(point3d.y-selectedPoint3d.y) < 0.25 &&
					abs(point3d.z-selectedPoint3d.z) < 0.25){

				//cout<<point3d<<endl;

				selectedIndexes.push_back(i);
				selectedPoints3d.push_back(point3d);
			}

		}
	}

	//Only for debug
	//Select the point
	vector<Point> selectedPoints2d_left, selectedPoints2d_right;

	cout<<"Size of points "<<selectedIndexes.size()<<endl;
	for(int i = 0; i<selectedIndexes.size(); i++){
		selectedPoints2d_left.push_back(keysLeft[matches[i].queryIdx].pt);
		selectedPoints2d_right.push_back(keysRight[matches[i].trainIdx].pt);
	}

	objRect_left = Rect(0,0,0,0);
	objRect_right = Rect(0,0,0,0);

	if(selectedIndexes.size() > 0){
		objRect_left =  boundingRect( Mat(selectedPoints2d_left) );
		objRect_right =  boundingRect( Mat(selectedPoints2d_right) );
	}

    //SENDING POINTS COORDINATES
	Bottle& bblOut = outputBBLeftPort.prepare();
	Bottle& bbrOut = outputBBRightPort.prepare();
	Bottle& checkOut = outputCheckPort.prepare();
	bblOut.clear();
	bbrOut.clear();
	checkOut.clear();
	bblOut.addInt(objRect_left.x);
	bblOut.addInt(objRect_left.y);
	bblOut.addInt(objRect_left.width);
	bblOut.addInt(objRect_left.height);
	bbrOut.addInt(objRect_right.x);
	bbrOut.addInt(objRect_right.y);
	bbrOut.addInt(objRect_right.width);
	bbrOut.addInt(objRect_right.height);
	checkOut.addInt(isValid);
	outputCheckPort.write();
	outputBBLeftPort.write();
	outputBBRightPort.write();


}

void StereoGeometry::pointFromCamera2World(cv::Point2f &pointCamera, cv::Point2f &pointWorld, std::string camera){

}

