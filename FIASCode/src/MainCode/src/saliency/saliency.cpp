#include "iCub/saliency/saliency.h"

using namespace std;


Saliency::Saliency(std::string name){

    Network yarp; 						//	set up yarp

    nameclass =name;					//	"Saliency"

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
    nameImageRightInPort = "/image/" + nameclass + "/right/in";		//[image/Saliency/right/in]
    nameSenderImageRightPort = "/image/Camera_right/right/out";	//[image/Camera_right/right/in]
    ReceivingImageRightPort.open(nameImageRightInPort.c_str());
    Network::connect(nameSenderImageRightPort.c_str(),nameImageRightInPort.c_str());
    receiveimageright = false;
    yarpimagerightin = NULL;
    iplimagerightin = NULL;

    /*	READ HARRIS	LEFT*/
    nameHarrisLeftInPort = "/harris/" + nameclass + "/left/in";							//[harris/Saliency/left/in]
    nameSenderHarrisLeftPort = "/harris/Match/left/out";							//[harris/Match/left/out]
    ReceivingHarrisLeftPort.open(nameHarrisLeftInPort.c_str());
    Network::connect(nameSenderHarrisLeftPort.c_str(),nameHarrisLeftInPort.c_str());
    receiveharrisleft = false;
    HarrisLeftIn=NULL;
    keypointsL=NULL;


    /*	READ HARRIS	RIGHT*/
    nameHarrisRightInPort = "/harris/" + nameclass + "/right/in";						//[harris/Saliency/right/in]
    nameSenderHarrisRightPort = "/harris/Match/right/out";						//[harris/Match/right/out]
    ReceivingHarrisRightPort.open(nameHarrisRightInPort.c_str());
    Network::connect(nameSenderHarrisRightPort.c_str(),nameHarrisRightInPort.c_str());
    receiveharrisright = false;
    HarrisRightIn=NULL;
    keypointsR=NULL;

    //for parallelization
    nameInhibitHarrisInPort = "/InhibitHarris/left/in";//	[InhibitHarris/left/in]
    InhibitHarrisInPort.open(nameInhibitHarrisInPort.c_str());
    receiveinhibitharris = true;
    inhibitHarrisArray = NULL;

    /*	SALIENCY	*/
    ipl_inputL = 0;
    ipl_inputR = 0;
    ipl_outputL = 0;
    ipl_outputR = 0;
    results = NULL;
    move = false;
    CenterL = cvPoint(160,120);
    CenterR = cvPoint(160,120);

    /*	DISPLAY SALIENCY	*/
#if DISPLAYSALIENCYIMAGE
    nameImageLeftOutPort = "/image/" + nameclass + "/left/out";//	[image/Harris_left/left/out]
    SendingImageLeftPort.open(nameImageLeftOutPort.c_str());
    dispImageLeft = NULL;
    if(!dispImageLeft) dispImageLeft = new DisplayIplImage("DisplaySaliency","left",nameclass.c_str(),640,0);

    nameImageRightOutPort = "/image/" + nameclass + "/right/out";//	[image/Harris_left/left/out]
    SendingImageRightPort.open(nameImageRightOutPort.c_str());
    dispImageRight = NULL;
    if(!dispImageRight) dispImageRight = new DisplayIplImage("DisplaySaliency","right",nameclass.c_str(),960,0);
#endif

    /*	SEND WINNER POINTS	*/
    /*
 namePointLeftOutPort = "/point/" + nameclass + "/left/out";//	[point/Saliency/left/out]
 SendingPointLeftPort.open(namePointLeftOutPort.c_str());
 namePointRightOutPort = "/point/" + nameclass + "/right/out";//	[point/Saliency/right/out]
 SendingPointRightPort.open(namePointRightOutPort.c_str());
 */

    std::cout<<"finished.\n";

}

Saliency::~Saliency() {

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


    /*	READ HARRIS RIGHT	*/
    ReceivingHarrisRightPort.interrupt();
    ReceivingHarrisRightPort.close();

    //for parallelization
    InhibitHarrisInPort.interrupt();
    InhibitHarrisInPort.close();

    if(keypointsL)
        free(keypointsL);
    if(keypointsR)
        free(keypointsR);

    /*	SALIENCY	*/
    cvReleaseImage(&ipl_inputL);
    cvReleaseImage(&ipl_inputR);
    cvReleaseImage(&ipl_outputL);
    cvReleaseImage(&ipl_outputR);

    /*	DISPLAY SALIENCY	*/
#if DISPLAYSALIENCYIMAGE
    if(dispImageLeft) delete dispImageLeft;
    SendingImageLeftPort.interrupt();
    SendingImageLeftPort.close();

    if(dispImageRight) delete dispImageRight;
    SendingImageRightPort.interrupt();
    SendingImageRightPort.close();
#endif

    /*	SEND WINNER POINTS	*/
    /*
 SendingPointLeftPort.interrupt();
 SendingPointLeftPort.close();
 SendingPointRightPort.interrupt();
 SendingPointRightPort.close();
 */

    std::cout<<"finished.\n";

}

void Saliency::execute()
{
    std::cout<<endl<<"receiving inhibit locations from Segmentation Topdown information"<<endl;
    inhibitHarris.clear();
    inhibitHarrisArray = InhibitHarrisInPort.read(true);
    int countInhibitHarris = inhibitHarrisArray->get(0).asInt();
    for(int i=0;i<countInhibitHarris;i++)
    {
        bool tempInhibitValue = (bool)inhibitHarrisArray->get(i+1).asInt();
        inhibitHarris.push_back(tempInhibitValue);
    }
    std::cout<<endl<<"received locations"<<endl;

    move=false;

#if DEBUGSALIENCY
    std::cout<<"\n"<< nameclass << "::execute\n";
#endif

    /*	READ IMAGE	LEFT AND RIGHT	*/
    yarpimageleftin = ReceivingImageLeftPort.read(&receiveimageleft);
    yarpimagerightin = ReceivingImageRightPort.read(&receiveimageright);
    iplimageleftin = ( IplImage*)yarpimageleftin->getIplImage();
    iplimagerightin = ( IplImage*)yarpimagerightin->getIplImage();

    if(!iplimageleftin || !iplimagerightin)
    {
        std::cerr <<endl<< nameclass << "::execute images NULL!"<<endl;
        return;
    }

    /*	READ HARRIS	LEFT AND RIGHT	(malloc)	*/
    HarrisLeftIn = ReceivingHarrisLeftPort.read(&receiveharrisleft);
    HarrisRightIn = ReceivingHarrisRightPort.read(&receiveharrisright);
    int sizearrayL = HarrisLeftIn->get(0).asInt();
    int sizearrayR = HarrisRightIn->get(0).asInt();
    if(sizearrayL <= 0 || sizearrayR <= 0) {
        std::cerr <<endl<< nameclass << "::execute::ERROR::sizearrayL <= 0 or sizearrayR <= 0!";
        return;
    }

    int counttrue = 0;
    for (int i = 0; i<inhibitHarris.size();i++)
    {
        if(inhibitHarris[i])
            counttrue++;
    }


    if(keypointsL)
    {
        free(keypointsL);
        keypointsL = (int*)malloc(2*sizeof(int)*counttrue);
    }
    else
        keypointsL = (int*)malloc(2*sizeof(int)*counttrue);

    if(keypointsR)
    {
        free(keypointsR);
        keypointsR = (int*)malloc(2*sizeof(int)*counttrue);
    }
    else
        keypointsR = (int*)malloc(2*sizeof(int)*counttrue);

    int keycountleft=0;
    for(unsigned int i = 0; i<2*sizearrayL; i=i+2)
    {
        if(inhibitHarris[i/2])
        {
            float posx = HarrisLeftIn->get(i+1).asDouble();
            float posy = HarrisLeftIn->get(i+2).asDouble();
            keypointsL[keycountleft] = (int)posx;
            keypointsL[keycountleft+1] = (int)posy;
            keycountleft = keycountleft + 2;
        }
    }

    int keycountright=0;
    for(unsigned int i = 0; i<2*sizearrayR; i=i+2)
    {
        if(inhibitHarris[i/2])
        {
            float posx = HarrisRightIn->get(i+1).asDouble();
            float posy = HarrisRightIn->get(i+2).asDouble();
            keypointsR[keycountright] = (int)posx;
            keypointsR[keycountright+1] = (int)posy;
            keycountright =keycountright + 2;
        }
    }


#if DEBUGSALIENCY
    std::cout<<endl<<"sizearrayL = "<<sizearrayL<<endl;
    std::cout<<"sizearrayR = "<<sizearrayL<<endl;
    std::cout<<"inhibitHarris.size() = "<<inhibitHarris.size()<<endl;
    std::cout<<"counttrue = "<<counttrue<<endl;
    std::cout<<"keycountleft = "<<keycountleft<<endl;
    std::cout<<"keycountright = "<<keycountright<<endl;

    std::cout<<"\n"<< nameclass << "::execute::sizearray left = "<< counttrue;
    std::cout<<"\n"<< nameclass << "::execute::sizearray right = "<< counttrue <<"\n";

    std::cout<<"\n"<< nameclass << "::execute::keypointsL[0]: [" << keypointsL[0]<< "," << keypointsL[1]<< "]";
    std::cout<<"\n"<< nameclass << "::execute::keypointsR[0]: [" << keypointsR[0] << "," << keypointsR[1] << "]\n";

    std::cout<<"\n"<< nameclass << "::execute::keypointsL[1]: [" << keypointsL[2]<< "," << keypointsL[3]<< "]";
    std::cout<<"\n"<< nameclass << "::execute::keypointsR[1]: [" << keypointsR[2] << "," << keypointsR[3] << "]\n";

    std::cout<<"\n"<< nameclass << "::execute::keypointsL["<< counttrue -1 <<"]: [" << keypointsL[(counttrue-1)*2]<< "," << keypointsL[(counttrue-1)*2+1]<< "]";
    std::cout<<"\n"<< nameclass << "::execute::keypointsR["<< counttrue -1 <<"]: [" << keypointsR[(counttrue-1)*2] << "," << keypointsR[(counttrue-1)*2+1] << "]\n";

    //cvWaitKey(0);
#endif

    /*	SALIENCY	*/

    ipl_inputL = cvCloneImage(iplimageleftin);
    ipl_inputR = cvCloneImage(iplimagerightin);

    int img_w = ipl_inputL->width;
    int img_h = ipl_inputR->height;

#if DEBUGSALIENCY
    std::cout<<"\n"<< nameclass << "::execute::Size image left = "<< img_w << " " << img_h << endl;
    //cvWaitKey(0);
#endif

    ipl_outputL = cvCloneImage(iplimageleftin);
    ipl_outputR = cvCloneImage(iplimagerightin);

    envision_nextframe(ipl_inputL, ipl_outputL, 0);
    envision_nextframe(ipl_inputR, ipl_outputR, 1);

    //ASK RICHARD FOR 2*sizearrayL AND
    index = SC_subset_winner(keypointsL, (2*counttrue),keypointsR, (2*counttrue));

    results=SC_naive_competition(ipl_outputL, ipl_outputR);

#if DEBUGSALIENCY
    std::cout<<"\n"<< nameclass << "::execute:: 0=LEFT  1=RIGHT ? =>" << results[0];
    std::cout<<"\n"<< nameclass << "::execute::RESULT X, Y: (" << results[1] << ", " << results[2] << ")" << endl;
#endif

    output_filenameL = "resultL.ppm";
    output_filenameR = "resultR.ppm";

    int posxL = keypointsL[2*index];
    int posyL = keypointsL[2*index+1];
    int posxR = keypointsR[2*index];
    int posyR = keypointsR[2*index+1];

    pointOL = cvPoint(posxL,posyL);
    pointOR = cvPoint(posxR,posyR);

    double distlx = posxL - CenterL.x;
    double distly = posyL - CenterL.y;
    double distrx = posxR - CenterR.x;
    double distry = posyR - CenterR.y;

    /*		SEND POINT TO CUE INTEGRATION	*/
    /*
 winnerPosL = cvPoint(0,0);
 winnerSizeL = cvSize(0,0);
 winnerRectL.x = 0;
 winnerRectL.y = 0;
 winnerRectL.height = 0;
 winnerRectL.width = 0;
 reliabilityL = 0;
 imageSizeL = cvSize(0,0);

 winnerPosR = cvPoint(0,0);
 winnerSizeR = cvSize(0,0);
 winnerRectR.x = 0;
 winnerRectR.y = 0;
 winnerRectR.height = 0;
 winnerRectR.width = 0;
 reliabilityR = 0;
 imageSizeR = cvSize(0,0);
 */
    //TO BE SURE THAT THE WINNER POINT IS NOT TOO CLOSE TO THE BORDER OF THE IMAGE (PROBLEM WHEN WE COMPUTE TEMPLATE FOR TRACKING)
    if((results[0]==0 || results[0]==1))// && fabs(distlx)<110 && fabs(distly)<70 && fabs(distrx)<110 && fabs(distry)<70)
    {

        cvCircle(ipl_outputL, pointOL, 20, cvScalar(0,0,255), 2);
        cvCircle(ipl_outputR, pointOR, 20, cvScalar(0,0,255), 2);

        cvEllipse(ipl_outputL,cvPoint(160,120),cvSize((int)FOVEA_BOUNDARY,(int)(FOVEA_BOUNDARY/2)),0,0,360,CV_RGB(255,255,255));
        cvEllipse(ipl_outputR,cvPoint(160,120),cvSize((int)FOVEA_BOUNDARY,(int)(FOVEA_BOUNDARY/2)),0,0,360,CV_RGB(255,255,255));


        static char text[256];
        sprintf(text, "WINNER");
        cvInitFont(&font, CV_FONT_HERSHEY_PLAIN | CV_FONT_ITALIC, 1.2, 1.2, 0, 1.7);

        if(results[0] == 0)
            cvPutText(ipl_outputL, text, cvPoint(10, 30), &font, cvScalar(0, 0, 255));
        if(results[0] == 1)
            cvPutText(ipl_outputR, text, cvPoint(10, 30), &font, cvScalar(0, 0, 255));

        //TODO REMOVE
        //JUST TO DISPLAY IN A COLOR IMAGE THE WINNER POINT
        cvNamedWindow("TEST");
        cvMoveWindow("TEST",1280,480);
        cvCvtColor(iplimageleftin,iplimageleftin, CV_BGR2RGB);
        cvCircle(iplimageleftin, pointOL, 20, cvScalar(255,0,0), 2);
        cvShowImage("TEST",iplimageleftin);



        /*		SEND POINT TO CUE INTEGRATION	*/
        /*
  winnerPosL = pointOL;
  winnerSizeL = cvSize(50,50);
  winnerRectL.x = pointOL.x-winnerSizeL.width;
  winnerRectL.y = pointOL.y-winnerSizeL.height;
  winnerRectL.height = winnerSizeL.height;
  winnerRectL.width = winnerSizeL.width;
  reliabilityL = 1;

  winnerPosR = pointOR;
  winnerSizeR = cvSize(50,50);
  winnerRectR.x = pointOR.x-winnerSizeR.width;
  winnerRectR.y = pointOR.y-winnerSizeR.height;
  winnerRectR.height = winnerSizeR.height;
  winnerRectR.width = winnerSizeR.width;
  reliabilityR = 1;
  */
        move = true;
    }

    /*		SEND POINT TO CUE INTEGRATION	*/
    /*
  Bottle& PointLeftOut = SendingPointLeftPort.prepare();
  Bottle& PointRightOut = SendingPointRightPort.prepare();
  PointLeftOut.clear();
  PointRightOut.clear();

  PointLeftOut.addInt(winnerPosL.x);
  PointLeftOut.addInt(winnerPosL.y);
  PointLeftOut.addInt(winnerSizeL.height);
  PointLeftOut.addInt(winnerSizeL.width);
  PointLeftOut.addInt(winnerRectL.x);
  PointLeftOut.addInt(winnerRectL.y);
  PointLeftOut.addInt(winnerRectL.height);
  PointLeftOut.addInt(winnerRectL.width);
  PointLeftOut.addInt(reliabilityL);
  PointLeftOut.addInt(ipl_outputL->width);
  PointLeftOut.addInt(ipl_outputL->height);

  PointRightOut.addInt(winnerPosR.x);
  PointRightOut.addInt(winnerPosR.y);
  PointRightOut.addInt(winnerSizeR.height);
  PointRightOut.addInt(winnerSizeR.width);
  PointRightOut.addInt(winnerRectR.x);
  PointRightOut.addInt(winnerRectR.y);
  PointRightOut.addInt(winnerRectR.height);
  PointRightOut.addInt(winnerRectR.width);
  PointRightOut.addInt(reliabilityR);
  PointRightOut.addInt(ipl_outputR->width);
  PointRightOut.addInt(ipl_outputR->height);

  SendingPointLeftPort.write();
  SendingPointRightPort.write();

  #if DEBUGSALIENCY
   std::cerr <<"\n"<< nameclass << "::readfromSal()"<<endl;
   std::cout<<"winnerPosL.x = " <<winnerPosL.x<<endl;
   std::cout<<"winnerPosL.y = " <<winnerPosL.y<<endl;
   std::cout<<"winnerSizeL.height = "<<winnerSizeL.height<<endl;
   std::cout<<"winnerSizeL.width = "<<winnerSizeL.width<<endl;
   std::cout<<"winnerRectL.x = " <<winnerRectL.x<<endl;
   std::cout<<"winnerRectL.y = " <<winnerRectL.y<<endl;
   std::cout<<"winnerRectL.height = "<<winnerRectL.height<<endl;
   std::cout<<"winnerRectL.width = "<<winnerRectL.width<<endl;
   std::cout<<"reliabilityL = "<<reliabilityL<<endl;
   std::cout<<"imageSizeL.width = "<<ipl_outputL->width<<endl;
   std::cout<<"imageSizeL.height = "<<ipl_outputL->height<<endl;

   std::cout<<endl<<"winnerPosR.x = " <<winnerPosR.x<<endl;
   std::cout<<"winnerPosR.y = " <<winnerPosR.y<<endl;
   std::cout<<"winnerSizeR.height = "<<winnerSizeR.height<<endl;
   std::cout<<"winnerSizeR.width = "<<winnerSizeR.width<<endl;
   std::cout<<"winnerRectR.x = " <<winnerRectR.x<<endl;
   std::cout<<"winnerRectR.y = " <<winnerRectR.y<<endl;
   std::cout<<"winnerRectR.height = "<<winnerRectR.height<<endl;
   std::cout<<"winnerRectR.width = "<<winnerRectR.width<<endl;
   std::cout<<"reliabilityR = "<<reliabilityR<<endl;
   std::cout<<"imageSizeR.width = "<<ipl_outputR->width<<endl;
   std::cout<<"imageSizeR.height = "<<ipl_outputR->height<<endl;
  #endif
  */

    /*		DISPLAY SALIENCY	*/
#if DISPLAYSALIENCYIMAGE
    yarpimageleftout.wrapIplImage(ipl_outputL);
    SendingImageLeftPort.prepare()=yarpimageleftout;
    SendingImageLeftPort.write();
    dispImageLeft->execute();

    yarpimagerightout.wrapIplImage(ipl_outputR);
    SendingImageRightPort.prepare()=yarpimagerightout;
    SendingImageRightPort.write();
    dispImageRight->execute();
#endif

    cvReleaseImage(&ipl_outputL);
    cvReleaseImage(&ipl_outputR);
    cvReleaseImage(&ipl_inputL);
    cvReleaseImage(&ipl_inputR);

}

void Saliency::inib(int featnumber){

    excite=0;

    //TODO READ MATCHING HARRIS ON THE OBJECT (COMPLEXITY OBJECT = NUM FEATURES)
    /*
 int sizearraydispL = (*sizearraydispInL.getBuffer());
 int sizearraydispR = (*sizearraydispInR.getBuffer());

 excite = (sizearraydispL + sizearraydispR)/2;
 */

    //TODO REPLACE WHEN WE HAVE DISPARITY MATCHING
    int inibition = (int)featnumber/2.0;
    excite = inibition;

    SC_reset();
    RF_inject(excite);

}

