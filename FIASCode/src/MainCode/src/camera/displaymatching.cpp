#include "iCub/camera/displaymatching.h"

using namespace std;
extern char pathResults[250];
extern int checkTime();
DisplayMatching::DisplayMatching(std::string name,std::string input,int x, int y)
{

    /*SET UP YARP NETWORK*/
    Network yarp;

    /*NAMECLASS = DisplayMatch
 NAMEINPUT = Match*/
    nameclass =name;
    nameinput = input;

    std::cout<<"\nConstructing " << nameclass << "...\n";

    /*	READ IMAGE	LEFT	*/
    nameImageLeftInPort = "/image/" + nameclass + "/left/in";
    nameSenderImageLeftPort = "/image/Camera_left/left/out";
    ReceivingImageLeftPort.open(nameImageLeftInPort.c_str());
    Network::connect(nameSenderImageLeftPort.c_str(),nameImageLeftInPort.c_str());
    receiveimageleft = false;
    yarpimageleftin = NULL;
    iplimageleftin = NULL;

    /*	READ IMAGE	RIGHT	*/
    nameImageRightInPort = "/image/" + nameclass + "/right/in";		//[image/DisplayMatch/left/in]
    nameSenderImageRightPort = "/image/Camera_right/right/out";	//[image/Camera_left/left/in]
    ReceivingImageRightPort.open(nameImageRightInPort.c_str());
    Network::connect(nameSenderImageRightPort.c_str(),nameImageRightInPort.c_str());
    receiveimageright = false;
    yarpimagerightin = NULL;
    iplimagerightin = NULL;

    /*	READ HARRIS	LEFT*/
    nameHarrisLeftInPort = "/harris/" + nameclass + "/left/in";							//[harris/DisplayMatch/left/in]
    nameSenderHarrisLeftPort = "/harris/" + nameinput + "/left/out";							//[harris/Match/left/out]
    ReceivingHarrisLeftPort.open(nameHarrisLeftInPort.c_str());
    Network::connect(nameSenderHarrisLeftPort.c_str(),nameHarrisLeftInPort.c_str());
    receiveharrisleft = false;
    HarrisLeftIn=NULL;
    mp_cornersleft = NULL;

    /*	READ HARRIS	RIGHT*/
    nameHarrisRightInPort = "/harris/" + nameclass + "/right/in";						//[harris/DisplayMatch/right/in]
    nameSenderHarrisRightPort = "/harris/" + nameinput + "/right/out";						//[harris/Match/right/out]
    ReceivingHarrisRightPort.open(nameHarrisRightInPort.c_str());
    Network::connect(nameSenderHarrisRightPort.c_str(),nameHarrisRightInPort.c_str());
    receiveharrisright = false;
    HarrisRightIn=NULL;
    mp_cornersright = NULL;

    /*	DISPLAY MATCHING	*/
    sideBySideImage=NULL;
    posx = x;
    posy = y;
    createwindow = false;

    nameleftmatchwindow = "DisplayMatching_Left";
    namerightmatchwindow = "DisplayMatching_Right";
    nameleftrightmatchwindow = "DisplayMatching";

    //yarpview
    nameDisplayLeftMatchYarpView = "/learning/imageleftmatch/yarpview/out";
    DisplayLeftMatchYarpView.open(nameDisplayLeftMatchYarpView.c_str());
    DisplayRightMatchYarpView.open("/learning/imagerightmatch/yarpview/out");
    DisplayLeftRightMatchYarpView.open("/learning/imageleftrightmatch/yarpview/out");
    /*iplimageinLeftyarpview = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
    iplimageinRightyarpview = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
    iplimageinLeftRightyarpview = cvCreateImage(cvSize(640,240),IPL_DEPTH_8U,3);*/

#if SAVEMATCHIMAGE
    number = 0;
    svName1 = &nameleftmatchwindow[0];
    svName2 = &namerightmatchwindow[0];
    svName3 = &nameleftrightmatchwindow[0];
    //TODO
    /*SET THE PATH*/
    sprintf(FolderImagePath,"%s",pathResults);
    //strcpy(FolderImagePath,"/home/pramod/robot/GABOR_WITH_RECOGNITION/AutonomousVisionSystem20aug2011/Image/AutonomousSystem/IntrinsicMotivation");
    sprintf(command,"mkdir %s/%s",FolderImagePath,svName1);
    system(command);
    sprintf(command,"mkdir %s/%s",FolderImagePath,svName2);
    system(command);
    sprintf(command,"mkdir %s/%s",FolderImagePath,svName3);
    system(command);

#endif


    std::cout<<"finished.\n";

}

DisplayMatching::~DisplayMatching() 
{

    std::cout<<"\nDestructing " << nameclass << "...";

    /*	READ IMAGE LEFT		*/
    ReceivingImageLeftPort.interrupt();
    ReceivingImageLeftPort.close();
    //if(&iplimageleftin) cvReleaseImageLeft(&iplimageleftin);		//if uncommented gives SIGFAULT

    /*	READ IMAGE	RIGHT	*/
    ReceivingImageRightPort.interrupt();
    ReceivingImageRightPort.close();
    //if(&iplimagerightin) cvReleaseImageRight(&iplimagerightin);		//if uncommented gives SIGFAULT

    /*	READ HARRIS LEFT	*/
    ReceivingHarrisLeftPort.interrupt();
    ReceivingHarrisLeftPort.close();
    if(mp_cornersleft) delete [] mp_cornersleft;
    //if(HarrisKeys) delete HarrisKeys;

    /*	READ HARRIS RIGHT	*/
    ReceivingHarrisRightPort.interrupt();
    ReceivingHarrisRightPort.close();
    if(mp_cornersright) delete [] mp_cornersright;
    //if(HarrisKeys) delete HarrisKeys;

    /*	DISPLAYMATCH	*/
    cvDestroyWindow(nameleftrightmatchwindow.c_str());
    cvReleaseImage(&sideBySideImage);

    //yarpview
    DisplayLeftMatchYarpView.interrupt();
    DisplayLeftMatchYarpView.close();

    DisplayRightMatchYarpView.interrupt();
    DisplayRightMatchYarpView.close();

    DisplayLeftRightMatchYarpView.interrupt();
    DisplayLeftRightMatchYarpView.close();

    cvReleaseImage(&iplimageinLeftyarpview);
    cvReleaseImage(&iplimageinRightyarpview);
    cvReleaseImage(&iplimageinLeftRightyarpview);
    std::cout<<"finished.\n";

}


void DisplayMatching::destroyWindow()
{

#if DEBUGDISPLAYMATCHING
    std::cout<<endl<< nameclass << "::destroyWindow";
#endif

    cvDestroyWindow(nameleftmatchwindow.c_str());
    cvDestroyWindow(namerightmatchwindow.c_str());
    cvDestroyWindow(nameleftrightmatchwindow.c_str());
    createwindow = false;
}	

void DisplayMatching::createWindow()
{

#if DEBUGDISPLAYMATCHING
    std::cout<<endl<< nameclass << "::createWindow";
#endif

    /*	DISPLAY MATCH	*/
    cvNamedWindow(nameleftmatchwindow.c_str(), 0);
    cvMoveWindow(nameleftmatchwindow.c_str(),posx,posy);
    cvNamedWindow(namerightmatchwindow.c_str(), 0);
    cvMoveWindow(namerightmatchwindow.c_str(),posx + 320 ,posy);
    cvNamedWindow(nameleftrightmatchwindow.c_str(), 0);
    cvMoveWindow(nameleftrightmatchwindow.c_str(),posx,posy +240);
    createwindow = true;

}		

void DisplayMatching::readfromports()
{

#if DEBUGDISPLAYMATCHING
    std::cout<<endl<< nameclass << "::readfromports";
#endif

    /*	READ IMAGE	LEFT AND RIGHT	*/
    yarpimageleftin = ReceivingImageLeftPort.read(&receiveimageleft);
    yarpimagerightin = ReceivingImageRightPort.read(&receiveimageright);
    iplimageleftin = ( IplImage*)yarpimageleftin->getIplImage();
    iplimagerightin = ( IplImage*)yarpimagerightin->getIplImage();

    cvCvtColor(iplimageleftin,iplimageleftin, CV_BGR2RGB);
    cvCvtColor(iplimagerightin,iplimagerightin, CV_BGR2RGB);

    /*	READ HARRIS	LEFT AND RIGHT*/
    HarrisLeftIn = ReceivingHarrisLeftPort.read(&receiveharrisleft);
    HarrisRightIn = ReceivingHarrisRightPort.read(&receiveharrisright);
    int cornerCountLeft = HarrisLeftIn->get(0).asInt();
    int cornerCountRight = HarrisRightIn->get(0).asInt();
    if(cornerCountLeft <= 0 || cornerCountRight <= 0) {
        std::cerr <<"\n"<< nameclass << "::execute::ERROR::cornerCountLeft <= 0 or cornerCountRight <= 0!";
        return;
    }

    m_cornerCountLeft=cornerCountLeft;
    m_cornerCountRight=cornerCountRight;
    if(mp_cornersleft) delete [] mp_cornersleft;
    if(mp_cornersright) delete [] mp_cornersright;
    mp_cornersleft = new CvPoint2D32f[m_cornerCountLeft];
    mp_cornersright = new CvPoint2D32f[m_cornerCountRight];

    vector<cv::KeyPoint> HarrisLeftKeys;
    for( int k=1 ;k<2*cornerCountLeft+1 ;k=k+2 )
    {
        KeyPoint *tmpleft = new KeyPoint();
        tmpleft->pt.x = HarrisLeftIn->get(k).asDouble();
        mp_cornersleft[(k-1)/2].x = HarrisLeftIn->get(k).asDouble();
        tmpleft->pt.y = HarrisLeftIn->get(k+1).asDouble();
        mp_cornersleft[(k-1)/2].y = HarrisLeftIn->get(k+1).asDouble();
        HarrisLeftKeys.push_back( *tmpleft );
        if(tmpleft) delete tmpleft;
    }

    vector<cv::KeyPoint> HarrisRightKeys;
    for( int k=1 ;k<2*cornerCountRight+1 ;k=k+2 )
    {
        KeyPoint *tmpright = new KeyPoint();
        tmpright->pt.x = HarrisRightIn->get(k).asDouble();
        mp_cornersright[(k-1)/2].x = HarrisRightIn->get(k).asDouble();
        tmpright->pt.y = HarrisRightIn->get(k+1).asDouble();
        mp_cornersright[(k-1)/2].y = HarrisRightIn->get(k+1).asDouble();
        HarrisRightKeys.push_back( *tmpright );
        if(tmpright) delete tmpright;
    }

#if DEBUGDISPLAYMATCHING
    int numberleft = HarrisLeftKeys.size();
    int numberright = HarrisRightKeys.size();

    std::cout<<"\n"<< nameclass << "::execute::Number of corners left: "<< m_cornerCountLeft;
    std::cout<<"\n"<< nameclass << "::execute::Number of corners right: "<< m_cornerCountRight;

    std::cout<<"\n"<< nameclass << "::execute::Number of HarrisKeys left: "<< numberleft;
    std::cout<<"\n"<< nameclass << "::execute::Number of HarrisKeys right: "<< numberright <<"\n";

    std::cout<<"\n"<< nameclass << "::execute::cornersleft[0] = [" << mp_cornersleft[0].x << "," << mp_cornersleft[0].y << "]";
    std::cout<<"\n"<< nameclass << "::execute::cornersright[0] = [" << mp_cornersright[0].x << "," << mp_cornersright[0].y << "]";

    std::cout<<"\n"<< nameclass << "::execute::HarrisLetKey[0]: [" << HarrisLeftKeys[0].pt.x << "," << HarrisLeftKeys[0].pt.y << "]";
    std::cout<<"\n"<< nameclass << "::execute::HarrisRightKey[0]: [" << HarrisRightKeys[0].pt.x << "," << HarrisRightKeys[0].pt.y << "]\n";

    std::cout<<"\n"<< nameclass << "::execute::cornersleft["<<(cornerCountLeft-1)/2<<"] = ["<< mp_cornersleft[(cornerCountLeft-1)/2].x <<
                "," << mp_cornersleft[(cornerCountLeft-1)/2].y << "]";
    std::cout<<"\n"<< nameclass << "::execute::cornersright["<<(cornerCountRight-1)/2<<"] = ["<< mp_cornersright[(cornerCountRight-1)/2].x <<
                "," << mp_cornersright[(cornerCountRight-1)/2].y << "]";

    std::cout<<"\n"<< nameclass << "::execute::HarrisLeftKey["<<(numberleft-1)/2<<"] = ["<< HarrisLeftKeys[(numberleft-1)/2].pt.x <<
                "," << HarrisLeftKeys[(numberleft-1)/2].pt.y << "]";
    std::cout<<"\n"<< nameclass << "::execute::HarrisRightKey["<<(numberright-1)/2<<"] = ["<< HarrisRightKeys[(numberright-1)/2].pt.x <<
                "," << HarrisRightKeys[(numberright-1)/2].pt.y << "]\n";

    std::cout<<"\n"<< nameclass << "::execute::cornersleft["<<(cornerCountLeft-1)<<"] = [" << mp_cornersleft[cornerCountLeft-1].x <<
                "," << mp_cornersleft[cornerCountLeft-1].y << "]";
    std::cout<<"\n"<< nameclass << "::execute::cornersright["<<(cornerCountRight-1)<<"] = [" << mp_cornersright[cornerCountRight-1].x <<
                "," << mp_cornersright[cornerCountRight-1].y << "]";

    std::cout<<"\n"<< nameclass << "::execute::HarrisLeftKey["<<(numberleft-1)<<"] = [" << HarrisLeftKeys[numberleft-1].pt.x <<
                "," << HarrisLeftKeys[numberleft-1].pt.y << "]";
    std::cout<<"\n"<< nameclass << "::execute::HarrisRightKey["<<(numberright-1)<<"] = [" << HarrisRightKeys[numberright-1].pt.x <<
                "," << HarrisRightKeys[numberright-1].pt.y << "]\n";

    //cvWaitKey(0);
#endif

}	


// Main function of the class
void DisplayMatching::execute()
{

#if DEBUGDISPLAYMATCHING
    std::cout<<"\n"<< nameclass << "::execute\n";
#endif

    readfromports();

#if DISPLAYMATCHIMAGE
    if(!createwindow)
        createWindow();
#endif


    CvScalar target_color[4] = { // in RGB order
                                 {{   0,   0, 255,   0 }},  // blue
                                 {{   0, 255,   0,   0 }},  // green
                                 {{ 255,   0,   0,   0 }},  // red
                                 {{   255, 255, 0,   0 }}   // yellow
                               };


    if(sideBySideImage==NULL)
    {
        sideBySideImage = cvCreateImage(cvSize(2 * iplimageleftin->width, iplimageleftin->height), iplimageleftin->depth, iplimageleftin->nChannels);
    }
    else
    {
        cvReleaseImage(&sideBySideImage);
        sideBySideImage = cvCreateImage(cvSize(2 * iplimageleftin->width, iplimageleftin->height), iplimageleftin->depth, iplimageleftin->nChannels);
    }

    putImagesSideBySide(sideBySideImage, iplimageleftin, iplimagerightin);

    showKeypoints();


#if DISPLAYMATCHIMAGE
    cvShowImage(nameleftmatchwindow.c_str(), iplimageleftin);
    cvShowImage(namerightmatchwindow.c_str(), iplimagerightin);
    cvShowImage(nameleftrightmatchwindow.c_str(), sideBySideImage);

    cvWaitKey(33);
#endif
    //yarpview
    //cvCopy(iplimageleftin,iplimageinLeftyarpview);
    iplimageinLeftyarpview =  cvCloneImage(iplimageleftin);
    cvCvtColor(iplimageinLeftyarpview,iplimageinLeftyarpview,CV_RGB2BGR);
    DisplayLeftMatchYarpImageOut.wrapIplImage(iplimageinLeftyarpview);
    DisplayLeftMatchYarpView.prepare()=DisplayLeftMatchYarpImageOut;
    DisplayLeftMatchYarpView.write();
    //cvReleaseImage(&iplimageinLeftyarpview);

    //cvCopy(iplimagerightin,iplimageinRightyarpview);
    iplimageinRightyarpview = cvCloneImage(iplimagerightin);
    cvCvtColor(iplimageinRightyarpview,iplimageinRightyarpview,CV_RGB2BGR);
    DisplayRightMatchYarpImageOut.wrapIplImage(iplimageinRightyarpview);
    DisplayRightMatchYarpView.prepare()=DisplayRightMatchYarpImageOut;
    DisplayRightMatchYarpView.write();
    //cvReleaseImage(&iplimageinRightyarpview);

    //cvCopy(sideBySideImage,iplimageinLeftRightyarpview);
    iplimageinLeftRightyarpview = cvCloneImage(sideBySideImage);
    //cvCvtColor(iplimageinLeftRightyarpview,iplimageinLeftRightyarpview,CV_RGB2BGR);
    DisplayLeftRightMatchYarpImageOut.wrapIplImage(iplimageinLeftRightyarpview);
    DisplayLeftRightMatchYarpView.prepare()=DisplayLeftRightMatchYarpImageOut;
    DisplayLeftRightMatchYarpView.write();
    //cvReleaseImage(&iplimageinLeftRightyarpview);

#if SAVEMATCHIMAGE
    /*sprintf(saveName1,"%s/%s/%s_%04d.jpg",FolderImagePath,svName1,svName1,number);
                sprintf(saveName2,"%s/%s/%s_%04d.jpg",FolderImagePath,svName2,svName2,number);
                sprintf(saveName3,"%s/%s/%s_%04d.jpg",FolderImagePath,svName3,svName3,number);*/
    sprintf(saveName1,"%s/%s/%s_%04d.jpg",FolderImagePath,svName1,svName1,checkTime());
    sprintf(saveName2,"%s/%s/%s_%04d.jpg",FolderImagePath,svName2,svName2,checkTime());
    sprintf(saveName3,"%s/%s/%s_%04d.jpg",FolderImagePath,svName3,svName3,checkTime());
    cvSaveImage(saveName1, iplimageleftin);
    cvSaveImage(saveName2, iplimagerightin);
    cvSaveImage(saveName3, sideBySideImage);
    number++;
#endif

#if DEBUGDISPLAYMATCHING
    std::cout<<"finished.\n";
#endif
}


// Puts img1 and img2 side by side and stores into result
void DisplayMatching::putImagesSideBySide(IplImage* result, const IplImage* img1, const IplImage* img2)
{

#if DEBUGDISPLAYMATCHING
    std::cout<<"\n"<< nameclass << "::putImagesSideBySide";
#endif

    // widthStep of the resulting image
    const int bigWS = result->widthStep;
    // half of the widthStep of the resulting image
    const int bigHalfWS = result->widthStep >> 1;
    // widthStep of the image which will be put in the left
    const int lWS = img1->widthStep;
    // widthStep of the image which will be put in the right
    const int rWS = img2->widthStep;

    // pointer to the beginning of the left image
    char *p_big = result->imageData;
    // pointer to the beginning of the right image
    char *p_bigMiddle = result->imageData + bigHalfWS;
    // pointer to the image data which will be put in the left
    const char *p_l = img1->imageData;
    // pointer to the image data which will be put in the right
    const char *p_r = img2->imageData;

    for (int i = 0; i < 240; ++i, p_big += bigWS, p_bigMiddle += bigWS) {
        // copy a row of the left image till the half of the resulting image
        memcpy(p_big, p_l + i*lWS, lWS);
        // copy a row of the right image from the half of the resulting image to the end of it
        memcpy(p_bigMiddle, p_r + i*rWS, rWS);
    }

#if DEBUGDISPLAYMATCHING
    std::cout<<"finished.\n";
#endif

}

// Marks the keypoints with plus signs on img
void DisplayMatching::showKeypoints()
{

#if DEBUGDISPLAYMATCHING
    std::cout<<"\n"<< nameclass << "::showKeypoints";
#endif

    for(unsigned int i = 0;i<m_cornerCountLeft;i++) {
        cvCircle(iplimageleftin, cvPoint((int)(mp_cornersleft[i].x),(int)(mp_cornersleft[i].y)), 1, CV_RGB(255, 225, 0));
        cvCircle(iplimagerightin, cvPoint((int)(mp_cornersright[i].x),(int)(mp_cornersright[i].y)), 1, CV_RGB(255, 225, 0));
        float posxLeft = mp_cornersleft[i].x;
        float posyLeft = mp_cornersleft[i].y;
        float posxRight = mp_cornersright[i].x + iplimageleftin->width;
        float posyRight = mp_cornersright[i].y;
        drawAPlusandLine(posxLeft,posyLeft,posxRight,posyRight);
    }
    // Draw Fovea
    //cvCircle(sideBySideImage,cvPoint(160,120),70,CV_RGB(0,0,255));
    //cvCircle(sideBySideImage,cvPoint(160+320,120),70,CV_RGB(0,0,255));

    // Draw Center
    //cvCircle(sideBySideImage,cvPoint(160,120),3,CV_RGB(0,0,255),2);
    //cvCircle(sideBySideImage,cvPoint(160+320,120),3,CV_RGB(0,0,255),2);

#if DEBUGDISPLAYMATCHING
    std::cout<<"finished.\n";
#endif

}

// Draws a plus sign on img given (x, y) coordinate
void DisplayMatching::drawAPlusandLine(const int xL, const int yL, const int xR, const int yR)
{

#if DEBUGDISPLAYMATCHING
    std::cout<<"\n"<< nameclass << "::drawAPlusandLine";
#endif

    //Draw a line between matching keypoints
    cvLine(sideBySideImage, cvPoint(xL, yL), cvPoint(xR,yR), cvScalar(0, 255, 0), 0.5);

    //Draw Circle
    cvCircle(sideBySideImage, cvPoint(xL, yL), 1, CV_RGB(255, 0, 0));
    cvCircle(sideBySideImage, cvPoint(xR, yR), 1, CV_RGB(255, 0, 0));

    //Draw Plus
    /*
   cvLine(sideBySideImage, cvPoint(xL - 5, yL), cvPoint(xL + 5, yL), CV_RGB(255, 0, 0));
   cvLine(sideBySideImage, cvPoint(xL, yL - 5), cvPoint(xL, yL + 5), CV_RGB(255, 0, 0));
   cvLine(sideBySideImage, cvPoint(xR - 5, yR), cvPoint(xR + 5, yR), CV_RGB(255, 0, 0));
   cvLine(sideBySideImage, cvPoint(xR, yR - 5), cvPoint(xR, yR + 5), CV_RGB(255, 0, 0));
 */
}
