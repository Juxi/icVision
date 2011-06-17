/*
 * ColoredObjectDetector.cpp
 *
 *  Created on: May 25, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#include "ColoredObjectDetector.h"

ColoredObjectDetector::ColoredObjectDetector(string camera)
{
  cameraFlag = camera;
  if (loadColorStats(COLORSTATFILENAME))
    cerr<<"Error in loading color statistics file"<<endl;

}

ColoredObjectDetector::~ColoredObjectDetector()
{
  // TODO Auto-generated destructor stub
}

bool ColoredObjectDetector::loadColorStats(string filename){

  /*Color statistics*/
    FileStorage fs(filename, CV_STORAGE_READ );
    int colorsDim;

    colorsStatMean.clear();
    colorsStatICov.clear();

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

        colorsStatMean.push_back(meanColor);
        colorsStatICov.push_back(icovColor);
    }
    fs.release();

    return true;
}

int ColoredObjectDetector::objectDetection(Mat &imageLeft, Mat &imageRight, vector<WorldObject> &obj_list, Mat & lc2w, Mat & rc2w){
  int numberOfNewObject = 0;
  if(imageLeft.empty() || imageRight.empty()){
      return 0;
  }

  //Predict Object position
  /*TODO*/

  //List of possible object represented as a rectangle with color
  vector<ColoredRect> left_possibleObjs, right_possibleObjs;
  Mat maskLeft, maskRight;

  //Only for debug
  Mat outputImageLeft, outputImageRight;
  imageLeft.copyTo(outputImageLeft);
  imageRight.copyTo(outputImageRight);

  //Detect colore object in left and right camera
   colorObjectDetection(imageLeft, maskLeft, left_possibleObjs, outputImageLeft);
   cout<<"Found "<<left_possibleObjs.size()<<" possible objects in left image"<<endl;
   colorObjectDetection(imageRight, maskRight, right_possibleObjs, outputImageRight);
   cout<<"Found "<<right_possibleObjs.size()<<" possible objects in right image"<<endl;

   //Left and Right association (It is better to use the H matrix)



   //Estimate the position in 3D... rays intersection


//   //Perform data association between left and right camera
//   numberOfNewObject =  dataAssociation(left_rectList, right_rectList, obj_list);
//
//   //For each object estimate position and orientation
//   for(uint oo = 0; oo<obj_list.size(); oo++){
//       char numberobj[2];
//       stringstream left,right;
//       sprintf(numberobj, "%.02d", obj_list[oo].getId());
//       left<<numberobj;
//
//       ColoredRect leftbb = obj_list[oo].getColoredRect('l');
//       RotatedRect leftRRect = leftbb.getRotatedRect();
//
//      // findObjectContour(imageLeft, imageLeft_gray, leftbb, outputImageLeft);
//
//       float angle =  leftRRect.angle;
//       float height = leftRRect.size.height;
//       float width = leftRRect.size.width;
//       float cosangle = cos((angle/180)*CV_PI);
//       float sinangle = sin((angle/180)*CV_PI);
//
//       //Divide the h and w
//       height = height/2;
//       width = width/2;
//
//       //define 4 points
//       Point2f pt1 = Point2f(-width * cosangle +  leftRRect.center.x, -width * sinangle +  leftRRect.center.y);
//       Point2f pt2 = Point2f(width * cosangle +  leftRRect.center.x, width * sinangle +  leftRRect.center.y);
//       Point2f pt3 = Point2f(height * sinangle +  leftRRect.center.x, -height * cosangle +  leftRRect.center.y);
//       Point2f pt4 = Point2f(-height * sinangle +  leftRRect.center.x, height * cosangle +  leftRRect.center.y);
//
//
//       line(outputImageLeft, pt1, pt2, leftbb.getColor());
//       line(outputImageLeft, pt3, pt4, leftbb.getColor());
//       ellipse(outputImageLeft, leftRRect, leftbb.getColor());
//       rectangle(outputImageLeft, leftbb.getRect(), leftbb.getColor());
//       putText(outputImageLeft, left.str(), leftbb.getBBCenter(), FONT_HERSHEY_DUPLEX , 0.5, leftbb.getColor());
//       right<<numberobj;
//
//       ColoredRect rightbb = obj_list[oo].getColoredRect('r');
//       RotatedRect rightRRect = rightbb.getRotatedRect();
//
//       angle =  rightRRect.angle;
//       height = rightRRect.size.height;
//       width = rightRRect.size.width;
//       cosangle = cos((angle/180)*CV_PI);
//       sinangle = sin((angle/180)*CV_PI);
//
//       //Divide the h and w
//       height = height/2;
//       width = width/2;
//
//       //define 4 points
//       pt1 = Point2f(-width * cosangle +  rightRRect.center.x, -width * sinangle +  rightRRect.center.y);
//       pt2 = Point2f(width * cosangle +  rightRRect.center.x, width * sinangle +  rightRRect.center.y);
//       pt3 = Point2f(height * sinangle +  rightRRect.center.x, -height * cosangle +  rightRRect.center.y);
//       pt4 = Point2f(-height * sinangle +  rightRRect.center.x, height * cosangle +  rightRRect.center.y);
//
//
//       line(outputImageRight, pt1, pt2, rightbb.getColor());
//       line(outputImageRight, pt3, pt4, rightbb.getColor());
//       ellipse(outputImageRight, rightRRect, rightbb.getColor());
//       rectangle(outputImageRight, rightbb.getRect(), rightbb.getColor());
//       putText(outputImageRight, right.str(), rightbb.getBBCenter(), FONT_HERSHEY_DUPLEX , 0.5, rightbb.getColor());
//   }

   return numberOfNewObject;
}

void ColoredObjectDetector::colorObjectDetection(Mat &image, Mat& outputMask, vector<ColoredRect> &listofobj, Mat& outputImage){

  outputMask = Mat::zeros(image.rows, image.cols, CV_8UC1);

  int boxcounter = 0;
  for(uint cc = 0; cc<colorsStatMean.size(); cc++){

    Mat binaryImage;
    Scalar color;

    switch (cc) {
      case 0: color = CV_RGB(255,0,0);
        break;
      case 1: color = CV_RGB(0,255,0);
        break;
      case 2: color = CV_RGB(0,0,255);
        break;
      default:
        break;
    }

    colorSegmentation(image, colorsStatMean[cc], colorsStatICov[cc+1], binaryImage);

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
          //drawContours( outputImage, contours_obj, idx, color, CV_FILLED, 8, hierarchy_obj );

          //Increase the BB to capture the background
          objRect = Rect(max(0,objRect.x-5), max(0,objRect.y-5), min(image.cols - objRect.x + 5, objRect.width+10), min(image.rows - objRect.y + 5, objRect.height+10));

          if(objRect.area() > 300){
              Mat roi(outputMask, objRect);
              roi =  Scalar(255);
              listofobj.push_back(ColoredRect(objRect, color, contours_obj[idx]));
              boxcounter++;
          }
       }
    }

  }
}

void ColoredObjectDetector::colorSegmentation(Mat &image, Mat &m, Mat &icov, Mat &binaryImage, float threshold){

   //Convert the image in HSV color space
   Mat imagetmp;
   cvtColor(image, imagetmp, CV_BGR2HSV_FULL);

   //Initialize the binary image
   Mat resultingImage = Mat::zeros(image.rows, image.cols, CV_64FC1);
   for(int r = 0; r<imagetmp.rows; r++){
          for(int c = 0; c<imagetmp.cols; c++){

              Mat color2test = (Mat_<double>(3,1) << (int)imagetmp.at<Vec3b>(r,c)[0],(int)imagetmp.at<Vec3b>(r,c)[1],(int)imagetmp.at<Vec3b>(r,c)[2]);

              resultingImage.at<double>(r,c) = Mahalanobis(color2test, m, icov);
          }
   }

   //cout<<resultingImage<<endl;
   Mat dst;
   compare(resultingImage, threshold, binaryImage, CMP_LE);


   Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3) );
   morphologyEx(binaryImage, binaryImage, CV_MOP_ERODE, element);
   morphologyEx(binaryImage, binaryImage, CV_MOP_ERODE, element);
   morphologyEx(binaryImage, binaryImage, CV_MOP_DILATE, element);
   morphologyEx(binaryImage, binaryImage, CV_MOP_DILATE, element);

}

void ColoredObjectDetector::lrDataAssociation(vector<ColoredRect> &l_listofrect, vector<ColoredRect> &r_listofrect, vector<DMatch> &matching){
//  //matches between blobs
//  //  vector<DMatch> blobs_matches;
//
//  //Iterate objects in left camera
//  for(uint i = 0; i<l_listofrect.size(); i++){
//
//      ColoredRect currentLeftRect = l_listofrect[i];
//
//      Scalar colorleft = currentLeftRect.getColor();
//      Mat leftobject = Mat(currentLeftRect.getContour());
//
//      //Estimate shape moments
//      Moments leftmoments = moments(Mat(currentLeftRect.getContour()));
//      Point2f massCenter_left = Point2f(leftmoments.m10/leftmoments.m00, leftmoments.m01/leftmoments.m00);
//
//
//      //Epipolar geometry
//      vector<Vec3f> right_epilines;
//      vector<Point2f> points_left;
//
//      //undistort masscenter
//      undistortPoint(massCenter_left, distCoeffs_left);
//
//      points_left.push_back(massCenter_left);
//      //Estimate Epipolar Line
//      computeCorrespondEpilines(Mat(points_left), 1, F, right_epilines);
//      Vec3f line2test = right_epilines[0];
//
//      //A possible match is a point (mass center) belonging to the Epipolar line
//      double minDistance = DBL_MAX;
//      double diffArea = -1;
//      int idxright = -1;
//
//      for(uint j = 0; j<l_listofrect.size(); j++){
//
//          ColoredRect currentRightRect = right_rectList[j];
//          Scalar colorright = currentRightRect.getColor();
//
//          //Compare colors
//          if(colorleft[0] == colorright[0] &&
//             colorleft[1] == colorright[1] &&
//             colorleft[2] == colorright[2]){
//
//             Moments rightmoments = moments(Mat(currentRightRect.getContour()));
//             Point2f massCenter_right = Point2f(rightmoments.m10/rightmoments.m00, rightmoments.m01/rightmoments.m00);
//
//             //undistort masscenter
//             undistortPoint(massCenter_right, distCoeffs_right);
//
//
//             //Estimate the distance from the epiline
//             float dist2line = fabs(line2test[0]*massCenter_right.x + line2test[1]*massCenter_right.y + line2test[2]);
//
//             //if distance is less then a threshold, the dimension are similar and the x and y position are not so far
//             if (dist2line < 5 &&
//                 abs(massCenter_left.x - massCenter_right.x)< 200 &&
//                 abs(massCenter_left.y- massCenter_right.y) < 200){
//
//                  Mat rightobject = Mat(currentRightRect.getContour());
//                  double distance =  matchShapes(leftobject, rightobject, CV_CONTOURS_MATCH_I3, 0);
//
//                  //Match also the shape
//                  if(distance < minDistance){
//                      minDistance = distance;
//                      idxright = j;
//                  }
//                }
//
//            }
//
//        }
//
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
//    }

}
