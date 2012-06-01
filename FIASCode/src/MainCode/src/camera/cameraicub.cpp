#include "iCub/camera/cameraicub.h"


CameraiCub::CameraiCub(std::string name, std::string side, std::string input) 
{

	Network yarp;								//	set up yarp
	
	namesideclass = side; 						//	"left or right" (suppose "left")
	nameclass =name + "_" + side;				//	"CameraiCub_left"
	nameinput = input;							//	"main"
	
	std::cout<<"\nConstructing " << nameclass << "...\n";
	
/*	READ AN IMAGE	*/		
	nameImageInPort = "/image/" + nameclass + "/" + namesideclass + "/in";		//[image/ClassTemplate_left/left/in]
	nameSenderImagePort = "/icub/cam/"+namesideclass;	//[icub/cam/left]
	ReceivingImagePort.open(nameImageInPort.c_str());
	Network::connect(nameSenderImagePort.c_str(),nameImageInPort.c_str());	
	receiveimage = false;
	yarpimagein = NULL;
	iplimagein = NULL;
	
/*	SEND AN IMAGE	*/		
	nameImageOutPort = "/image/" + nameclass + "/" + namesideclass + "/out"; //[image/CameraiCub_left/left/out]
	SendingImagePort.open(nameImageOutPort.c_str());					
	iplimageout = NULL;

	std::cout<<"finished.\n";
	
}

CameraiCub::~CameraiCub() 
{

	std::cout<<"\nDestructing " << nameclass << "...";

/*	READ AN IMAGE	*/
	ReceivingImagePort.interrupt();
	ReceivingImagePort.close();
	//if(iplimagein) cvReleaseImage(&iplimagein);		//if uncommented gives SIGFAULT	

/*	SEND AN IMAGE	*/			
	SendingImagePort.interrupt();
	SendingImagePort.close();
	//cvReleaseImage(&iplimageout);
	
	std::cout<<"finished.\n";
}

void CameraiCub::setParam() 
{

	#if DEBUGCAMERAICUB
		std::cout <<"\n" << nameclass <<"::setParam()";
	#endif


}

void CameraiCub::execute()
{

	#if DEBUGCAMERAICUB
		std::cout <<"\n" << nameclass <<"::execute()";
	#endif
/*	READ AN IMAGE	*/	
	yarpimagein = ReceivingImagePort.read(&receiveimage);
	iplimagein = ( IplImage*)yarpimagein->getIplImage();
	
	iplimageout = iplimagein;

	//std::cout <<"\n" << nameclass <<"::here\n";
	//cvWaitKey(0);
		
/*	SEND AN IMAGE	*/			
    yarpimageout.wrapIplImage(iplimageout);
	SendingImagePort.prepare()=yarpimageout;
	SendingImagePort.write();

}
