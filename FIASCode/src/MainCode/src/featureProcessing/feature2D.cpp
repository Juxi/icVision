#include "iCub/featureProcessing/feature2D.h"


feature2D::feature2D(std::string name, std::string side)//, std::string input)
//void feature2D::feature2DInit(std::string side)//, std::string input)
{
    /*std::string name;
    switch(SELECT_FEATURE)
    {
    case HARRIS_CORNER:
        name = "Harris";
        break;
    case FAST_FEATURE:
        name = "Harris";
        break;
    case STAR_FEATURE:
        name = "Harris";
        break;
    case MSER_FEATURE:
        name = "Harris";
        break;
    case SIFT_FEATURE:
        name = "Harris";
        break;
    case SURF_FEATURE:
        name = "Harris";
        break;
    }*/

    Network yarp; 						//	set up yarp

    namesideclass = side;				//	"left or right" (suppose "left")
    nameclass =name + "_" + side;		//	"Harris_left"
    //nameinput = input;					//	"Camera"
    nameinput = "Camera_" + side;

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
    /*if(side=="left")
        dispImage->DisplayIplImage("DisplayHarris",namesideclass.c_str(),nameclass.c_str(),0,0);
    if(side=="right")
        dispImage->DisplayIplImage("DisplayHarris",namesideclass.c_str(),nameclass.c_str(),320,0);*/
#endif


    std::cout<<"finished.\n";
}

feature2D::~feature2D()
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

void feature2D::setParam(unsigned int cornerCount, double qualityLevel, double minDistance)
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

void feature2D::execute()
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

    switch(SELECT_FEATURE)
    {
    case HARRIS_CORNER:
        std::cout<<"Calculating Harris"<<endl;
        HarrisExecute();
        break;
    case FAST_FEATURE:
        //std::cout<<endl<<"Calculating FAST"<<endl;
        FastFeatureExecute();
        break;
    case STAR_FEATURE:
        std::cout<<endl<<"Calculating STAR"<<endl;
        StarFeatureExecute();
        break;
    case MSER_FEATURE:
        std::cout<<endl<<"Calculating MSER"<<endl;
        MserFeatureExecute();
        break;
    case SIFT_FEATURE:
        std::cout<<endl<<"Calculating SIFT"<<endl;
        SiftFeatureExecute();
        break;
    case SURF_FEATURE:
        std::cout<<endl<<"Calculating SURF"<<endl;
        SurfFeatureExecute();
        break;
    default:
        std::cout<<"Please abort, select one of the features and execute the code again"<<endl;
        break;
    }



}

void feature2D::HarrisExecute()
{
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
void feature2D::FastFeatureExecute()
{
    vector<cv::KeyPoint> FASTkeypoints;
    FAST(grayimg,FASTkeypoints,10,true);

    /*	SEND A GRAY IMAGE	*/
    yarpgrimageout.wrapIplImage(grayimg);
    SendingGrImagePort.prepare()=yarpgrimageout;
    SendingGrImagePort.write();

#if DEBUGHARRIS

    CvScalar target_color[4] = { // in RGB order
                                 {{   0,   0, 255,   0 }},  // blue
                                 {{   0, 255,   0,   0 }},  // green
                                 {{ 255,   0,   0,   0 }},  // red
                                 {{   255, 255, 0,   0 }}   // yellow
                               };

    for( int i = 0; i < FASTkeypoints.size(); i++)
    {
        int radius = 1;
        cvCircle(iplimagein,
                 cvPoint((int)(FASTkeypoints[i].pt.x + 0.5f),(int)(FASTkeypoints[i].pt.y + 0.5f)),
                 radius,
                 target_color[3]);
    }

    yarpimageout.wrapIplImage(iplimagein);
    SendingImagePort.prepare()=yarpimageout;
    SendingImagePort.write();
    dispImage->execute();
    //cvWaitKey(0);
#endif

    //std::cout<<"Fast keypoints"<<endl;
    Bottle& HarrisOut = SendingHarrisPort.prepare();
    HarrisOut.clear();
    HarrisOut.addInt(FASTkeypoints.size());
    //std::cout<<"count "<<FASTkeypoints.size()<<endl;
    for (int i = 0; i<FASTkeypoints.size(); i++)
    {
        HarrisOut.addDouble((double)FASTkeypoints[i].pt.x);
        HarrisOut.addDouble((double)FASTkeypoints[i].pt.y);
        //std::cout<<FASTkeypoints[i].pt.x<<" "<<FASTkeypoints[i].pt.y<<endl;
    }
    SendingHarrisPort.write();
    FASTkeypoints.clear();
}

void feature2D::StarFeatureExecute()
{
    //StarDetector starFeature(128,10,10,8,5);
    StarDetector starFeature;
    starFeature.responseThreshold = 10;
    vector<cv::KeyPoint> STARkeypoints;
    starFeature.operator ()(iplimagein,STARkeypoints);

    /*	SEND A GRAY IMAGE	*/
    yarpgrimageout.wrapIplImage(grayimg);
    SendingGrImagePort.prepare()=yarpgrimageout;
    SendingGrImagePort.write();

#if DEBUGHARRIS

    CvScalar target_color[4] = { // in RGB order
                                 {{   0,   0, 255,   0 }},  // blue
                                 {{   0, 255,   0,   0 }},  // green
                                 {{ 255,   0,   0,   0 }},  // red
                                 {{   255, 255, 0,   0 }}   // yellow
                               };

    for( int i = 0; i < STARkeypoints.size(); i++)
    {
        int radius = 1;
        cvCircle(iplimagein,
                 cvPoint((int)(STARkeypoints[i].pt.x + 0.5f),(int)(STARkeypoints[i].pt.y + 0.5f)),
                 radius,
                 target_color[3]);
    }

    yarpimageout.wrapIplImage(iplimagein);
    SendingImagePort.prepare()=yarpimageout;
    SendingImagePort.write();
    dispImage->execute();
    //cvWaitKey(0);
#endif

    std::cout<<"Star keypoints"<<endl;
    Bottle& HarrisOut = SendingHarrisPort.prepare();
    HarrisOut.clear();
    HarrisOut.addInt(STARkeypoints.size());
    //std::cout<<"count "<<STARkeypoints.size()<<endl;
    for (int i = 0; i<STARkeypoints.size(); i++)
    {
        HarrisOut.addDouble((double)STARkeypoints[i].pt.x);
        HarrisOut.addDouble((double)STARkeypoints[i].pt.y);
        //std::cout<<STARkeypoints[i].pt.x<<" "<<STARkeypoints[i].pt.y<<endl;
    }
    SendingHarrisPort.write();
    STARkeypoints.clear();
}

void feature2D::MserFeatureExecute()
{
    //StarDetector starFeature(128,10,10,8,5);
    MserFeatureDetector mserFeature;
    vector<cv::KeyPoint> MSERKeypoints;
    mserFeature.detect(iplimagein,MSERKeypoints);

    yarpgrimageout.wrapIplImage(grayimg);
    SendingGrImagePort.prepare()=yarpgrimageout;
    SendingGrImagePort.write();

#if DEBUGHARRIS

    CvScalar target_color[4] = { // in RGB order
                                 {{   0,   0, 255,   0 }},  // blue
                                 {{   0, 255,   0,   0 }},  // green
                                 {{ 255,   0,   0,   0 }},  // red
                                 {{   255, 255, 0,   0 }}   // yellow
                               };

    for( int i = 0; i < MSERKeypoints.size(); i++)
    {
        int radius = 1;
        cvCircle(iplimagein,
                 cvPoint((int)(MSERKeypoints[i].pt.x + 0.5f),(int)(MSERKeypoints[i].pt.y + 0.5f)),
                 radius,
                 target_color[3]);
    }

    yarpimageout.wrapIplImage(iplimagein);
    SendingImagePort.prepare()=yarpimageout;
    SendingImagePort.write();
    dispImage->execute();
    //cvWaitKey(0);
#endif

    std::cout<<"Mser keypoints"<<endl;
    Bottle& HarrisOut = SendingHarrisPort.prepare();
    HarrisOut.clear();
    HarrisOut.addInt(MSERKeypoints.size());
    //std::cout<<"count "<<MSERKeypoints.size()<<endl;
    for (int i = 0; i<MSERKeypoints.size(); i++)
    {
        HarrisOut.addDouble((double)MSERKeypoints[i].pt.x);
        HarrisOut.addDouble((double)MSERKeypoints[i].pt.y);
        //std::cout<<MSERKeypoints[i].pt.x<<" "<<MSERKeypoints[i].pt.y<<endl;
    }
    SendingHarrisPort.write();
    MSERKeypoints.clear();
}

void feature2D::SiftFeatureExecute()
{
    SiftFeatureDetector siftFeature;
    vector<cv::KeyPoint> SIFTkeypoints;
    siftFeature.detect(iplimagein,SIFTkeypoints);

    yarpgrimageout.wrapIplImage(grayimg);
    SendingGrImagePort.prepare()=yarpgrimageout;
    SendingGrImagePort.write();

#if DEBUGHARRIS

    CvScalar target_color[4] = { // in RGB order
                                 {{   0,   0, 255,   0 }},  // blue
                                 {{   0, 255,   0,   0 }},  // green
                                 {{ 255,   0,   0,   0 }},  // red
                                 {{   255, 255, 0,   0 }}   // yellow
                               };

    for( int i = 0; i < SIFTkeypoints.size(); i++)
    {
        int radius = 1;
        cvCircle(iplimagein,
                 cvPoint((int)(SIFTkeypoints[i].pt.x + 0.5f),(int)(SIFTkeypoints[i].pt.y + 0.5f)),
                 radius,
                 target_color[3]);
    }

    yarpimageout.wrapIplImage(iplimagein);
    SendingImagePort.prepare()=yarpimageout;
    SendingImagePort.write();
    dispImage->execute();
    //cvWaitKey(0);
#endif

    std::cout<<"Sift keypoints"<<endl;
    Bottle& HarrisOut = SendingHarrisPort.prepare();
    HarrisOut.clear();
    HarrisOut.addInt(SIFTkeypoints.size());
    //std::cout<<"count "<<SIFTkeypoints.size()<<endl;
    for (int i = 0; i<SIFTkeypoints.size(); i++)
    {
        HarrisOut.addDouble((double)SIFTkeypoints[i].pt.x);
        HarrisOut.addDouble((double)SIFTkeypoints[i].pt.y);
        //std::cout<<SIFTkeypoints[i].pt.x<<" "<<SIFTkeypoints[i].pt.y<<endl;
    }
    SendingHarrisPort.write();
    SIFTkeypoints.clear();
}

void feature2D::SurfFeatureExecute()
{
    SurfFeatureDetector surfFeature;
    vector<cv::KeyPoint> SURFkeypoints;
    surfFeature.detect(iplimagein,SURFkeypoints);

    yarpgrimageout.wrapIplImage(grayimg);
    SendingGrImagePort.prepare()=yarpgrimageout;
    SendingGrImagePort.write();

#if DEBUGHARRIS

    CvScalar target_color[4] = { // in RGB order
                                 {{   0,   0, 255,   0 }},  // blue
                                 {{   0, 255,   0,   0 }},  // green
                                 {{ 255,   0,   0,   0 }},  // red
                                 {{   255, 255, 0,   0 }}   // yellow
                               };

    for( int i = 0; i < SURFkeypoints.size(); i++)
    {
        int radius = 1;
        cvCircle(iplimagein,
                 cvPoint((int)(SURFkeypoints[i].pt.x + 0.5f),(int)(SURFkeypoints[i].pt.y + 0.5f)),
                 radius,
                 target_color[3]);
    }

    yarpimageout.wrapIplImage(iplimagein);
    SendingImagePort.prepare()=yarpimageout;
    SendingImagePort.write();
    dispImage->execute();
    //cvWaitKey(0);
#endif

    std::cout<<"Surf keypoints"<<endl;
    Bottle& HarrisOut = SendingHarrisPort.prepare();
    HarrisOut.clear();
    HarrisOut.addInt(SURFkeypoints.size());
    //std::cout<<"count "<<SURFkeypoints.size()<<endl;
    for (int i = 0; i<SURFkeypoints.size(); i++)
    {
        HarrisOut.addDouble((double)SURFkeypoints[i].pt.x);
        HarrisOut.addDouble((double)SURFkeypoints[i].pt.y);
        //std::cout<<SURFkeypoints[i].pt.x<<" "<<SURFkeypoints[i].pt.y<<endl;
    }
    SendingHarrisPort.write();
    SURFkeypoints.clear();
}
