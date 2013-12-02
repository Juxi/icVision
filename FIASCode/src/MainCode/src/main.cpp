#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

char pathResults[250];

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

#include "iCub/main.h"

void initStuff() {
    std::cout << "init stuff " << std::endl;    
    camicubLeft = new CameraiCub("Camera","left","main");
    camicubRight = new CameraiCub("Camera","right","main");
    #if DEBUGMAIN
    dispImageLeft = new DisplayIplImage("DisplayImage","left","Camera_left",0,0);
    dispImageRight= new DisplayIplImage("DisplayImage","right","Camera_right",320,0);
    #endif

    featureLeft  = new feature2D("Harris","left");
    featureRight = new feature2D("Harris","right");

    GaborLeft = new GaborDescriptor("Gabor","left","Harris_left");
    GaborRight = new GaborDescriptor("Gabor","right","Harris_right");

    Match = new DetectObject("Match");

    Sal = new Saliency("Saliency");

    SegmentObj = new Segmentation("Segmentation");
    #if DEBUGMAIN
        DisplayMatch = new DisplayMatching("DisplayMatch","Match",0,240);
    #endif
}

int main(int argc, char* argv[])
{
    // exit(1);
// int main(int argc, char* argv[])
// {    
    ofstream myfile;
    
    if(argc >= 2) {
        //sprintf(pathResults,"/home/pramod/robot/CuriousVisionSystem20Feb2012/FIASforIDSIA/Images");
        sprintf(pathResults, argv[1]);
    } else
        sprintf(pathResults, "Images");

    char command[250];
    
    printf("lala: %s\n", pathResults);
    
    sprintf(command, "rm -r %s",pathResults);
    system(command);
    sprintf(command, "mkdir %s",pathResults);
    system(command);
    sprintf(command, "mkdir %s/Renamed",pathResults);
    system(command);

    //set up yarp network
    Network yarp;
    initStuff();

    BalisticDoneInPort.open("/main/BalisticDone/in");
    Network::connect("/motion/BalisticDone/out","/main/BalisticDone/in");

    commandLocationPort.open("/main/commandMotion/out");
    Network::connect("/main/commandMotion/out","/motion/commandMotion/in");

    MotionPortLeftPort.open("/main/motion/left/out");
    MotionPortRightPort.open("/main/motion/right/out");
    Network::connect("/main/motion/left/out","/motion/left/in");
    Network::connect("/main/motion/right/out","/motion/right/in");

    Network::connect("/InhibitHarris/left/out","/InhibitHarris/left/in");

    featureLeft->setParam();
    featureRight->setParam();

    GaborLeft->setParam();
    GaborRight->setParam();

    bool quitprogram = false;

    starttimerprogram = (double)clock()/CLOCKS_PER_SEC;
    starttimerobject = (double)clock()/CLOCKS_PER_SEC;

    while(!quitprogram)
    {
        if(checkTime()>DURATION_RUN)
            quitprogram=true;
        runFeatures();
        switch (decide)
        {
        case SALIENCYMAP:
            //std::cout<<endl<<"Check if icub is outside boundary and recoil the head otherwise"<<endl;
            sendcommand("checkboundary");

            //std::cout<<endl<<"ask motion module to send the visited locations"<<endl;
            sendcommand("getlocation");

            //std::cout<<endl<<"obtain locations to inhibit from motion code"<<endl;
            SegmentObj->setSelectHarris();

            //std::cout<<endl<<"inhibiting already visited locations"<<endl;
            SegmentObj->inhibitSamplesLocationHistory();

            //std::cout<<endl<<"sending inhibition to saliency"<<endl;
            SegmentObj->sendInhibitionToSaliency();

            Sal->execute();

            if (Sal->move == true)
            {
                std::cout<<"\nFound Salient Point"<<endl<<endl;

                sendcommand("moverobot");

                double pointSalLeftX = Sal->pointOL.x;
                double pointSalLeftY = Sal->pointOL.y;
                double pointSalRightX = Sal->pointOR.x;
                double pointSalRightY = Sal->pointOR.y;

                sendtomotion(pointSalLeftX,pointSalLeftY,pointSalRightX,pointSalRightY, "Balistic");

                if(!waitMotionDone())
                    break;

                decide = SEGMENTOBJECT;

                break;

            }
            break;

        case SEGMENTOBJECT:

            std::cout<<"\n Segmenting the Object near the Saliency point"<<endl;

            SegmentObj->fileSave = false;
            SegmentObj->segmentObject(cvPoint(160,120),false);

            std::cout<<"segmentation done"<<endl;
#if TRAINING
            if (SegmentObj->numberfeatures>FEATURE_COUNT_TRESHOLD)
            {

                std::cout<<endl<<"Now move to the center of the object"<<endl;

                sendcommand("moverobot");

                double pointSalLeftX = SegmentObj->meanObjectLeft.x;
                double pointSalLeftY = SegmentObj->meanObjectLeft.y;
                double pointSalRightX = SegmentObj->meanObjectRight.x;
                double pointSalRightY = SegmentObj->meanObjectRight.x;

                sendtomotion(pointSalLeftX,pointSalLeftY,pointSalRightX,pointSalRightY, "Balistic");

                if(!waitMotionDone())
                    break;

                SegmentObj->segmentObject(cvPoint(160,120),false);
                if(SegmentObj->numberfeatures<FEATURE_COUNT_TRESHOLD)
                {
                    std::cout<<endl<<"Very few features. Cannot segment the object. Searching for a new object"<<endl;
                    Sal->inib(SALIENCY_INHIBIT_CENTER);
                    decide = SALIENCYMAP;
                    break;
                }

                int poses_Total;

#if ASK_USER

                std::cout<<endl<<"Press 's' to skip the object and press any other key to continue"<<endl;
                poses_Total = POSES_PER_OBJECT_DYNAMIC;
                char ch=getchar();
                if(ch=='s')
                {
                    std::cout<<"Object skipped"<<endl;
                    Sal->inib(SALIENCY_INHIBIT_CENTER);
                    decide = SALIENCYMAP;
                    break;
                }
                std::cout<<"Image capturing starts in 5 sec"<<endl;
                std::cout<<"keep changing the object pose slowly for 15 seconds. 1 frame is drawn at every second"<<endl;
                std::cout<<"Try to maintain the object position roughly the same"<<endl;

#else
                std::cout<<"Image capturing starts in 1 sec"<<endl;
                poses_Total = POSES_PER_OBJECT_STATIC;
#endif

                cvWaitKey(1000);

                SegmentObj->objectCount++;
                SegmentObj->numberPoses=0;

                for(int i=0;i<poses_Total;i++)
                {
                    runFeatures();

                    SegmentObj->fileSave = true;
                    SegmentObj->numberPoses++;

                    SegmentObj->segmentObject(cvPoint(160,120),false);
                    std::cout<<endl<<"saving the image...";
                    cvWaitKey(1000);
                    SegmentObj->displayObject();
                    std::cout<<"saved!"<<endl;                    

                }

            }
            else
            {
                std::cout<<endl<<"Very few features. Cannot segment the object. Searching for a new object"<<endl;
            }
#else
            std::cout<<endl<<"Now move to the center of the object"<<endl;

            sendcommand("moverobot");

            double pointSalLeftX = SegmentObj->meanObjectLeft.x;
            double pointSalLeftY = SegmentObj->meanObjectLeft.y;
            double pointSalRightX = SegmentObj->meanObjectRight.x;
            double pointSalRightY = SegmentObj->meanObjectRight.x;

            sendtomotion(pointSalLeftX,pointSalLeftY,pointSalRightX,pointSalRightY, "Balistic");

            if(!waitMotionDone())
                break;

            runFeatures();

            std::cout<<endl<<"Testing the object!"<<endl;
            cvWaitKey(3000);

#endif

            Sal->inib(SALIENCY_INHIBIT_CENTER);
            decide = SALIENCYMAP;
            break;


        }


    }
    
    std::cout<<endl<<"main: end of loop!"<<endl;    

    std::cout<<endl<<"main: saveFeatures() ... ";        
    std::cout<<" ... "<<endl;        
    SegmentObj->saveFeatures();
    std::cout<<" done!"<<endl;    
    
    std::cout<<endl<<"main: close balsitic port";            
    BalisticDoneInPort.interrupt();
    BalisticDoneInPort.close();
    std::cout<<" done!"<<endl;        

    std::cout<<endl<<"main: close command location port";            
    commandLocationPort.interrupt();
    commandLocationPort.close();
    std::cout<<" done!"<<endl;    
    
    std::cout<<endl<<"main: close motion ports";                
    MotionPortLeftPort.interrupt();
    MotionPortLeftPort.close();    
    MotionPortRightPort.interrupt();
    MotionPortRightPort.close();
    std::cout<<" done!"<<endl;    
}


bool checkClock()
{
    tstop = (double)clock()/CLOCKS_PER_SEC;

    ttime= tstop-tstart; //ttime is how long saliency map run

    if(ttime>6)
    {
#if DEBUGMAIN
        std::cout<<endl<<"Not possible to focus. Try different location"<<endl;
#endif
        return(true);
    }
    return(false);
}
void sendcommand(string command)
{
    Bottle& commandLocation = commandLocationPort.prepare();
    commandLocation.clear();
    if(command == "getlocation")
        commandLocation.addString("getlocation");
    if(command == "moverobot")
        commandLocation.addString("moverobot");
    commandLocationPort.write(true);
}
int checkTime()
{
    stoptimerprogram = (double)clock()/CLOCKS_PER_SEC;
    timeprogram= round((stoptimerprogram-starttimerprogram)*SAMPLERATE);
    return(timeprogram);
}
void sendtomotion(double pointSalLeftX,double pointSalLeftY,double pointSalRightX,double pointSalRightY, char *type)
{

    Bottle& targetL = MotionPortLeftPort.prepare();
    Bottle& targetR = MotionPortRightPort.prepare();
    targetL.clear();
    targetL.addDouble(pointSalLeftX);
    targetL.addDouble(pointSalLeftY);
    targetL.addDouble(1);
    targetL.addString(type);
    targetR.clear();
    targetR.addDouble(pointSalRightX);
    targetR.addDouble(pointSalRightY);
    targetR.addDouble(1);
    targetR.addString(type);

    targetL = MotionPortLeftPort.prepare();
    MotionPortLeftPort.write();
    targetR = MotionPortRightPort.prepare();
    MotionPortRightPort.write();
}

bool waitMotionDone()
{

    std::cout<<endl<<"Reading reply from motion"<<endl;

    //tstop = (double)clock()/CLOCKS_PER_SEC;
    BalisticDone=NULL;

    while(1)
    {
        runFeatures();
        BalisticDone = BalisticDoneInPort.read(false);

        if(BalisticDone)
        {
            Done = BalisticDone->get(0).asString();
            if(Done == "BalisticDone")
            {
                std::cout<<"\nBalistic Movement Done"<<endl;

                break;
            }
        }
        /*if(checkClock())
        {
            decide = SALIENCYMAP;
            return(false);
            break;
        }*/
    }
    return(true);
}

void runFeatures()
{
    camicubLeft->execute();
    camicubRight->execute();

#if DEBUGMAIN
    dispImageLeft->execute();
    dispImageRight->execute();
#endif
    featureLeft->execute();
    featureRight->execute();
    GaborLeft->execute();
    GaborRight->execute();

    Match->execute();
#if DEBUGMAIN
    DisplayMatch->execute();
#endif
    SegmentObj->readfromport();
}
