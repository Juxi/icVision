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

int ColoredObjectDetector::objectDetection(Mat &image, Mat &outputImage, vector<ColoredRect> &objects){
  int numberOfNewObject = 0;
  if(image.empty()){
      return 0;
  }

  //Predict Object position
  /*TODO*/

  //List of possible object represented as a rectangle with color

  Mat mask;

  //Detect colored object in left and right camera
  colorObjectDetection(image, mask, objects, outputImage);
  cout<<"Found "<<objects.size()<<" possible objects in "<<cameraFlag<<" camera"<<endl;

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


