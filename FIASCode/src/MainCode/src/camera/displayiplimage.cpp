#include "iCub/camera/displayiplimage.h"
#include "highgui.h"

extern char pathResults[250];
extern double starttimerprogram, stoptimerprogram, timeprogram;
extern int checkTime();

DisplayIplImage::DisplayIplImage(std::string name, std::string side, std::string input, int x, int y) 
{
    /*SET UP YARP NETWORK*/
    Network yarp;

    /*NAMESIDECLASS = side (left or right)
 NAMECLASS = DisplayImage_side
 NAMEINPUT = Camera_side*/
    namesideclass = side;
    nameclass =name + "_" + side;
    nameinput = input;

    std::cout<<"\nConstructing " << nameclass << "...\n";

    /*	READ AN IMAGE	*/
    nameImageInPort = "/image/" + nameclass + "/" + namesideclass + "/in";	//[image/DisplayImage_left/left/in]
    nameSenderImagePort = "/image/" + nameinput + "/" + namesideclass + "/out";	//[image/Camera_left/left/out]
    ReceivingImagePort.open(nameImageInPort.c_str());
    Network::connect(nameSenderImagePort.c_str(),nameImageInPort.c_str());
    receiveimage = false;
    yarpimagein = NULL;
    iplimagein = NULL;

    /*VARIABLES WINDOW*/
    posx = x;
    posy = y;
    createwindow = false;

#if SAVEIPLIMAGE
    number = 0;
    svName = &nameclass[0];
    //TODO
    /*SET THE PATH*/
    sprintf(FolderImagePath,"%s",pathResults);
    //strcpy(FolderImagePath,pathResults);
    //strcpy(FolderImagePath,"/home/pramod/robot/GABOR_WITH_RECOGNITION/AutonomousVisionSystem20aug2011/Image/AutonomousSystem/IntrinsicMotivation");
    sprintf(command,"mkdir %s/%s",FolderImagePath,svName);
    system(command);

#endif

    //yarpview
    yarpviewName =  "/learning/displayimage/" + namesideclass + "/out";//[/learning/displayimage/left/out]
    DisplayImageYarpViewPort.open(yarpviewName.c_str());
    iplimageinyarpview1 = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);

    std::cout<<"finished.\n";
}

DisplayIplImage::~DisplayIplImage() 
{

    std::cout<<"\nDestructing " << nameclass << "...";

    /*	READ AN IMAGE	*/
    ReceivingImagePort.interrupt();
    ReceivingImagePort.close();

#if DISPLAYIPLIMAGE
    cvDestroyWindow(nameclass.c_str());
#endif

    DisplayImageYarpViewPort.interrupt();
    DisplayImageYarpViewPort.close();
    cvReleaseImage(&iplimageinyarpview1);

    std::cout<<"finished.\n";
}

void DisplayIplImage::createWindow()
{
#if DEBUGDISPLAY
    std::cout<<endl<< nameclass << "::createWindow";
#endif

    cvNamedWindow( nameclass.c_str(), 0);
    cvMoveWindow( nameclass.c_str(),posx,posy);
    createwindow = true;
}

void DisplayIplImage::destroyWindow()
{
#if DEBUGDISPLAY
    std::cout<<endl<< nameclass << "::destroyWindow";
#endif

    cvDestroyWindow(nameclass.c_str());
    createwindow = false;
}

void DisplayIplImage::execute() 
{

#if DEBUGDISPLAY
    std::cout<<"\n"<< nameclass << "::execute";
#endif

#if DISPLAYIPLIMAGE
    if (!createwindow)
        createWindow();
#endif

    /*	READ AN IMAGE	*/
    yarpimagein = ReceivingImagePort.read(&receiveimage);
    iplimagein = ( IplImage*)yarpimagein->getIplImage();
    cvCvtColor(iplimagein,iplimagein, CV_BGR2RGB);
    //cvCvtColor(iplimagein,iplimagein,CV_RGB2BGR);

    /* DISPLAY AN IMAGE	*/
#if DISPLAYIPLIMAGE
    cvShowImage(nameclass.c_str(), iplimagein);
    cvWaitKey(33);
#endif

    //yarpview
    iplimageinyarpview1 = cvCloneImage(iplimagein);
    //cvCopy(iplimagein,iplimageinyarpview1);
    cvCvtColor(iplimageinyarpview1,iplimageinyarpview1,CV_RGB2BGR);
    DisplayImageYarpImageOut.wrapIplImage(iplimageinyarpview1);
    DisplayImageYarpViewPort.prepare()=DisplayImageYarpImageOut;
    DisplayImageYarpViewPort.write();
    //cvReleaseImage(&iplimageinyarpview1);

#if SAVEIPLIMAGE
    //sprintf(saveName,"%s/%s/%s_%04d.jpg",FolderImagePath,svName,svName,number);
    sprintf(saveName,"%s/%s/%s_%04d.jpg",FolderImagePath,svName,svName,checkTime());
    cvSaveImage(saveName, iplimagein);
    number++;
#endif


}
