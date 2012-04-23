/* DepthYARP.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-11
 * Class definition file for the DepthYARP class.
 * This class is the encapsulation for a YARP module that runs the Depth Perception algorithm.
 * It uses the DisparityMapper class, which is the actual implementation of the disparity-based
 * depth perception algorithm.
 *
 * Todo:- make ini parameters readable for shifting range
 */
#include "stdafx.h"
#include "DepthYARP.h"

// standard constructor, sets initial values to the DisparityMapper
DepthYARP::DepthYARP(){
	mapper = new DisparityMapper();
//	mapper->setShiftingRange(-40, 40, 2);
	mapper->setLikelihoodParameters("gaussian", .8);

	// set the filter parameters for the standard linear filter
	((LinFilter *)mapper->getFilter())->setParameter(.66);

	// init the map image
	dispMap = NULL;
}

// standard destructor
DepthYARP::~DepthYARP() { 
	free(mapper);
	if(dispMap) free(dispMap);
}

double DepthYARP::getPeriod() {
	return 1;
}

// open function for the YARP module
// is called when the module is created, initial parameters can be set in an ini file
// (and will be parsed here)
bool DepthYARP::open(Searchable &config) {
	// check if called with help option
	if(config.check("help", "if present, display usage message")) {
		printf("Call with --name </portprefix> --file <configfile.ini>\n");
		return false;
	}

	//TODO: parse parameters for mapper and filter from ini file ...
	
	
	if ( config.check("scale") ) {
		scalingFactor = (config.find("scale")).asDouble();
	} else {
		scalingFactor = 1.0;
	}
	std::cout << "Scaling input images with factor: " << scalingFactor << std::endl;		
	
	
	int vStep, hStep, vRange, hRange;
	
	hStep = config.check("hStep", Value(2.0)).asInt();
	vStep = config.check("vStep", Value(4.0)).asInt();
	hRange = config.check("hRange", Value(40.0)).asInt();
	vRange = config.check("vRange", Value(8.0)).asInt();
	
	//	-70 70 5 -6 6 2  0.3 0.66	
	mapper->setShiftingRange(-hRange, hRange, hStep, -vRange, vRange, vStep);
	

	// Set the name of the port (use "/DepthPerception" if there is no --name option)
	setName(config.check("name", Value("/DepthPerception")).asString());
	
	std::string rpcPortName;
	rpcPortName = getName("rpc:i");
	std::cout << "blu: " << rpcPortName;
	
	if(! rpcPort.open( rpcPortName.c_str() )){
		return false;
	}
	
	
	// open the ports
	leftIn.open(  getName(config.check("LeftImage", Value("/LeftImage")).asString()));
	rightIn.open( getName(config.check("RightImage", Value("/RightImage")).asString()));
	outPort.open( getName(config.check("DisparityMap", Value("/DisparityMap")).asString()));
	
	Network::connect("/icub/cam/left", getName(config.check("LeftImage", Value("/LeftImage")).asString())); 
	Network::connect("/icub/cam/right", getName(config.check("RightImage", Value("/RightImage")).asString())); 
	
	return true;
}

// close function for the YARP module
// is called when the module is destroyed, clean up is done here
bool DepthYARP::close() {
	leftIn.close();
	rightIn.close();
	outPort.close();
	return true;
}

// interrupt function for the YARP module
// is called when the module communication is interrupted, this is passed on to all ports
bool DepthYARP::interruptModule() {
	leftIn.interrupt();
	rightIn.interrupt();
	outPort.interrupt();
	return true;
}

// update function for the YARP module
// is called when the module is updated (received data), 
// this is the main function of this class, calling the mapper to calculate 
bool DepthYARP::updateModule() {
	ImageOf<PixelBgr> *left = leftIn.read(true);
	ImageOf<PixelBgr> *right = rightIn.read(true);
	if(left == NULL || right == NULL) // application has been asked to quit
		return true;

	// get the ipl images
	IplImage *leftImg = (IplImage*) left->getIplImage();
	IplImage *rightImg = (IplImage*) right->getIplImage();

	if(left != NULL && right != NULL) {
		// both images are here right now...

		/*		//some debug information
		char help[100];
		sprintf(help, "leftImg.%04d.bmp", counter);
		cvSaveImage(help, leftImg);
		sprintf(help, "rightImg.%04d.bmp", counter);
		cvSaveImage(help, rightImg);
		counter++;
		cvNamedWindow("main");
		cvShowImage("main", leftImg);*/

		// converting the images to be usable and also resize them (reduce calculation time)
		IplImage *newLeft = NULL, *newRight = NULL, *helper = NULL;
		if(!helper) helper = cvCreateImage(cvGetSize(rightImg), rightImg->depth, 1); 
		if(!newRight) newRight = cvCreateImage(cvSize(rightImg->width*scalingFactor, rightImg->height*scalingFactor), rightImg->depth, 1); 
		if(!newLeft) newLeft = cvCreateImage(cvSize(rightImg->width*scalingFactor, rightImg->height*scalingFactor), rightImg->depth, 1); 

		std::cout << "The image size was set to width: " << newLeft->width << " height: " << newLeft->height << std::endl;
		
				
		// convert to grayscale
		cvCvtColor(rightImg, helper, CV_RGB2GRAY);
		cvResize(helper, newRight);
		cvCvtColor(leftImg, helper, CV_RGB2GRAY);
		cvResize(helper, newLeft);
		
		cvSaveImage("left.bmp", newLeft);
		cvSaveImage("right.bmp", newRight);
		
		
		cvReleaseImage(&helper); helper = NULL;

		// set the mappers attributes
		mapper->setLeftImage(newLeft);
		mapper->setRightImage(newRight);

		// generate the disparity map(s)
		mapper->generateDisparityMap();

		// normalize disparity map for displaying purposes
		mapper->normalizeDisparityMap();

//		dispMap = mapper->getHorizontalDisparityMap();

		// for output convert the map (dispMap) to 32bit RGB (helper) for output
		ImageOf<PixelBgr> yarpReturnImage;
		IplImage *map = mapper->getHorizontalDisparityMap();
		IplImage* img = cvCreateImage( cvGetSize(map), 8, 1 );
		
		cvConvertScale(map, img, 255, 0);

		IplImage* imgOut = cvCreateImage( cvSize(640,480), 8, 1 );
		cvResize(img, imgOut, CV_BILATERAL);
		
		helper = cvCreateImage(cvGetSize(imgOut), IPL_DEPTH_8U, 3);
		cvConvertImage(imgOut, helper);
		yarpReturnImage.wrapIplImage(helper);
		outPort.prepare() = yarpReturnImage;
		outPort.write();

		cvSaveImage("map.out.bmp", imgOut);

		cvReleaseImage(&helper); helper = NULL;
	}
	return false;
}