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

		// /worldmapping/cam/left/image:i
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
			imageout = outputPort.prepare();
			iplimageout = iplimagein;
			outputPort.write();

			return true;
	     }
		 cout<<"Error in communication"<<endl;

		 return false;
	 }

}

bool CameraiCub::getFeaturesOnOutputPort(FeatureType type){

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

bool CameraiCub::getGaborDescriptorOnOutputPort(){

	if(!keypoints.empty() && iplimageinGrayLevel){
		gaborDescriptor.detect(keypoints, iplimageinGrayLevel, gaborDescMatrix);
		return true;
	}

	return false;

}

bool CameraiCub::loadCalibration(){
  //TODO
}

bool CameraiCub::saveCalibration(){
  //TODO
}

Mat& CameraiCub::undistortImage(Mat& image2undist){
  //TODO
}




