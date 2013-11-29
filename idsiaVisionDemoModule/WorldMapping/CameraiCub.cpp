/*
 * CameraiCub.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: icub
 */

#include "CameraiCub.h"

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

CameraiCub::CameraiCub(std::string moduleName, std::string flag) {

  nameflag = flag; 						//	"left or right" (suppose "left")

  cout<<"Opening ports for camera " << flag << endl;

  // /WorldMappingModule/cam/left/image:i
  inputPortName = "/"+moduleName+"/cam/"+flag+"/image:i";
  outputPortName = "/"+moduleName+"/cam/"+flag+"/image:o";
  outputHarrisPortName = "/"+moduleName+"/cam/"+flag+"/harris:o";

  //Input image port
  if(! inputPort.open( inputPortName.c_str() )){
      cerr<<"Unable to open port "+inputPortName<<endl;
  }
  cout<<"Opened port "+inputPortName<<endl;

  //Output image port
  if(! outputPort.open( outputPortName.c_str() )){
      cerr<<"Unable to open port "+outputPortName<<endl;
  }
  cout<<"Opened port "+outputPortName<<endl;

  //Output HARRIS port
  if(! outputHarrisPort.open( outputHarrisPortName.c_str() )){
      cerr<<"Unable to open port "+outputHarrisPortName<<endl;
  }
  cout<<"Opened port "+outputHarrisPortName<<endl;

  //For feature detector
  detector = FeatureDetector::create("HARRIS");

  //For feature descriptor
  descriptor = DescriptorExtractor::create("BRIEF");

  //Set up Gabor Descriptor
  gaborDescriptor.setParam();

  //For the happiness of Gabriele
  iplimagein = NULL;
  iplimageout = NULL;
  iplimageinGrayLevel = NULL;

}

CameraiCub::~CameraiCub() {
  close();

  if(iplimagein)
    cvReleaseImage(&iplimagein);
  if(iplimageout)
    cvReleaseImage(&iplimageout);
  if(iplimageinGrayLevel)
    cvReleaseImage(&iplimageinGrayLevel);
}

bool CameraiCub::connect(string port2connect){
  Network::connect(port2connect.c_str(),inputPortName.c_str());
}

bool CameraiCub::interrupt(){
  inputPort.interrupt();
  outputPort.interrupt();
}

bool CameraiCub::close(){
  interrupt();
  inputPort.close();
  outputPort.close();
}

bool CameraiCub::getImageOnOutputPort(){

  thereIsImage =  ( inputPort.getInputCount() > 0 );


  if ( !thereIsImage){
      cout<<"There are no images"<<endl;
      return false;
  }
  else{

      imagein = inputPort.read();  // read an image
      iplimagein = ( IplImage*)imagein->getIplImage();

      if(iplimagein){
          cout<<"Receiving image"<<endl;

          //Convert the image in gray level
          if(!iplimageinGrayLevel)
            iplimageinGrayLevel = cvCreateImage(cvSize(iplimagein->width,iplimagein->height),IPL_DEPTH_8U,1);
          cvCvtColor(iplimagein, iplimageinGrayLevel, CV_BGR2GRAY);

          //Broadcast the output image
          iplimageout = iplimagein;
          imageout.wrapIplImage(iplimageout);
          outputPort.prepare()=imageout;
          outputPort.write();


          return true;
      }
      cout<<"Error in communication"<<endl;

      return false;
  }

}

bool CameraiCub::getFeaturesOnOutputPort(FeatureType type){

  GoodFeaturesToTrackDetector::Params params;

  switch (type) {
  case HARRIS:
    params.useHarrisDetector = true;
    params.maxCorners = 1000;
    params.qualityLevel = 0.01;
    params.minDistance = 3;
    detector = new GoodFeaturesToTrackDetector(params);
    break;
  case GFTT:
    params.useHarrisDetector = false;
    params.maxCorners = 1000;
    params.qualityLevel = 0.01;
    params.minDistance = 3;
    params.blockSize = 3;
    detector = new GoodFeaturesToTrackDetector(params);
    break;
  case FAST10:
    detector = new FastFeatureDetector(10);
    break;
  case DeSURF:
    detector = new  SurfFeatureDetector( 1000, 3, 4 );
    break;
  default:
    break;
  }


  Mat outImage = Mat(iplimagein,false);
  cvtColor(outImage, imageMat, CV_BGR2GRAY);

  if(!imageMat.empty()){

      //TODO change the feature detection on the base of type

      //Detect Features
      keypoints.clear();
      detector->detect(imageMat, keypoints);

      /*	SEND HARRIS CORNER POINT	*/
      Bottle& HarrisOut = outputHarrisPort.prepare();
      HarrisOut.clear();
      HarrisOut.addInt(keypoints.size());
      for (int i = 0; i<keypoints.size(); i++)
        {
          HarrisOut.addDouble(keypoints[i].pt.x);
          HarrisOut.addDouble(keypoints[i].pt.y);
        }
      outputHarrisPort.write();

      return true;
  }

  cout<<"The image is empty"<<endl;
  return false;
}

bool CameraiCub::getGaborDescriptorsOnOutputPort(){

  if(!keypoints.empty() && iplimageinGrayLevel){
      gaborDescriptor.detect(keypoints, iplimageinGrayLevel, gaborDescMatrix);
      return true;
  }

  //TODO put gabor on ports
  return false;

}

bool CameraiCub::getDescriptorsOnOutputPort(DescriptorType type){

  switch (type) {
  case DSURF:
    descriptor = DescriptorExtractor::create("SURF");
    break;
  case DBRIEF:
    descriptor = new BriefDescriptorExtractor(64);
    break;
    break;
  case GABOR:
    gaborDescriptor.detect(keypoints, iplimageinGrayLevel, gaborDescMatrix);
    return true;
    break;
  default:
    break;
  }

  descriptor->compute(iplimageinGrayLevel,keypoints, descMatrix);

  return false;

}



