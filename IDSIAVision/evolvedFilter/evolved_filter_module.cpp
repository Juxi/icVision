// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include <time.h>	
#include "evolved_filter_module.h"

#include "GpImage.h"


// TODO:
//		- check for memory leak
//			o probably something with the input files?!
//			o or also the CvCreate stuff...

using namespace yarp::os;
using namespace yarp::sig;

EvolvedFilterModule::EvolvedFilterModule()
{
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("EvolvedFilter");
	gray = red = green = blue =	h =	s = v = NULL;
	
	inDebugMode = false;
}

EvolvedFilterModule::~EvolvedFilterModule() { 
	
	if( rawImageToWrite != NULL )
		cvReleaseImage(&rawImageToWrite);
	if( outputImageToWrite != NULL )
		cvReleaseImage(&outputImageToWrite);
	
	// cleanup input files
	if( gray != NULL) {
		cvReleaseImage(&gray );		
		cvReleaseImage(&red  );		
		cvReleaseImage(&green);				
		cvReleaseImage(&blue );			
		cvReleaseImage(&h    );				
		cvReleaseImage(&s    );		
		cvReleaseImage(&v    );
		
		gray = red = green = blue =	h =	s = v = NULL;	
	}		
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool EvolvedFilterModule::updateModule()
{
	if( ! inDebugMode) {
		putchar('.'); std::cout.flush();
	} else {
		std::cout << "DEBUG: Run filter!" << std::endl;	
		start = clock();
	}

	IplImage* in = NULL;
	
	ImageOf<PixelBgr> *left_image = NULL;
	ImageOf<PixelBgr> *right_image = NULL;	
	
	if( isReadingFileFromHDD ) {
		in = cvLoadImage(fileName.c_str());
	} else {	
		// read image from the port
		left_image = leftInPort.read();  // read an image
		if (left_image == NULL) { 
			std::cout << "ERROR: Could not read from port '" << leftInPort.getName() << "'!" << std::endl;
			return false;
		}
		// read image from the right port
		right_image = rightInPort.read();  // read an image
		if (right_image == NULL) { 
			std::cout << "ERROR: Could not read from port '" << rightInPort.getName() << "'!" << std::endl;
			return false;
		}

	}
	
	bool allFramesDone = true;
	if( runOnLeft == runOnRight == true ) {
		allFramesDone = false;
	}
	
	CvPoint frame1_1, frame1_2, frame2_1, frame2_2;
	
	frame1_1.x = frame2_1.x = 0.0;
	frame1_1.y = frame2_1.y = 0.0;
	frame1_2.x = frame2_2.x = 0.0;
	frame1_2.y = frame2_2.y = 0.0;
	
	CvPoint ph, ph1, ph2;
	
	do {
		readEncoderPositions();
		
		if(in == NULL) {
			// first run 
			in = (IplImage*) left_image->getIplImage();
		}else{
			allFramesDone = true;
			in = (IplImage*) right_image->getIplImage();
		}

		ImageWidth  = in->width * scalingFactor;
		ImageHeight = in->height * scalingFactor;	
		
		static int index = 0;
		if( inDebugMode) {
			std::cout << "DEBUG: Got input image!" << std::endl;	
			GpImage* inputImg = new GpImage(in);
			char fileIn[80];
			sprintf(fileIn, "input-frame-%05d.png", index++
					);
			inputImg->Save(fileIn);
		}
		
		// set which images from the input we need for the filter
		this->setUsedInputs();
		
		// create input images to the filter
		createInputImages(in);

		if( inDebugMode ) {
			// DEBUG test output of the creating
			std::cout << "DEBUG: Created the GP input images!" << std::endl;		
			int i = 0;
			for(std::vector<GpImage*>::iterator it = InputImages.begin(); it != InputImages.end(); it++ ) {
				if((*it)) {
					std::string fileName = "input-";
					fileName += '0' + i++;
					fileName += ".png";
					(*it)->Save(fileName);
				}
			}
		}
		
		// run filter
		GpImage* filteredImg = this->runFilter();
			
		if( inDebugMode ) {
			//DEBUG
			filteredImg->Save("output.png");
			std::cout << "DEBUG: Now yarping..." << std::endl;		
		}

		
		bool wewantoverlay = true;
		bool wewanttothreshold = true;	
		
		IplImage* rgb = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_32F, 3);	
		IplImage* out8 = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);
		IplImage* out_single = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 1);			
		
		// convert the of filtered image to be outputted
		cvCvtColor(filteredImg->Image, rgb, CV_GRAY2RGB);
		cvConvertScale(rgb, out8, 1.0, 0.0);

		if(allFramesDone && streamRawFilterOutput) {
			// check if we have allocated the to be written image already
			if(rawImageToWrite == NULL)
				rawImageToWrite = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);
			cvCopy(out8, rawImageToWrite);
			ImageOf<PixelBgr>& rawOutput = rawOutputPort.prepare();
			rawOutput.wrapIplImage(rawImageToWrite); //output.copy ( *left_image );
			rawOutputPort.writeStrict();
		}		
		
		if(wewantoverlay) {
			// overlay original	
			//cvAdd(InputImages[0]->Image, filteredImg->Image, InputImages[0]->Image);
			IplImage* r = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
			IplImage* g = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
			IplImage* b = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
			
			cvCvtColor(gray, rgb, CV_GRAY2RGB);
			cvSplit(rgb, b, g, r, NULL);
			cvAdd(r, filteredImg->Image, r);
			cvMerge(b, g, r, NULL, rgb);	
			
			cvReleaseImage(&r);
			cvReleaseImage(&g);	
			cvReleaseImage(&b);	
		}
		
		// if we want to threshold the output! there we go	
		if(wewanttothreshold) {
			cvThreshold(out8, out8, 250.0, 255.0, CV_THRESH_BINARY);
			
			
			// Blob detection
			cvCvtColor(out8, out_single, CV_RGB2GRAY);
			//Linked list of connected pixel sequences in a binary image
			CvSeq* seq;
			
			//Array of bounding boxes
			vector<CvRect> boxes;
			
			//Memory allocated for OpenCV function operations
			CvMemStorage* storage = cvCreateMemStorage(0);
			cvClearMemStorage(storage);
			
			//Find connected pixel sequences within a binary OpenGL image (diff), starting at the top-left corner (0,0)
			cvFindContours(out_single, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
			
			//Iterate through segments
			for(; seq; seq = seq->h_next) {
				//Find minimal bounding box for each sequence
				CvRect boundbox = cvBoundingRect(seq);
				boxes.push_back(boundbox);
			}
			
			cvReleaseMemStorage(&storage);

			for(unsigned int boxNr = 0; boxNr < boxes.size(); boxNr++) {
				CvRect r = boxes[boxNr];
				//std::cout << "BOX Info: " << r.x << "," << r.y << "," << r.width << "," << r.height  << std::endl;
				CvPoint p1, p2;
				p1.x = r.x; p1.y = r.y;
				p2.x = r.x + r.width; p2.y = r.y + r.height;
				
				int x = r.x + r.width/2;
				int y = r.y + r.height;

				if(wewantoverlay) {
					cvRectangle(rgb, p1, p2, CV_RGB(255,0,0), 2, 8, 0 );
					ph.x = x; ph.y = y;
					cvCircle(rgb, ph, 2, CV_RGB(0,0,255), 2, 8, 0 );			
					
					if(allFramesDone) ph1 = ph;
					else ph2 = ph;			

				} else {
					cvRectangle(out8, p1, p2, CV_RGB(255,0,0), 2, 8, 0 );
				}
				
				
				if(allFramesDone) {
					frame1_1 = p1;
					frame1_2 = p2;
				}else{
					frame2_1 = p1;
					frame2_2 = p2;
				}
				// only do for one block // HACK // TODO
				break;
			}
			
		}
		
		if(wewantoverlay) {
			cvConvertScale(rgb, out8, 1.0, 0.0);
		}
		
		if(allFramesDone && streamProcessedFilterOutput) {
			// check if we have allocated the to be written image already
			if(outputImageToWrite == NULL)
				outputImageToWrite = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);
			
			cvCopy(out8, outputImageToWrite);			
			ImageOf<PixelBgr>& output = outputPort_Image.prepare();
			output.wrapIplImage(outputImageToWrite); 
			outputPort_Image.write();	
		}
			
		// cleanup 
		cvReleaseImage(&out_single);	
		cvReleaseImage(&out8);
		cvReleaseImage(&rgb);
		delete filteredImg;
		
		InputImages.clear();
		
	}while(!allFramesDone);

	// only if we do it on both images
	if( runOnLeft == runOnRight == true )	{
		std::cout << "frame1.x/2: " << ph1.x/2 << "\ty/2: " << ph1.y/2;
		std::cout << "\t\tframe2.x/2: " << ph2.x/2 << "\ty/2:" << ph2.y/2 << std::endl;		
		calculateAndSetObjectWorldPosition(frame1_1, frame1_2, frame2_1,frame2_2);

		
		
		// workaround!!
		
		Vector X = Vector();
		X.push_back(ph1.x/2);
		X.push_back(ph1.y/2);
		X.push_back(ph2.x/2);
		X.push_back(ph2.y/2);
		
		Vector &Xsend=portIKinIn->prepare(); // get pointer
		Xsend = X; // set to port
		
		//CvPoint3D32f p3d = cvPoint3D32f(FLT_MAX, FLT_MAX, FLT_MAX);
		
//		std::cout << "trying to connect to the ikinport " << std::endl;		
		if (portIKinOut->getInputCount() > 0) {
			portIKinIn->writeStrict();
			Bottle *ret = portIKinOut->read(true); //#TODO: the iKinHead thread should actually wait for a new coordinate to arrive, which can be achieved by changing the .read command in iKinEyeTriangulate.cpp..
//			if (ret->size() >= 3) {
//				p3d = cvPoint3D32f(ret->get(0).asDouble(), ret->get(1).asDouble(), ret->get(2).asDouble());
//			}
			std::cout << "Bottle: " << ret->toString() << std::endl;
		}
//		std::cout << "ending ikinport " << std::endl;				
		
				
	}

	if( inDebugMode ) {
		end = clock();	
		double diffms = (end-start)*1000.0/CLOCKS_PER_SEC;
		// Debug
		std::cout << "Filter ran for: " << diffms << " ms" << std::endl;
	}
		
	// return	(when we read from HDD we only run once!)
	return !isReadingFileFromHDD;
}

void EvolvedFilterModule::calculateAndSetObjectWorldPosition(CvPoint frame1_1, CvPoint frame1_2, CvPoint frame2_1, CvPoint frame2_2) {
	// HACKING !! todo rewrite, ...
	// assumes we got both points!
	// calculate world
	// first try...
	//			double x = -0.5;
	//			double y = -1.0 * (frame1.x - (in->width*scalingFactor * 0.5))/360.0;
	//			double z = 0.0;
	
	//readEncoderPositions();
	
	//	std::cout <<  frame1.x << ", "<< frame1.y << ", "<< frame2.x<< ", "<< frame2.y << std::endl;
//	double x[21] = { (frame1_1.x + frame1_2.x) / 2.0, (frame1_1.y + frame1_2.y) / 2.0,
//		(frame2_1.x + frame2_2.x) / 2.0, (frame2_1.y + frame2_2.y) / 2.0, 
//		frame1_1.x, frame1_1.y, frame1_2.x, frame1_2.y,
//		frame2_1.x, frame2_1.y, frame2_2.x, frame2_2.y,
//		headjnt_pos[0], headjnt_pos[1],headjnt_pos[2], headjnt_pos[3], headjnt_pos[4], headjnt_pos[5],
//		torsojnt_pos[0], torsojnt_pos[1], torsojnt_pos[2] };
	
//	double PredictedZ = 16.336582 + 0.1394611*x[7] + 0.15662868*x[4] - 0.11288279*x[12] - 0.018227309*x[1];
//	double PredictedX = 2.3311224 + 0.012280603*x[0] + 0.075872004*x[10] + 0.00019401088*x[0]*x[8] + cos(5.1875334 - 0.075872004*x[10] - 0.00019401088*x[0]*x[8] - 0.013261461*x[0])	;


	double x[13] = { 
		(double) (frame1_1.x + (frame1_2.x - frame1_1.x)/2),
		(double) frame1_2.y,
		(double) (frame2_1.x + (frame2_2.x - frame2_1.x)/2),
		(double) frame2_2.y,
		headjnt_pos[0], headjnt_pos[1],headjnt_pos[2], headjnt_pos[3], headjnt_pos[4], headjnt_pos[5],
		torsojnt_pos[0], torsojnt_pos[1], torsojnt_pos[2] };
		
	
	
	
	
//	frontybacky=
//	2.3311224 + 0.012280603*x[0] + 0.075872004*x[10] + 0.00019401088*x[0]*x[8] + cos(5.1875334 - 0.075872004*x[10] - 0.00019401088*x[0]*x[8] - 0.013261461*x[0])
	double estimatedX = 16.336582 + 0.1394611*x[7] + 0.15662868*x[4] - 0.11288279*x[12] - 0.018227309*x[1];
	double estimatedY = 2.3311224 + 0.012280603*x[0] + 0.075872004*x[10] + 0.00019401088*x[0]*x[8] + cos(5.1875334 - 0.075872004*x[10] - 0.00019401088*x[0]*x[8] - 0.013261461*x[0]);
	//1.4924586 + 43.674198/x[0] + 1.5535291*pow((28.112892/x[0]),0.11274087)*abs(13.024383 + 0.11767572*x[7] + 0.13460229*x[4] - 0.1140458*x[12] - 0.015047867*x[1]);
	double estimatedZ = -.075;	//15cm is the table right roughtly
	
	
	
//	///////
//	// wed jan 11
//	// misc1
//	estimatedY = 2.4449713 + 0.45422038*pow(0.014661767*x[0] + 0.11048388*x[10] + 0.10637926*x[8], 1.4340008);
//	estimatedX = 16.690353 + 0.12812519*x[7] + 0.14214782*x[4] - 0.11215064*x[12] - 0.017972222*x[1];
//		
//	// misc2
//	estimatedX = 16.083103 + 0.12178416*x[7] + 0.13797964*x[4] - 0.10328569*x[12] - 0.017162004*x[1];
//	estimatedY = 1.3739585 + 0.01655753*x[0] + 0.11799882*x[10] + 0.12334365*x[8] - 0.11540742*x[8]*pow(0.87538558, 0.25877491*x[0]);
//	
//	// misc3
//	estimatedX = 16.776382 + 0.13043526*x[7] + 0.14312536*x[4] - 0.10902537*x[12] - 0.018313933*x[1];
//	estimatedY = 1.9018469 + 0.014656176*x[0] + 0.12437823*x[10] + 0.11133261*x[8] + (0.11133261*x[8] + 0.057853397*x[0] - 15.825777)/x[12];3	
//	//	printf("%f %f\t", x[0],x[1]);
//	printf("%f %f\n ", x[2],x[3]);	
	
	///////
	// thu jan 12
	// includes last two days
	estimatedX = 16.776382 + 0.13043526*x[7] + 0.14312536*x[4] - 0.10902537*x[12] - 0.018313933*x[1];
	estimatedY = 1.9018469 + 0.014656176*x[0] + 0.12437823*x[10] + 0.11133261*x[8] + (0.11133261*x[8] + 0.057853397*x[0] - 15.825777)/x[12];	

	
	estimatedX = 17.453409 + 0.13380532*x[7] + 0.14905137*x[4] - 0.11798947*x[12] - 0.018469423*x[1];
	estimatedY = (0.031274121*x[0] + 0.2344905*x[10] + 0.21543403*x[8])/pow(x[12], 0.18780835) + log(x[12]) - 2.1461744;
	
	
	
	
	
	std::cout <<  "Predition: "<< round(estimatedX)<< ", "<< (char)(round(estimatedY)+'A') << std::endl;
	
	double CellSize = 6;
	
	estimatedY *= CellSize;
	estimatedX *= CellSize;
	//estimatedZ *= CellSize;
	
	double leftOffset = CellSize * 6 + 2.5;
	double forwardOffset = 17.5;
	
	estimatedY -= leftOffset;
	estimatedX += forwardOffset;
	
	
	std::cout <<  "Predition x/y/z(cm): "<< estimatedX << ", "<< estimatedY << ", " << estimatedZ << std::endl;	
	
	// todo 
	// hacking from thurs 12 jan
	estimatedX += 8.5;
	estimatedY -= 6.5;
	setWorldPositionOfObject(-estimatedX/100.0, estimatedY/100.0, estimatedZ, "cup1");
}

void EvolvedFilterModule::readEncoderPositions() {
	//	std::cout << "Show encoder positions() " << std::endl;	
	
    Port *yarp_port;
	
	// Head data
	yarp_port = &head_port;			
    Bottle input;
	yarp_port->read(input);
	
	if (input != NULL) {
		for(int i = 0; i < input.size(); i++) {
			if(input.get(i).isDouble() && i < HEAD_JOINTS) {
				headjnt_pos[i] = input.get(i).asDouble();
			}
		}
	}
	
	// Torso data
	yarp_port = &torso_port;			
    Bottle input2;
	yarp_port->read(input2);
	if (input2 != NULL) {
		for(int i = 0; i < input2.size(); i++) {
			if(input2.get(i).isDouble() && i < 3) {
				torsojnt_pos[i] = input2.get(i).asDouble();
			}
		}
	}
}


void EvolvedFilterModule::createInputImages(IplImage* in) {
	InputImages.clear();
	
	// only initalize the memory for the images once!
	if( gray == NULL ) {
		gray  = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		red   = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		green = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		blue  = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		h     = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		s     = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);
		v     = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 1);	
	}
	
	// to float
	IplImage* in32  = cvCreateImage(cvSize(in->width, in->height), IPL_DEPTH_32F, 3);	
	cvConvertScale(in, in32, 1.0, 0.0);
	
	// scale if wanted
	IplImage* in32_scaled;
	if( scalingFactor != 1.0) {
		in32_scaled = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 3);	
		cvResize(in32, in32_scaled);
	} else {
		in32_scaled = in32;
	}

	// to gray
	cvCvtColor(in32_scaled, gray, CV_BGR2GRAY);
	if( UsedInputs.uses(0) ) {
		InputImages.push_back(new GpImage(gray));
	} else {
		InputImages.push_back(NULL); // gray
	}
	

	if( UsedInputs.uses(1) || UsedInputs.uses(2) || UsedInputs.uses(3) ) {	
		// from BGR to RGB
		cvSplit(in32_scaled, blue, green, red, NULL);
		InputImages.push_back(new GpImage(red));
		InputImages.push_back(new GpImage(green));
		InputImages.push_back(new GpImage(blue));		
	} else {
		InputImages.push_back(NULL); // red
		InputImages.push_back(NULL); // green
		InputImages.push_back(NULL); // blue
	}	
	
	if( UsedInputs.uses(4) || UsedInputs.uses(5) || UsedInputs.uses(6) ) {	
		// to HSV 
		IplImage* hsvIn = cvCreateImage(cvSize(in->width*scalingFactor, in->height*scalingFactor), IPL_DEPTH_32F, 3);
		cvCvtColor(in32_scaled, hsvIn, CV_BGR2HSV);
		cvSplit(hsvIn, h, s, v, NULL);
		cvReleaseImage(&hsvIn);
		InputImages.push_back(new GpImage(h));
		InputImages.push_back(new GpImage(s));
		InputImages.push_back(new GpImage(v));			
	} else {
		InputImages.push_back(NULL); // h
		InputImages.push_back(NULL); // s
		InputImages.push_back(NULL); // v
	}		
	
	// in32_scaled and in32 are the same if scale is 1.0
	cvReleaseImage(&in32_scaled);
	if( scalingFactor != 1.0) {
		cvReleaseImage(&in32);
	}
	
}


//GpImage* EvolvedFilterModule::runFilter() {
//	GpImage* node1  = InputImages[0];
//	GpImage* node2  = node1->subc(12.0094858501107);
//	GpImage* node3  = node1->ShiftUp();
//	GpImage* node4  = node3->max(node1);
//	GpImage* node5  = InputImages[4];
//	GpImage* node6  = node2->absdiff(node4);
//	GpImage* node7  = node5->threshold(64);
//	GpImage* node8  = node3->absdiff(node6);
//	GpImage* node9  = node7; //NOP
//	GpImage* node11 = node9->SmoothMedian(5);
//	GpImage* node12 = node8->gauss(9);
//	GpImage* node13 = node8->threshold(64);
//	GpImage* node15 = node13->erode(2);
//	GpImage* node16 = node15->mul(node11);
//	GpImage* node18 = node9; //NOP
//	GpImage* node19 = InputImages[4];
//	GpImage* node21 = node7->dilate(1);
//	GpImage* node22 = node16->ShiftRight();
//	GpImage* node24 = node22->unsharpen(9);
//	GpImage* node26 = node12->subc(2.83571027964354);
//	GpImage* node34 = node24->mul(node21);
//	GpImage* node36 = node34->SmoothMedian(5);
//	GpImage* node38 = node36->SmoothMedian(5);
//	GpImage* node39 = node38->ShiftDown();
//	GpImage* node43 = node18->sub(node39);
//	GpImage* node44 = node24->mul(node43);
//	GpImage* node46 = node44->mul(node44);
//	GpImage* node47 = node26->dilate(4);
//	GpImage* node57 = node19->dilate(3);
//	GpImage* node60 = node47->sub(node57);
//	GpImage* node71 = node46->addc(1.16124618053436);
//	GpImage* node85 = node60->threshold(64);
//	GpImage* node99 = node71->min(node85);
//	
//	/*
//	 node1.Save("traceNode0001.png");  
//	 node2.Save("traceNode0002.png");  
//	 node3.Save("traceNode0003.png");  
//	 node4.Save("traceNode0004.png");  
//	 node5.Save("traceNode0005.png");  
//	 node6.Save("traceNode0006.png");  
//	 node7.Save("traceNode0007.png");  
//	 node8.Save("traceNode0008.png");  
//	 node9.Save("traceNode0009.png");  
//	 node11.Save("traceNode0011.png");  
//	 node12.Save("traceNode0012.png");  
//	 node13.Save("traceNode0013.png");  
//	 node15.Save("traceNode0015.png");  
//	 node16.Save("traceNode0016.png");  
//	 node18.Save("traceNode0018.png");  
//	 node19.Save("traceNode0019.png"); */
//	node21->Save("traceNode0021.png"); /* 
//	 node22.Save("traceNode0022.png");  
//	 node24.Save("traceNode0024.png");  
//	 node26.Save("traceNode0026.png");  
//	 node34.Save("traceNode0034.png");  
//	 node36.Save("traceNode0036.png");  
//	 node38.Save("traceNode0038.png");  
//	 node39.Save("traceNode0039.png");  
//	 node43.Save("traceNode0043.png");  
//	 node44.Save("traceNode0044.png");  
//	 node46.Save("traceNode0046.png");  
//	 node47.Save("traceNode0047.png");  */
//	 node57->Save("traceNode0057.png"); /*
//	 node60.Save("traceNode0060.png");  
//	 node71.Save("traceNode0071.png");  
//	 node85.Save("traceNode0085.png");  
//	 node99.Save("traceNode0099.png");  
//	 */
//
//	delete node2 ;
//	delete node3 ;
//	delete node4 ;
//	delete node6 ;
//	delete node7 ;
//	delete node8 ;
//	delete node11;
//	delete node12;
//	delete node13;
//	delete node15;
//	delete node16;
//	delete node21;
//	delete node22;
//	delete node24;
//	delete node26;
//	delete node34;
//	delete node36;
//	delete node38;
//	delete node39;
//	delete node43;
//	delete node44;
//	delete node46;
//	delete node47;
//	delete node57;
//	delete node60;
//	delete node71;
//	delete node85;	
//	
//	return node99;
//}
//

// filter table in sim
GpImage* EvolvedFilterModule::runFilter() {	
//	std::cout << "DEBUG: Created the GP input images!" << std::endl;		
	
	//GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	GpImage* node23 = InputImages[5];
	GpImage* node26 = InputImages[2];

	GpImage* node32 = node26; //NOP
	GpImage* node44 = node32; //NOP
	GpImage* node49 = node23->mul(node44);
	GpImage* node57 = node49; //NOP
	GpImage* node99 = node57->threshold(64);

	//delete(node987654321);
	//delete(node23);
	//delete(node26); //	delete(node32);
					//	delete(node44);
	delete(node49);	// 	delete(node57);
	
	return node99;
}

	   
void EvolvedFilterModule::setUsedInputs() {
	UsedInputs.Add(2);
	UsedInputs.Add(5);	
}


//	GpImage* node987654321 = new GpImage(0, ImageWidth, ImageHeight);
//	GpImage* node0 = node987654321->ShiftDown();
//	GpImage* node1 = node0->sobelx(11);
//	GpImage* node2 = node1->subc(4.58097606152296);
//	GpImage* node3 = InputImages[4];
//	GpImage* node4 = node3->erode(2);
//	GpImage* node5 = node2->mul(node2);
//	GpImage* node11 = node4->erode(1);
//	GpImage* node12 = node11->avg(node5);
//	GpImage* node13 = node12->threshold(64);
//	GpImage* node14 = node13->avg(node0);
//	GpImage* node19 = node14->erode(5);
//	GpImage* node23 = node19->SmoothMedian(5);
//	GpImage* node28 = node23->dilate(4);
//	GpImage* node30 = node28->unsharpen(7);
//	GpImage* node55 = node30->gauss(7);
//	GpImage* node99 = node55->dilate(5);
	
	//	GpImage node987654321 = new GpImage(0, ImageWidth, ImageHeight);
	//	GpImage node0 = InputImages[6];
	//	GpImage node1 = node0.add(node0);
	//	GpImage node2 = node1.dilate(1);
	//	GpImage node3 = node0.dilate(5);
	//	GpImage node5 = node3.laplace(15);
	//	GpImage node6 = node5.dilate(5);
	//	GpImage node8 = node6.Normalize();
	//	GpImage node11 = node8.subc(15.9085604557768);
	//	GpImage node14 = node11.min(node2);
	//	GpImage node27 = node14.addc(0.130839884281158);
	//	GpImage node30 = node8.subc(-3.01183433854021);
	//	GpImage node41 = node27.Normalize();
	//	GpImage node58 = node41.subc(-2.72319238772616);
	//	GpImage node69 = node30.sub(node58);
	//	GpImage node99 = node69.dilate(4);
	
	//	node99 is output
//}
/*
* Message handler. Just echo all received messages.
*/
bool EvolvedFilterModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	std::cout << "Message Received: (echo is on)" << endl;
	std::cout << command.toString() << endl;
	return true;
}

/*
* Interrupt function.
*/
bool EvolvedFilterModule::interruptModule()
{
	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
	leftInPort.interrupt();
//	rightInPort.interrupt();	
	outputPort_Image.interrupt();
//	moduleOutputPort.interrupt();
	return true;
}



/*
* Close function, to perform cleanup.
*/
bool EvolvedFilterModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;

//	moduleOutputPort.close();
	leftInPort.close();
//	rightInPort.close();	
	outputPort_Image.close();
	
	return true;
}
