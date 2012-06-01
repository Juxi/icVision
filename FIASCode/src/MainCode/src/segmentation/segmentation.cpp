#include "iCub/segmentation/segmentation.h"

extern char pathResults[250];
extern int checkTime();

Segmentation::Segmentation(std::string name)
{
    Network yarp;

    nameclass = name;

    std::cout<<"\nConstructing " << nameclass << "...\n";

    /*	READ LEFT IMAGE	*/
    nameImageLeftInPort = "/image/" + nameclass + "/left/in";		//[image/ClassTemplate_left/left/in]
    nameSenderImageLeftPort = "/image/Camera_left/left/out";	//[image/Camera_left/left/in]
    ReceivingImageLeftPort.open(nameImageLeftInPort.c_str());
    Network::connect(nameSenderImageLeftPort.c_str(),nameImageLeftInPort.c_str());
    receiveimageleft = false;
    yarpimageleftin = NULL;
    iplimageleftin = NULL;

    /*	READ RIGHT IMAGE	*/
    nameImageRightInPort = "/image/" + nameclass + "/right/in";		//[image/ClassTemplate_left/left/in]
    nameSenderImageRightPort = "/image/Camera_right/right/out";	//[image/Camera_left/left/in]
    ReceivingImageRightPort.open(nameImageRightInPort.c_str());
    Network::connect(nameSenderImageRightPort.c_str(),nameImageRightInPort.c_str());
    receiveimageright = false;
    yarpimagerightin = NULL;
    iplimagerightin = NULL;

    /*	READ HARRIS	LEFT*/
    nameHarrisLeftInPort = "/harris/" + nameclass + "/left/in";							//[harris/Match/left/in]
    nameSenderHarrisLeftPort = "/harris/Match/left/out";							//[harris/Harris_left/left/out]
    ReceivingHarrisLeftPort.open(nameHarrisLeftInPort.c_str());
    Network::connect(nameSenderHarrisLeftPort.c_str(),nameHarrisLeftInPort.c_str());
    receiveharrisleft = false;
    HarrisLeftIn=NULL;
    mp_cornersleft = NULL;

    /*	READ HARRIS	RIGHT*/
    nameHarrisRightInPort = "/harris/" + nameclass + "/right/in";						//[harris/Match/right/in]
    nameSenderHarrisRightPort = "/harris/Match/right/out";						//[harris/Harris_right/right/out]
    ReceivingHarrisRightPort.open(nameHarrisRightInPort.c_str());
    Network::connect(nameSenderHarrisRightPort.c_str(),nameHarrisRightInPort.c_str());
    receiveharrisright = false;
    HarrisRightIn=NULL;
    mp_cornersright = NULL;

    //TO SALIENCY CODE
    nameInhibitHarrisOutPort = "/InhibitHarris/left/out";//	[InhibitHarris/left/out]
    InhibitHarrisOutPort.open(nameInhibitHarrisOutPort.c_str());

    featureImage = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
    blobsAllVisitedLocations = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
    salimg = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
    imageObjects = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);

    cvSetZero(blobsAllVisitedLocations);
    cvSetZero(featureImage);
    cvSetZero(salimg);
    cvSetZero(imageObjects);

    /*RECEIVE PIXEL POINTS FROM MOTION*/
    PixelLocationsInPort.open("/motion/pixel_locations/in");
    Network::connect("/motion/pixel_locations/out","/motion/pixel_locations/in");

    //MOD
    createwindow = false;
    numberPoses = 0;
    objectCount = 0;
#if DISPLAYCLASSIMAGE
    if(!createwindow)
        createWindow();
#endif

    nameVisitedLocationBlobs = "VisitedLocationBlobs";
    nameTestWhenFocussed = "TestWhenFocussed";

    for(int i=0;i<OBJECT_LIMIT;i++)
    {
        poseIndex[i]=1;
    }

    std::cout<<"finished.\n";
}

Segmentation::~Segmentation()
{

    std::cout<<"\nDestructing " << nameclass << "...";

    /*	READ LEFT IMAGE	*/
    ReceivingImageLeftPort.interrupt();
    ReceivingImageLeftPort.close();
    //if(&iplimagein) cvReleaseImage(&iplimagein);		//if uncommented gives SIGFAULT

    /*	READ RIGHT IMAGE	*/
    ReceivingImageRightPort.interrupt();
    ReceivingImageRightPort.close();
    //if(&iplimagein) cvReleaseImage(&iplimagein);		//if uncommented gives SIGFAULT

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

    /*RECEIVE PIXEL POINTS FROM MOTION*/
    PixelLocationsInPort.interrupt();
    PixelLocationsInPort.close();

    cvReleaseImage(&blobsAllVisitedLocations);
    cvReleaseImage(&featureImage);
    cvReleaseImage(&salimg);
    cvReleaseImage(&imageObjects);

    //MOD
#if DISPLAYCLASSIMAGE
    cvDestroyWindow(nameTestWhenFocussed.c_str());
    cvDestroyWindow(nameVisitedLocationBlobs.c_str());
    cvDestroyWindow(nameObjectsDisplay.c_str());
#endif
    //MOD

    std::cout<<"finished.\n";
}


//MOD
void Segmentation::createWindow()
{

#if DEBUGSegmentation
    std::cout<<endl<< nameclass << "::createWindow";
#endif
    cvNamedWindow(nameTestWhenFocussed.c_str(),0);
    cvMoveWindow(nameTestWhenFocussed.c_str(),640,300);
    cvShowImage(nameTestWhenFocussed.c_str(),featureImage);
    cvWaitKey(33);

    cvNamedWindow(nameVisitedLocationBlobs.c_str(),0);
    cvMoveWindow(nameVisitedLocationBlobs.c_str(),960,300);
    cvShowImage(nameVisitedLocationBlobs.c_str(),blobsAllVisitedLocations);
    cvWaitKey(33);

    cvNamedWindow(nameObjectsDisplay.c_str(),0);
    cvMoveWindow(nameObjectsDisplay.c_str(),320,500);
    cvShowImage(nameObjectsDisplay.c_str(),imageObjects);
    cvWaitKey(33);


    createwindow = true;

}

void Segmentation::destroyWindow()
{

#if DEBUGSegmentation
    std::cout<<endl<< nameclass << "::destroyWindow";
#endif

    cvDestroyWindow(nameVisitedLocationBlobs.c_str());
    cvDestroyWindow(nameTestWhenFocussed.c_str());
    cvDestroyWindow(nameObjectsDisplay.c_str());

    createwindow = false;

}
//MOD

void Segmentation::VisitedLocationBlobDisplay(int iterationLocation,int LocX, int LocY)
{

    CvPoint newwinnerpos;

    newwinnerpos.x = LocX;
    newwinnerpos.y = LocY;


    int m_windowSize = 50,m_size=10*10;
    for(int x = -m_windowSize; x <= m_windowSize; ++x)
    {
        for(int y = -m_windowSize; y <= m_windowSize; ++y)
        {
            int posx = newwinnerpos.x + x;
            int posy = newwinnerpos.y + y;
            if(posx >= 0 && posy >= 0 && posx < iplimageleftin->width && posy < iplimageleftin->height)
            {
                double val = exp(-0.5/m_size*(x*x+y*y));

                (salimg->imageData + posy*salimg->widthStep)[posx*salimg->nChannels+0]=cvRound(colors[iterationLocation].val[0]*val);
                (salimg->imageData + posy*salimg->widthStep)[posx*salimg->nChannels+1]=cvRound(colors[iterationLocation].val[1]*val);
                (salimg->imageData + posy*salimg->widthStep)[posx*salimg->nChannels+2]=cvRound(colors[iterationLocation].val[2]*val);
            }
        }
    }
}


void Segmentation::setSelectHarris()
{

    selectHarris.assign(sizeGaborLeft,1);

    PixelLocationsData = NULL;
    PixelLocationsData = PixelLocationsInPort.read(true);
    countPixelsFromMotion = PixelLocationsData->get(0).asInt();
    for(int i=0;i<countPixelsFromMotion;i++)
    {
        bool valid = (bool)PixelLocationsData->get(3*i+1).asInt();
        pixelLocTemp.pxl = PixelLocationsData->get(3*i+2).asDouble();
        pixelLocTemp.pxr = PixelLocationsData->get(3*i+3).asDouble();
        //std::cout<<"validity"<<valid<<" pxl "<<pixelLocTemp.pxl<<" pxr "<<pixelLocTemp.pxr<<endl;
        if(valid)
        {
            pixelLocFromMotion.push_back(pixelLocTemp);
        }
    }

    array5 = segmentationInhibition();

}

void Segmentation::inhibitSamplesLocationHistory()
{
#if INHIBIT_VISITED_LOCATIONS
    for(int i=0;i<sizeGaborLeft;i++)
    {
        for(int j=0; j<pixelLocFromMotion.size(); j++)
        {
            if(array5[j]==1)
            {
                float featureImageX = mp_cornersleft[i].x;
                float featureImageY = mp_cornersleft[i].y;

                float dist = sqrt((featureImageX-pixelLocFromMotion[j].pxl)*(featureImageX-pixelLocFromMotion[j].pxl)+
                                  (featureImageY-pixelLocFromMotion[j].pxr)*(featureImageY-pixelLocFromMotion[j].pxr));
                if(dist<TOPDOWNTHRESHOLD)
                {
                    selectHarris[i]=0;

                }

            }
        }
    }
#endif
#if FOVEA
    for(int i=0;i<sizeGaborLeft;i++)
    {
        float featureImageX = mp_cornersleft[i].x;
        float featureImageY = mp_cornersleft[i].y;

        float dist = sqrt((featureImageX-160.0)*(featureImageX-160.0)+
                          (featureImageY-120.0)*(featureImageY-120.0)*4.0);
        if(dist>FOVEA_BOUNDARY)
        {
            selectHarris[i]=0;

        }
    }
#endif
#if DISPLAYINHIBITION
    cvSetZero(blobsAllVisitedLocations);
    for(unsigned int loc=0; loc<pixelLocFromMotion.size();loc++)
    {
        cvSetZero(salimg);
        VisitedLocationBlobDisplay(loc,pixelLocFromMotion[loc].pxl,pixelLocFromMotion[loc].pxr);
        cvAdd(salimg,blobsAllVisitedLocations,blobsAllVisitedLocations);
        cvCircle(blobsAllVisitedLocations,cvPoint(pixelLocFromMotion[loc].pxl,pixelLocFromMotion[loc].pxr),TOPDOWNTHRESHOLD,CV_RGB(255,255,255));
    }
    for(unsigned int k=0; k<sizeGaborLeft;k++)
    {
        if(selectHarris[k]==1 && array5[k]==1)
        {
            cvCircle(blobsAllVisitedLocations,cvPoint((int)mp_cornersleft[k].x,(int)mp_cornersleft[k].y),2,CV_RGB(255,255,255),CV_FILLED );
        }
    }
    cvMoveWindow(nameVisitedLocationBlobs.c_str(),960,300);
    cvShowImage(nameVisitedLocationBlobs.c_str(),blobsAllVisitedLocations);
    cvWaitKey(33);

#endif

}

void Segmentation::sendInhibitionToSaliency()
{

    Bottle& InhibitHarrisOut = InhibitHarrisOutPort.prepare();
    InhibitHarrisOut.clear();
    InhibitHarrisOut.addInt(sizeGaborLeft);
    for (int i = 0; i<sizeGaborLeft; i++)
    {
        int temp = selectHarris[i];
        InhibitHarrisOut.addInt(temp);
    }
    InhibitHarrisOutPort.write(true);
    std::cout<<"sent"<<endl;
    selectHarris.clear();
    pixelLocFromMotion.clear();
    free(array5);

}

void Segmentation::readfromport()
{

#if DEBUGSegmentation
    std::cout<<endl<< nameclass << "::readfromport"<<endl;
#endif

    /*	READ LEFT IMAGE	*/
    yarpimageleftin = ReceivingImageLeftPort.read(&receiveimageleft);
    iplimageleftin = ( IplImage*)yarpimageleftin->getIplImage();
    cvCvtColor(iplimageleftin,iplimageleftin, CV_BGR2RGB);

    /*	READ RIGHT IMAGE	*/
    yarpimagerightin = ReceivingImageRightPort.read(&receiveimageright);
    iplimagerightin = ( IplImage*)yarpimagerightin->getIplImage();
    cvCvtColor(iplimagerightin,iplimagerightin, CV_BGR2RGB);

    /*	READ HARRIS	LEFT AND RIGHT*/
    HarrisLeftIn = ReceivingHarrisLeftPort.read(&receiveharrisleft);
    HarrisRightIn = ReceivingHarrisRightPort.read(&receiveharrisright);
    int cornerCountLeft = HarrisLeftIn->get(0).asInt();
    int cornerCountRight = HarrisRightIn->get(0).asInt();
    if(cornerCountLeft <= 0 || cornerCountRight <= 0) {
        std::cerr <<"\n"<< nameclass << "::readfromport::ERROR::cornerCountLeft <= 0 or cornerCountRight <= 0!";
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

    sizeGaborLeft = m_cornerCountLeft;
    heightRegionLeft = m_cornerCountLeft;

}

int* Segmentation::segmentationInhibition()
{

    //Partitioning
    //TODO RELEASE array4
    int* array1 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array2 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array3 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array4 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array5 = (int*)malloc(heightRegionLeft*sizeof(int));
    for(unsigned int r1=0; r1<heightRegionLeft;r1++){
        array1[r1]=r1;
        array2[r1]=0;
        array3[r1]=0;
        array4[r1]=0;
        array5[r1]=0;
    }

    bool change=true;
    while(change)
    {
        change=false;
        for(unsigned int r1=0; r1<heightRegionLeft;r1++)
        {
            for(unsigned int r2=0; r2<heightRegionLeft;r2++)
            {
                if(r1!=r2)
                {
                    //We are using only the left image in order to cluster based on x and y coordinates
                    float distX = mp_cornersleft[r1].x - mp_cornersleft[r2].x;
                    float distY = mp_cornersleft[r1].y - mp_cornersleft[r2].y;
                    float distXY = sqrt(distX*distX+distY*distY/4.0);
                    float temp1 = mp_cornersleft[r1].x - mp_cornersleft[r2].x;
                    float temp2 = mp_cornersright[r1].x - mp_cornersright[r2].x;
                    float dispLeftRight = fabs(temp1 - temp2);
                    if(distXY<CLUSTERXY && dispLeftRight<CLUSTERDEPTH)
                    {
                        if(array1[r1]<array1[r2])
                        {
                            array1[r2]=array1[r1];
                            change=true;
                        }
                        if(array1[r2]<array1[r1])
                        {
                            array1[r1]=array1[r2];
                            change=true;
                        }
                    }
                }
            }
        }
    }

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        array2[array1[r1]]=1;
    }
    int count=0;

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        if(array2[r1])
        {
            array3[r1]=count;
        }
        else
        {
            count++;
        }
    }
    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        array1[r1]-=array3[array1[r1]];
    }

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {

        array4[array1[r1]]++;

    }

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        if(array4[array1[r1]]>2)
        {
            array5[r1]=1;
        }
    }


    free(array1);
    free(array2);
    free(array3);
    free(array4);
    return(array5);
}

void Segmentation::segmentObject(CvPoint PointOL,bool focussed)
{

    /*	READ THE WINNER FROM CUEINTEGRATION	*/
    WinnerL = PointOL;

    cvCopyImage(iplimageleftin,featureImage);

    array5 = segment(focussed);

    //cvCircle(featureImage,cvPoint(160,120),5,CV_RGB(255,0,0), CV_FILLED );

    //MOD
#if DISPLAYCLASSIMAGE
    cvMoveWindow(nameTestWhenFocussed.c_str(),640,300);
    cvShowImage(nameTestWhenFocussed.c_str(),featureImage);
    cvWaitKey(33);
#endif

    if(fileSave)
    {
        char filename[200];
        sprintf(filename,"%s/object%02d_image%02d.png",pathResults,objectCount, numberPoses);
        cvSaveImage(filename, iplimageleftin);
        sprintf(filename,"%s/object%02d_features%02d.png",pathResults,objectCount,numberPoses);
        cvSaveImage(filename, featureImage);
    }

    free(array5);
    return;
}

int* Segmentation::segment(bool zerodisp)
{

    unsigned int pointsbehind = 0, pointsinfront = 0, pointson = 0;

    //Partitioning
    //TODO RELEASE array4
    int* array1 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array2 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array3 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array4 = (int*)malloc(heightRegionLeft*sizeof(int));
    int* array5 = (int*)malloc(heightRegionLeft*sizeof(int));
    for(unsigned int r1=0; r1<heightRegionLeft;r1++){
        array1[r1]=r1;
        array2[r1]=0;
        array3[r1]=0;
        array4[r1]=0;
        array5[r1]=0;
    }

    bool change=true;
    while(change)
    {
        change=false;
        for(unsigned int r1=0; r1<heightRegionLeft;r1++)
        {
            for(unsigned int r2=0; r2<heightRegionLeft;r2++)
            {
                if(r1!=r2)
                {
                    //We are using only the left image in order to cluster based on x and y coordinates
                    float distX = mp_cornersleft[r1].x - mp_cornersleft[r2].x;
                    float distY = mp_cornersleft[r1].y - mp_cornersleft[r2].y;
                    float distXY = sqrt(distX*distX+distY*distY/4.0);
                    float temp1 = mp_cornersleft[r1].x - mp_cornersleft[r2].x;
                    float temp2 = mp_cornersright[r1].x - mp_cornersright[r2].x;
                    float dispLeftRight = fabs(temp1 - temp2);

                    if(distXY<CLUSTERXY && dispLeftRight<CLUSTERDEPTH)
                    {

                        if(array1[r1]<array1[r2])
                        {
                            array1[r2]=array1[r1];
                            change=true;
                        }
                        if(array1[r2]<array1[r1])
                        {
                            array1[r1]=array1[r2];
                            change=true;
                        }
                    }
                }
            }
        }
    }

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        array2[array1[r1]]=1;
    }
    int count=0;

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        if(array2[r1])
        {
            array3[r1]=count;
        }
        else
        {
            count++;
        }
    }
    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        array1[r1]-=array3[array1[r1]];
    }
    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        array4[array1[r1]]++;
    }

    int minDistClusterInd=-1; float minDistCluster=1000.0;
    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {
        int tempDispX,tempDispY;
        if(zerodisp)
        {
            tempDispX = WIDTHSEGMENTATION; tempDispY = WIDTHSEGMENTATION;
        }
        else
        {
            //tempDispX = 10; tempDispY = 10;
            tempDispX = WIDTHSEGMENTATION; tempDispY = WIDTHSEGMENTATION;
        }
        if(abs(mp_cornersleft[r1].x-WinnerL.x)<tempDispX && abs(mp_cornersleft[r1].y-WinnerL.y)<tempDispY)
        {
            float tempDist = sqrt((mp_cornersleft[r1].x-WinnerL.x)*(mp_cornersleft[r1].x-WinnerL.x)
                                  +(mp_cornersleft[r1].y-WinnerL.y)*(mp_cornersleft[r1].y-WinnerL.y));
            if(tempDist<minDistCluster)
            {
                minDistCluster=tempDist;
                minDistClusterInd=array1[r1];
            }
        }
    }

    meanObjectLeft.x=0.0;
    meanObjectLeft.y=0.0;

    meanObjectRight.x=0.0;
    meanObjectRight.y=0.0;

    CountForMean=0;
    numberfeatures = 0;

    if(fileSave)
    {
        char filename[250];
        sprintf(filename,"%s/object%02d_fileFeaturesIn%02d.txt",pathResults,objectCount,numberPoses);
        fp1 = fopen(filename,"w+");
        sprintf(filename,"%s/object%02d_fileFeaturesOut%02d.txt",pathResults,objectCount,numberPoses);
        fp2 = fopen(filename,"w+");

    }

    for(unsigned int r1=0; r1<heightRegionLeft;r1++)
    {

        if(array1[r1]==minDistClusterInd)
        {
            if(zerodisp)
            {
                if( (mp_cornersleft[r1].x-mp_cornersright[r1].x) < - HORDISPTHRES)
                {
#if DEBUGSegmentation
                    cvCircle(featureImage,cvPoint((int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y),2,CV_RGB(0,0,255), CV_FILLED );
#endif
                    pointsbehind++;
                    continue;
                }
                else if( (mp_cornersleft[r1].x-mp_cornersright[r1].x) > HORDISPTHRES)
                {
                    pointsinfront++;
#if DEBUGSegmentation
                    cvCircle(featureImage,cvPoint((int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y),2,CV_RGB(12,100,20), CV_FILLED );
#endif
                    continue;
                }
            }
            pointson++;

            array5[r1]=1;
            numberfeatures++;

            cvCircle(featureImage,cvPoint((int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y),2,CV_RGB(255,255,255), CV_FILLED );

            if(fileSave)
            {
                fprintf(fp1,"%d\t%d\n",(int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y);
            }

            meanObjectLeft.x=(mp_cornersleft[r1].x+CountForMean*meanObjectLeft.x)/(float)(CountForMean+1);
            meanObjectLeft.y=(mp_cornersleft[r1].y+CountForMean*meanObjectLeft.y)/(float)(CountForMean+1);
            meanObjectRight.x=(mp_cornersright[r1].x+CountForMean*meanObjectRight.x)/(float)(CountForMean+1);
            meanObjectRight.y=(mp_cornersright[r1].y+CountForMean*meanObjectRight.y)/(float)(CountForMean+1);
            CountForMean++;
        }
        else
        {
            if(array4[array1[r1]]>5)
            {
                  cvCircle(featureImage,cvPoint((int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y),2,colors[cvRound(array1[r1]%13)], CV_FILLED );
            }

            //cvCircle(featureImage,cvPoint((int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y),2,CV_RGB(0,0,0), CV_FILLED );
            if(fileSave)
            {
                fprintf(fp2,"%d\t%d\n",(int)mp_cornersleft[r1].x,(int)mp_cornersleft[r1].y);
            }
        }

    }

#if DEBUGSegmentation
    std::cout<<endl<<"pointsbehind = "<<pointsbehind<<endl;
    std::cout<<"pointsinfront = "<<pointsinfront<<endl;
    std::cout<<"pointson = "<<pointson<<endl;
    std::cout<<"numberfeatures = "<<numberfeatures<<endl;
    cvShowImage("TestWhenFocussed",featureImage);
#endif

    if(fileSave)
    {
        fclose(fp1);
        fclose(fp2);
    }

    free(array1);
    free(array2);
    free(array3);
    free(array4);
    return(array5);
}

void Segmentation::displayObject()
{
    imageObjectResize[objectCount-1] = cvCreateImage(cvSize(80,80),IPL_DEPTH_8U,3);
    cvSetImageROI(iplimageleftin,cvRect(120,80,80,80));
    cvResize(iplimageleftin,imageObjectResize[objectCount-1]);
    cvResetImageROI(iplimageleftin);

    cvSetImageROI(imageObjects,cvRect(((objectCount-1)%4)*imageObjectResize[objectCount-1]->width,imageObjectResize[objectCount-1]->height*((objectCount-1)/4),imageObjectResize[objectCount-1]->width,imageObjectResize[objectCount-1]->height));
    cvCopy(imageObjectResize[objectCount-1],imageObjects);
    cvResetImageROI(imageObjects);
    cvShowImage(nameObjectsDisplay.c_str(), imageObjects);
}
void Segmentation::saveFeatures()
{
    for(int i=0;i<objectCount;i++)
    {
        cvSetZero(imageObjects);
        static char text2[256];CvFont font;
        sprintf(text2, "Input object Index");
        cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7, 0, 1.5);
        cvPutText(imageObjects, text2, cvPoint(20, 10), &font, cvScalar(0, 255, 0));
        cvSetImageROI(imageObjects,cvRect(120,80,80,80));
        cvCopy(imageObjectResize[i],imageObjects);
        cvResetImageROI(imageObjects);
        cvShowImage(nameVisitedLocationBlobs.c_str(), imageObjects);

        int objectIndex = (int)cvWaitKey(0);
        int asciiCorrect=objectIndex-48;

        sprintf(text2, "Your input: %d",asciiCorrect);
        cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7, 0, 1.5);
        cvPutText(imageObjects, text2, cvPoint(20, 200), &font, cvScalar(0, 255, 0));
        cvShowImage(nameVisitedLocationBlobs.c_str(), imageObjects);
        cvWaitKey(2000);


        char filename[300],command[300],newname[300];
        sprintf(filename,"%s/object%02d_image%02d.png",pathResults,i+1, 1);
        sprintf(newname,"%s/Renamed/object%02d_image%02d.png",pathResults,asciiCorrect,poseIndex[asciiCorrect]);
        std::cout<<filename<<endl;
        std::cout<<newname<<endl;
        sprintf(command, "cp %s %s",filename,newname);
        system(command);

        sprintf(filename,"%s/object%02d_features%02d.png",pathResults,i+1, 1);
        sprintf(newname,"%s/Renamed/object%02d_features%02d.png",pathResults,asciiCorrect,poseIndex[asciiCorrect]);
        std::cout<<filename<<endl;
        std::cout<<newname<<endl;
        sprintf(command, "cp %s %s",filename,newname);
        system(command);

        sprintf(filename,"%s/object%02d_fileFeaturesIn%02d.txt",pathResults,i+1, 1);
        sprintf(newname,"%s/Renamed/object%02d_fileFeaturesIn%02d.txt",pathResults,asciiCorrect,poseIndex[asciiCorrect]);
        std::cout<<filename<<endl;
        std::cout<<newname<<endl;
        sprintf(command, "cp %s %s",filename,newname);
        system(command);

        sprintf(filename,"%s/object%02d_fileFeaturesOut%02d.txt",pathResults,i+1, 1);
        sprintf(newname,"%s/Renamed/object%02d_fileFeaturesOut%02d.txt",pathResults,asciiCorrect,poseIndex[asciiCorrect]);
        std::cout<<filename<<endl;
        std::cout<<newname<<endl;
        sprintf(command, "cp %s %s",filename,newname);
        system(command);

        poseIndex[asciiCorrect]++;

    }
}
