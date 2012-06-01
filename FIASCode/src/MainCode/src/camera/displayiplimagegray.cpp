#include "iCub/camera/displayiplimagegray.h"
#include "highgui.h"

extern char pathResults[250];
extern int checkTime();

DisplayIplImageGray::DisplayIplImageGray(std::string name, std::string side, std::string input, int x, int y) 
{

    Network yarp;

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

    posx = x;
    posy = y;
    createwindow = false;

    //yarpview
    yarpviewName =  "/learning/displaygrayimage/" + namesideclass + "/out";//[/learning/displaygrayimage/left/out]
    DisplayGrayImageYarpViewPort.open(yarpviewName.c_str());
    iplimageinyarpview1 = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);

#if SAVEGRAYIMAGE
    number = 0;
    svName = &nameclass[0];
    //TODO
    /*SET THE PATH*/
    sprintf(FolderImagePath,"%s",pathResults);
    sprintf(command,"mkdir %s/%s",FolderImagePath,svName);
    system(command);
    //strcpy(FolderImagePath,"/home/pramod/robot/GABOR_WITH_RECOGNITION/AutonomousVisionSystem20aug2011/Image/AutonomousSystem/IntrinsicMotivation");
#endif

    std::cout<<"finished.\n";
}

DisplayIplImageGray::~DisplayIplImageGray() 
{

    std::cout<<"\nDestructing " << nameclass << "...";

    /*	READ AN IMAGE	*/
    ReceivingImagePort.interrupt();
    ReceivingImagePort.close();

#if DISPLAYGRAYIMAGE
    cvDestroyWindow(nameclass.c_str());
#endif

    //yarpview
    DisplayGrayImageYarpViewPort.interrupt();
    DisplayGrayImageYarpViewPort.close();
    cvReleaseImage(&iplimageinyarpview1);

    std::cout<<"finished.\n";
}

void DisplayIplImageGray::createWindow()
{
    cvNamedWindow( nameclass.c_str(), 0);
    cvMoveWindow( nameclass.c_str(),posx,posy);
    createwindow = true;
}

void DisplayIplImageGray::destroyWindow()
{
    cvDestroyWindow(nameclass.c_str());
    createwindow = false;
}

void DisplayIplImageGray::execute() 
{

#if DEBUGDISPLAY
    std::cout<<"\n"<< nameclass << "::execute";
#endif

#if DISPLAYGRAYIMAGE
    if (!createwindow)
        createWindow();
#endif

    /*	READ AN IMAGE	*/
    yarpimagein = ReceivingImagePort.read(&receiveimage);
    iplimagein = ( IplImage*)yarpimagein->getIplImage();

    /* DISPLAY AN IMAGE	*/
#if DISPLAYGRAYIMAGE
    cvShowImage(nameclass.c_str(), iplimagein);
    cvWaitKey(33);
#endif

    //yarpview
    /*cvCopy(iplimagein,iplimageinyarpview1);
    cvCvtColor(iplimageinyarpview1,iplimageinyarpview1,CV_RGB2BGR);
    DisplayGrayImageYarpImageOut.wrapIplImage(iplimagein);
    DisplayGrayImageYarpViewPort.prepare()=DisplayGrayImageYarpImageOut;
    DisplayGrayImageYarpViewPort.write();*/

#if SAVEGRAYIMAGE
    //sprintf(saveName,"%s/%s/%s_%04d.jpg",FolderImagePath, svName,svName,number);
    sprintf(saveName,"%s/%s/%s_%04d.jpg",FolderImagePath, svName,svName,checkTime());
    cvSaveImage(saveName, iplimagein);
    number++;
#endif


}
