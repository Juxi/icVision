#include "iCub/camera/camera.h"


Camera::Camera(std::string name, std::string side, std::string input) 
{

	Network yarp;								//	set up yarp
	
	namesideclass = side; 						//	"left or right" (suppose "left")
	nameclass =name + "_" + side;				//	"Camera_left"
	nameinput = input;							//	"main"
	
	std::cout<<"\nConstructing " << nameclass << "...\n";
	
/*	SEND AN IMAGE	*/		
	nameImageOutPort = "/image/" + nameclass + "/" + namesideclass + "/out"; //[image/Camera_left/left/out]
	SendingImagePort.open(nameImageOutPort.c_str());					
	iplimageout = NULL;

	
	
}

Camera::~Camera() 
{

	std::cout<<"\nDestructing " << nameclass << "...";

/*	SEND AN IMAGE	*/			
	SendingImagePort.interrupt();
	SendingImagePort.close();
	cvReleaseImage(&iplimageout);
	
	std::cout<<"finished.\n";
}

void Camera::setParam(std::string path) 
{

	#if DEBUGCAMERA
		std::cout <<"\n" << nameclass <<"::setParam()";
	#endif

/*	SEND AN IMAGE	*/			
	nameimagepath = path;
	#if DEBUGCAMERA
		std::cout <<"\n" << nameclass <<"::setParam()::nameimagepath = [" << nameimagepath << "]";
	#endif
	
	cvReleaseImage(&iplimageout);
	iplimageout = cvLoadImage(nameimagepath.c_str());
	

}

void Camera::execute()
{

	#if DEBUGCAMERA
		std::cout <<"\n" << nameclass <<"::execute()";
	#endif

/*	SEND AN IMAGE	*/			
    yarpimageout.wrapIplImage(iplimageout);
	SendingImagePort.prepare()=yarpimageout;
	SendingImagePort.write();

}
