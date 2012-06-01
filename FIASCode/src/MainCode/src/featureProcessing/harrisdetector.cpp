#include "iCub/featureProcessing/harrisdetector.h"



HarrisDetector::HarrisDetector(std::string name, std::string side, std::string input) 
{


	Network yarp; 						//	set up yarp

	namesideclass = side;				//	"left or right" (suppose "left")
	nameclass =name + "_" + side;		//	"Harris_left"
	nameinput = input;					//	"Camera"

	std::cout<<"\nConstructing " << nameclass << "...\n";
	
	/*	READ AN IMAGE	*/		
	nameImageInPort = "/image/" + nameclass + "/" + namesideclass + "/in";		//[image/Harris_left/left/in]
	nameSenderImagePort = "/image/" + nameinput +"/" + namesideclass + "/out";	//[image/Camera_left/left/in]
	ReceivingImagePort.open(nameImageInPort.c_str());
	//[image/Camera_left/left/in] [image/Harris_left/left/in]	
	Network::connect(nameSenderImagePort.c_str(),nameImageInPort.c_str());	
	receiveimage = false;
	yarpimagein = NULL;
	iplimagein = NULL;

	
	/*	SEND HARRIS	*/
	nameHarrisOutPort = "/harris/" + nameclass + "/" + namesideclass + "/out";//	[harris/Harris_left/left/out]
	SendingHarrisPort.open(nameHarrisOutPort.c_str());
	grayimg = NULL;
	mp_eig_image = NULL;
	mp_temp_image = NULL;
	mp_corners = NULL;
	
	/*	SEND A GRAY IMAGE	*/	
	nameGrImageOutPort = "/grayimage/"+nameclass+"/"+namesideclass+"/out";	//[grayimage/Harris_left/left/out]
	SendingGrImagePort.open(nameGrImageOutPort.c_str());


	/*	DISPLAY HARRIS	*/
	#if DEBUGHARRIS
		nameImageOutPort = "/image/" + nameclass + "/" + namesideclass + "/out";//	[image/Harris_left/left/out]
		SendingImagePort.open(nameImageOutPort.c_str());
		dispImage = NULL;
		if(!dispImage) dispImage = new DisplayIplImage("DisplayHarris",namesideclass.c_str(),nameclass.c_str());
	#endif
		
	
	std::cout<<"finished.\n";
}
 
HarrisDetector::~HarrisDetector() 
{

	std::cout<<"\nDestructing " << nameclass << "...";

/*	READ AN IMAGE	*/
	ReceivingImagePort.interrupt();
	ReceivingImagePort.close();
	//if (iplimagein) cvReleaseImage(&iplimagein);
	
/*	SEND HARRIS	*/
	if(mp_corners) delete [] mp_corners;
	if(grayimg) cvReleaseImage(&grayimg);
	if(mp_eig_image) cvReleaseImage(&mp_eig_image);
	if(mp_temp_image) cvReleaseImage(&mp_temp_image);
	SendingHarrisPort.interrupt();
	SendingHarrisPort.close();
	
/*	SEND A GRAY IMAGE	*/
	SendingGrImagePort.interrupt();
	SendingGrImagePort.close();


/*	DISPLAY HARRIS	*/	
	#if DEBUGHARRIS
		if(dispImage) delete dispImage;
		SendingImagePort.interrupt();
		SendingImagePort.close();
	#endif


	
	
	std::cout<<"finished.\n";
}

void HarrisDetector::setParam(unsigned int cornerCount, double qualityLevel, double minDistance) 
{

	#if DEBUGHARRIS
		std::cout<<"\n"<< nameclass << "::setParam";
	#endif
		
	m_cornerCount = cornerCount; //maximum number of corners returned
	m_qualityLevel = qualityLevel;  //threshold for corners
	m_minDistance = minDistance; // minimum distance between corners

	m_block_size = 3; // Size of the averaging block, passed to underlying cvCornerMinEigenVal or cvCornerHarris
	m_use_harris = 0; // if 0, instead of cvCornerHarris, cvCornerMinEigenVal is used
	m_k = 0.04; // Free parameter of Harris detector; used only if use_harris≠0


	if(mp_corners) delete [] mp_corners;
	mp_corners = new CvPoint2D32f[m_cornerCount];

	if(mp_eig_image) cvReleaseImage(&mp_eig_image);
	mp_eig_image = NULL;

	if(mp_temp_image) cvReleaseImage(&mp_temp_image);
	mp_temp_image = NULL;



}

void HarrisDetector::execute() 
{

	#if DEBUGHARRIS
		std::cout<<"\n"<< nameclass << "::execute";
	#endif
	
	/*	READ AN IMAGE	*/	
	yarpimagein = ReceivingImagePort.read(&receiveimage);
	iplimagein = ( IplImage*)yarpimagein->getIplImage();
	
	if(!iplimagein)
	{ 
		std::cerr <<"\n"<< nameclass << "::execute()::ERROR::img is NULL!"; 
		return;
	}
	if(!grayimg)
	{
		grayimg = cvCreateImage(cvSize(iplimagein->width,iplimagein->height),IPL_DEPTH_8U,1);
	}

	/*	CONVERT IN GRAYIMAGE	*/
	cvCvtColor(iplimagein,grayimg, CV_BGR2GRAY);
	if(grayimg->nChannels != 1) 
	{ 
		std::cerr <<"\n"<< nameclass << "::execute()::ERROR::grayimg must be single-channel 8-bit image!"; 
		return; 
	}

	/*	CREATE IMAGE FOR HARRIS DETECTION	*/
	if(!mp_eig_image) {
		mp_eig_image = cvCreateImage(cvSize(iplimagein->width, iplimagein->height), IPL_DEPTH_32F, 1);
		mp_temp_image = cvCreateImage(cvSize(iplimagein->width, iplimagein->height), IPL_DEPTH_32F, 1);
	}

	/*	EXTRACT HARRIS CORNER POINT	*/
	int cornerCount = m_cornerCount;
	cvGoodFeaturesToTrack	(	grayimg,
					mp_eig_image,
					mp_temp_image,
					mp_corners,
					&(cornerCount),
					m_qualityLevel,
					m_minDistance,
					NULL,
					m_block_size,
					m_use_harris,
					m_k
				);
	if(cornerCount <= 0) {
		std::cerr <<"\n"<< nameclass << "::execute()::ERROR::cornerCount <=0!"; 
		return;
	}
	/*	STORE HARRIS CORNER POINT IN VECTOR KEYPOINTS	*/
	vector<cv::KeyPoint> HarrisKeys;
	for( int k=0 ;k<cornerCount ;k++ )
	{         
		KeyPoint *tmp = new KeyPoint(); 
		tmp->pt.x = mp_corners[k].x; 
		tmp->pt.y = mp_corners[k].y; 
		HarrisKeys.push_back( *tmp ); 
		if(tmp) delete tmp;
	} 
	
	/*	SEND A GRAY IMAGE	*/
	yarpgrimageout.wrapIplImage(grayimg);
	SendingGrImagePort.prepare()=yarpgrimageout;
	SendingGrImagePort.write();
	
	#if DEBUGHARRIS
		int number = HarrisKeys.size();
		std::cout<<"\n"<< nameclass << "::execute::Number of corners: "<< cornerCount;
		std::cout<<"\n"<< nameclass << "::execute::Number of HarrisKeys: "<< number<<"\n";
		
		std::cout<<"\n"<< nameclass << "::execute::First corners[x,y]: [" << mp_corners[0].x << "," << mp_corners[0].y << "]";
		std::cout<<"\n"<< nameclass << "::execute::First HarrisKey[x,y]: [" << HarrisKeys[0].pt.x << "," << HarrisKeys[0].pt.y << "]\n";
		
		std::cout<<"\n"<< nameclass << "::execute::Middle corners[x,y]: [" << mp_corners[(cornerCount-1)/2].x << "," << mp_corners[(cornerCount-1)/2].y << "]";
		std::cout<<"\n"<< nameclass << "::execute::Middle HarrisKey[x,y]: [" << HarrisKeys[(number-1)/2].pt.x << "," << HarrisKeys[(number-1)/2].pt.y << "]\n";
		
		std::cout<<"\n"<< nameclass << "::execute::Last corners[x,y]: [" << mp_corners[cornerCount-1].x << "," << mp_corners[cornerCount-1].y << "]";
		std::cout<<"\n"<< nameclass << "::execute::Last HarrisKey[x,y]: [" << HarrisKeys[number-1].pt.x << "," << HarrisKeys[number-1].pt.y << "]\n";


		CvScalar target_color[4] = { // in RGB order
			{{   0,   0, 255,   0 }},  // blue
			{{   0, 255,   0,   0 }},  // green
			{{ 255,   0,   0,   0 }},  // red
			{{   255, 255, 0,   0 }}   // yellow
		};

		for( int i = 0; i < cornerCount; i++) 
		{
			int radius = 1;
			cvCircle(iplimagein,
				cvPoint((int)(mp_corners[i].x + 0.5f),(int)(mp_corners[i].y + 0.5f)),
				radius,
				target_color[3]);
		}

        yarpimageout.wrapIplImage(iplimagein);
		SendingImagePort.prepare()=yarpimageout;
		SendingImagePort.write();
		dispImage->execute();
		//cvWaitKey(0);
	#endif	

	/*	SEND HARRIS CORNER POINT	*/	
	Bottle& HarrisOut = SendingHarrisPort.prepare();
    HarrisOut.clear();
	HarrisOut.addInt(cornerCount);
	for (int i = 0; i<cornerCount; i++)
	{
		HarrisOut.addDouble(mp_corners[i].x);
		HarrisOut.addDouble(mp_corners[i].y);
	}	
	SendingHarrisPort.write();
	

	
	
}
