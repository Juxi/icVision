// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Authors: David Vernon
 * email:   yuor_email
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

/**
 * @file demoModule.cpp
 * @brief implementation of the demoModule methods following the RFModule standards.
 */

/*
 * Audit Trail
 * -----------
 *  .... if any
 */

#include "WorldMapping.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;
using namespace yarp::math;
using namespace iCub::iKin;


/*
 * Configure method. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * If you are migrating from the old Module, this is the
 * equivalent of the "open" method.
 */

bool WorldMapping::configure(yarp::os::ResourceFinder &rf) {

	/*
	 * PLEASE remove useless comments when writing actual code. If needed then use Doxygen comments and tags.
	 */


	/* Process all parameters from both command-line and .ini file */
	/* get the module name which will form the stem of all module port names */
	moduleName            = rf.check("name",
			Value("WorldMappingModule"),
			"module name (string)").asString();

	/*
	 * before continuing, set the module name before getting any other parameters,
	 * specifically the port names which are dependent on the module name
	 */
	setName(moduleName.c_str());

	/* now, get the rest of the parameters */
	/*
	 * get the robot name which will form the stem of the robot ports names
	 * and append the specific part and device required
	 */
	robotName             = rf.check("robot",
			Value("icub"),
			"Robot name (string)").asString();

	cyclecounter = 0;
	counter = 0;
	detectNewSaliencyPoint = true;


	//Set-up camera ports, input and output
	cameraLeft = new CameraiCub(moduleName, "left");
	cameraRight = new CameraiCub(moduleName, "right");

	//Set-up stereo geometry function
	stereoutils = new StereoGeometry(moduleName, Camera320);

	//Set-up saliency map
	saliencyutils = new SaliencyMap();

	//Set-up moving head
	// movinghead = new MovingHead();

	//Define LookAtPoint thread
	// lookAtLocation = new LookAtLocation();

	//  lookAtLocation->start();
	//lookAtLocation->threadInit();

	/* do all initialization here */
	/*
	 * attach a port of the same name as the module (prefixed with a /) to the module
	 * so that messages received from the port are redirected to the respond method
	 */
	handlerPortName =  "/";
	handlerPortName += getName();         // use getName() rather than a literal

	if (!handlerPort.open(handlerPortName.c_str())) {
		cout << getName() << ": Unable to open port " << handlerPortName << endl;
		return false;
	}

	lookAt3DPortName = "/";
	lookAt3DPortName +=getName();
	lookAt3DPortName +="/lookAt3Dpoint:o";

	if (!lookAt3DPort.open(lookAt3DPortName.c_str())) {
		cout << getName() << ": Unable to open port " << lookAt3DPortName << endl;
		return false;
	}

	motionPortName = "/";
	motionPortName +=getName();
	motionPortName +="/motion:i";

	if (!motionPort.open(motionPortName.c_str())) {
		cout << getName() << ": Unable to open port " << motionPortName << endl;
		return false;
	}

	trainingPortName = "/";
	trainingPortName +=getName();
	trainingPortName +="/training:i";

	if (!trainingPort.open(trainingPortName.c_str())) {
		cout << getName() << ": Unable to open port " << trainingPortName << endl;
		return false;
	}

	attach(handlerPort);  // attach to port

	//Connect Port
	cameraLeft->connect("/icub/cam/left");
	cameraRight->connect("/icub/cam/right");

	stereoutils->connect("/icub/torso/state:o", "/icub/head/state:o");
	//movinghead->connect(saliencyutils->getPortName(0),saliencyutils->getPortName(1));
	// lookAtLocation->connect(lookAt3DPortName);


	//    /* create the thread and pass pointers to the module parameters */
	//    dThread = new demoThread(&thresholdValue);
	//
	//    /* now start the thread to do the work */
	//    dThread->start(); // this calls threadInit() and it if returns true, it then calls run()
	isSavingImage = false;
	waitAnswer = true;

	if(waitAnswer){
		//WAIT For a key
		cout<<" Press a key "<<endl;
		cin >> go;
	}

    Network::connect("/MotionModule/check:o", motionPortName.c_str());
    Network::connect("/TrainingModule/check_out", trainingPortName.c_str());
	return true ;     // let the RFModule know everything went well
	// so that it will then run the module
}

bool WorldMapping::interruptModule() {
	handlerPort.interrupt();
	return true;
}

bool WorldMapping::close() {
	handlerPort.close();

	cameraLeft->close();
	cameraRight->close();
	//movinghead->close();


	return true;
}

bool WorldMapping::respond(const Bottle& command, Bottle& reply) {
	string helpMessage =  string(getName().c_str()) +
			" commands are: \n" +
			"help \n" +
			"quit \n" +
			"set thr <n> ... set the threshold \n" +
			"(where <n> is an integer number) \n";

	reply.clear();

	if (command.get(0).asString()=="quit") {
		reply.addString("quitting");
		return false;
	}
	else if (command.get(0).asString()=="help") {
		cout << helpMessage;
		reply.addString("ok");
	}
	else if (command.get(0).asString()=="set") {
		if (command.get(1).asString()=="thr") {
			//        thresholdValue = command.get(2).asInt(); // set parameter value
			reply.addString("ok");
		}
	}
	return true;
}

/* Called periodically every getPeriod() seconds */
bool WorldMapping::updateModule() {

	cyclecounter++;


	isImageLeft = cameraLeft->getImageOnOutputPort();
	isImageRight = cameraRight->getImageOnOutputPort();

	if(isSavingImage){
		saveImage(cameraLeft->getImage(), "/home/icub/Desktop/Detection/left/", cyclecounter);
		saveImage(cameraRight->getImage(), "/home/icub/Desktop/Detection/right/", cyclecounter);
	}
	if(isImageLeft && isImageRight ){

		if(detectNewSaliencyPoint){


			//Correct the stereo camera if something is changed
			stereoutils->changeCalibration(cameraLeft->getImage().cols);

			//do something
			cameraLeft->getFeaturesOnOutputPort(FAST10);
			//cameraLeft->getFeaturesOnOutputPort(GFTT);
			//cameraLeft->getGaborDescriptorsOnOutputPort();
			cameraLeft->getDescriptorsOnOutputPort(DBRIEF);

			//TODO CHANGE IN CameraiCub.cpp "HARRIS" linea 48
			cameraRight->getFeaturesOnOutputPort(FAST10);
			//cameraRight->getFeaturesOnOutputPort(GFTT);
			//cameraRight->getGaborDescriptorsOnOutputPort();
			cameraRight->getDescriptorsOnOutputPort(DBRIEF);

			vector<KeyPoint> keypointsLeft = cameraLeft->getKeypoints();
			vector<KeyPoint> keypointsRight = cameraRight->getKeypoints();

			//Mat gaborDescrLeft = cameraLeft->getGaborDescriptors();
			//Mat gaborDescrRight = cameraRight->getGaborDescriptors();
			Mat descrLeft = cameraLeft->getDescriptors();
			Mat descrRight = cameraRight->getDescriptors();

			vector<DMatch> matches;

			//stereoutils->matchingGabor(gaborDescrLeft, gaborDescrRight, matches );
			stereoutils->matching(descrLeft, descrRight, matches, DBRIEF);

			int point_count = matches.size();
			vector<Point2f> points1(point_count);
			vector<Point2f> points2(point_count);

			// initialize the points here ... */
			for( int i = 0; i < point_count; i++ )
			{
				points1[i] = keypointsLeft[matches[i].queryIdx].pt;
				points2[i] = keypointsRight[matches[i].trainIdx].pt;

			}

			vector<uchar> outlier_mask;
			if(point_count>10)
				Mat H =  findHomography(points1, points2, RANSAC, 3, outlier_mask);

			Mat resultImage;
			drawMatches(cameraLeft->getImage(), keypointsLeft, cameraRight->getImage(), keypointsRight, matches, resultImage,CV_RGB(255,0,0), CV_RGB(0,0,255), reinterpret_cast<const vector<char>&> (outlier_mask));
			namedWindow("Feature Matching Result");
			imshow("Feature Matching Result", resultImage);

			if(isSavingImage){
				saveImage(resultImage, "/home/icub/Desktop/Detection/stereo/", cyclecounter);
			}
			vector<DMatch> matches_tmp;
			//Filter out matches
			for( int i = 0; i < matches.size(); i++ )
			{
				if(outlier_mask[i]){
					matches_tmp.push_back(matches[i]);
				}
			}

			matches.clear();
			matches = matches_tmp;

			int selectedfeature = saliencyutils->detectSaliencyPoint(cameraLeft->getImage(), cameraRight->getImage(), keypointsLeft, keypointsRight, matches);

			namedWindow("SaliencyMapLeft");
			imshow("SaliencyMapLeft", saliencyutils->getLeftMap());

			namedWindow("SaliencyMapRight");
			imshow("SaliencyMapRight", saliencyutils->getRightMap());

			if(isSavingImage){
				saveImage(saliencyutils->getLeftMap(), "/home/icub/Desktop/Detection/leftsm/", cyclecounter);
				saveImage(saliencyutils->getRightMap(), "/home/icub/Desktop/Detection/rightsm/", cyclecounter);
			}

			/*if(saliencyutils->move == true)
				movinghead->get2DPoints();*/

			Mat outImageLeft;
			cameraLeft->getImage().copyTo(outImageLeft);
			Mat outImageRight;
			cameraRight->getImage().copyTo(outImageRight);

			stereoutils->estimateRTfromImages(cameraLeft->getImage(), cameraRight->getImage(), outImageLeft, outImageRight);

			namedWindow("Left Camera Chessboard Detection");
			imshow("Left Camera Chessboard Detection", outImageLeft);

			namedWindow("Right Camera Chessboard Detection");
			imshow("Right Camera Chessboard Detection", outImageRight);

			vector<int> selectedIndexes;
			vector<Point3f> selectedPoints3d;
			Point3f lookAtThisPoint3D;
			Rect selectedBB_left, selectedBB_right;
		    isValid = 0;

		    //if(selectedfeature!=-1 && counter < 5)
		    	stereoutils->segmentOnDepth(keypointsLeft, keypointsRight, matches, selectedfeature, selectedIndexes, selectedPoints3d, lookAtThisPoint3D, selectedBB_left, selectedBB_right, isValid);
		   /* else{

		    	stereoutils->sendfakeBBOnPort();
		    	lookAtThisPoint3D.x = -0.5;
		    	lookAtThisPoint3D.y = 0.0;
		    	lookAtThisPoint3D.z = 0.3;
		    	counter= 0;
		    }*/


			//Send the 3d position
			//SENDING POINTS COORDINATES
			Bottle& PntOut = lookAt3DPort.prepare();
			PntOut.clear();
			PntOut.addDouble(lookAtThisPoint3D.x);
			PntOut.addDouble(lookAtThisPoint3D.y);
			PntOut.addDouble(lookAtThisPoint3D.z);
			lookAt3DPort.write();

			cout<<"I am sending "<<lookAtThisPoint3D<<endl;

			// counter++;

			vector<KeyPoint> selectedPoints2d_left, selectedPoints2d_right;
			vector<DMatch> testmatches;

			for(int i=0; i<selectedIndexes.size(); i++){
				selectedPoints2d_left.push_back(keypointsLeft[matches[selectedIndexes[i]].queryIdx]);
				selectedPoints2d_right.push_back(keypointsRight[matches[selectedIndexes[i]].trainIdx]);

				testmatches.push_back(matches[selectedIndexes[i]]);
			}

			namedWindow("Selected Matching");
			Mat image2Print_left = cameraLeft->getImage();
			Mat image2Print_right = cameraRight->getImage();
			rectangle(image2Print_left, selectedBB_left, CV_RGB(255,0,0));
			rectangle(image2Print_right, selectedBB_right, CV_RGB(255,0,0));
			drawMatches(image2Print_left, keypointsLeft, image2Print_right, keypointsRight, testmatches, resultImage,CV_RGB(255,0,0), CV_RGB(0,0,255));

			imshow("Selected Matching", resultImage);

			if(isSavingImage){
				saveImage(resultImage, "/home/icub/Desktop/Detection/stereoselected/", cyclecounter);
			}

			//      namedWindow("SelectedObjLeft");
			//      drawKeypoints(cameraLeft->getImage(), selectedPoints2d_left, outImageLeft, CV_RGB(255,0,0),DrawMatchesFlags::DEFAULT);
			//      imshow("SelectedObjLeft", outImageLeft);
			//
			//      namedWindow("SelectedObjRight");
			//      drawKeypoints(cameraRight->getImage(), selectedPoints2d_right, outImageRight, CV_RGB(255,0,0),DrawMatchesFlags::DEFAULT);
			//      imshow("SelectedObjRight", outImageRight);

			counter++;
		}

		//TODO UNCOMMENT WHEN MOTION READY
		if(waitAnswer && isValid == 1){
			cout<<"Waiting Answer from Gabriele"<<endl;
			Bottle* motionAnswer = motionPort.read(true);

			cout<<"The Gabriele answer is "<<endl;
			//if answer is 1 then detect a new point
			if(motionAnswer->get(0).asInt() == 1){
				detectNewSaliencyPoint = true;
				//Inhibit
				cout<<"OK"<<endl;
				//Send message to Yiannis
				stereoutils->sendZeroBBOnPort();
				cout<<"Waiting Answer from Yiannis"<<endl;

				Bottle* trainingAnswer;
				do{
					trainingAnswer = trainingPort.read(true);
					cout<<"The Yiannis answer is "<<endl;
					//if answer is 1 then detect a new point
					if(trainingAnswer->get(0).asInt() == 2){
						cout<<"OK"<<endl;
						saliencyutils->inhibitSaliencyMap(100, 100);
					}
					else
						cout<<"No"<<endl;
				}while(trainingAnswer->get(0).asInt() != 2);

			}
			else{
				detectNewSaliencyPoint = false;

				cout<<"No"<<endl;
			}
		}
		cvWaitKey(330);


	}

	return true;
}

double WorldMapping::getPeriod() {
	/* module periodicity (seconds), called implicitly by myModule */
	return 0.1;
}

bool WorldMapping::saveImage(Mat& image2save, string directory, int framecounter){
	cout<<"Start saving image "<<endl;

	stringstream filename;
	if(framecounter<10)
		filename<<directory<<"im_000"<<framecounter<<".ppm";
	else if(framecounter<100)
		filename<<directory<<"im_00"<<framecounter<<".ppm";
	else if(framecounter<1000)
		filename<<directory<<"im_0"<<framecounter<<".ppm";
	else
		filename<<directory<<"im_"<<framecounter<<".ppm";

	cout<<"Saving image "<<filename.str()<<endl;

	vector<int> params;
	params.push_back(CV_IMWRITE_PXM_BINARY);
	return imwrite(filename.str(), image2save,params);
}
