
#include "iCub/main.h"

int main(int argc, char* argv[])
{
    ofstream myfile;

    sprintf(pathResults,"/home/pramod/robot/CuriousVisionSystem20Feb2012/FIASforIDSIA/Images");
    char command[250];
    sprintf(command, "rm -r %s",pathResults);
    system(command);
    sprintf(command, "mkdir %s",pathResults);
    system(command);
    sprintf(command, "mkdir %s/Renamed",pathResults);
    system(command);

    //set up yarp network
    Network yarp;

    BalisticDoneInPort.open("/main/BalisticDone/in");
    Network::connect("/motion/BalisticDone/out","/main/BalisticDone/in");

    commandLocationPort.open("/main/commandMotion/out");
    Network::connect("/main/commandMotion/out","/motion/commandMotion/in");

    MotionPortLeftPort.open("/main/motion/left/out");
    MotionPortRightPort.open("/main/motion/right/out");
    Network::connect("/main/motion/left/out","/motion/left/in");
    Network::connect("/main/motion/right/out","/motion/right/in");

    Network::connect("/InhibitHarris/left/out","/InhibitHarris/left/in");

    featureLeft.setParam();
    featureRight.setParam();

    GaborLeft.setParam();
    GaborRight.setParam();

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
            SegmentObj.setSelectHarris();

            //std::cout<<endl<<"inhibiting already visited locations"<<endl;
            SegmentObj.inhibitSamplesLocationHistory();

            //std::cout<<endl<<"sending inhibition to saliency"<<endl;
            SegmentObj.sendInhibitionToSaliency();

            Sal.execute();

            if (Sal.move == true)
            {
                std::cout<<"\nFound Salient Point"<<endl<<endl;

                sendcommand("moverobot");

                double pointSalLeftX = Sal.pointOL.x;
                double pointSalLeftY = Sal.pointOL.y;
                double pointSalRightX = Sal.pointOR.x;
                double pointSalRightY = Sal.pointOR.y;

                sendtomotion(pointSalLeftX,pointSalLeftY,pointSalRightX,pointSalRightY, "Balistic");

                if(!waitMotionDone())
                    break;

                decide = SEGMENTOBJECT;

                break;

            }
            break;

        case SEGMENTOBJECT:

            std::cout<<"\n Segmenting the Object near the Saliency point"<<endl;

            SegmentObj.fileSave = false;
            SegmentObj.segmentObject(cvPoint(160,120),false);

            //std::cout<<"segmentation done"<<endl;
#if TRAINING
            if (SegmentObj.numberfeatures>FEATURE_COUNT_TRESHOLD)
            {

                std::cout<<endl<<"Now move to the center of the object"<<endl;

                sendcommand("moverobot");

                double pointSalLeftX = SegmentObj.meanObjectLeft.x;
                double pointSalLeftY = SegmentObj.meanObjectLeft.y;
                double pointSalRightX = SegmentObj.meanObjectRight.x;
                double pointSalRightY = SegmentObj.meanObjectRight.x;

                sendtomotion(pointSalLeftX,pointSalLeftY,pointSalRightX,pointSalRightY, "Balistic");

                if(!waitMotionDone())
                    break;

                SegmentObj.segmentObject(cvPoint(160,120),false);
                if(SegmentObj.numberfeatures<FEATURE_COUNT_TRESHOLD)
                {
                    std::cout<<endl<<"Very few features. Cannot segment the object. Searching for a new object"<<endl;
                    Sal.inib(SALIENCY_INHIBIT_CENTER);
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
                    Sal.inib(SALIENCY_INHIBIT_CENTER);
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

                SegmentObj.objectCount++;
                SegmentObj.numberPoses=0;

                for(int i=0;i<poses_Total;i++)
                {
                    runFeatures();

                    SegmentObj.fileSave = true;
                    SegmentObj.numberPoses++;

                    SegmentObj.segmentObject(cvPoint(160,120),false);
                    std::cout<<endl<<"saving the image..."<<endl;
                    cvWaitKey(1000);
                    SegmentObj.displayObject();

                }

            }
            else
            {
                std::cout<<endl<<"Very few features. Cannot segment the object. Searching for a new object"<<endl;
            }
#else
            std::cout<<endl<<"Now move to the center of the object"<<endl;

            sendcommand("moverobot");

            double pointSalLeftX = SegmentObj.meanObjectLeft.x;
            double pointSalLeftY = SegmentObj.meanObjectLeft.y;
            double pointSalRightX = SegmentObj.meanObjectRight.x;
            double pointSalRightY = SegmentObj.meanObjectRight.x;

            sendtomotion(pointSalLeftX,pointSalLeftY,pointSalRightX,pointSalRightY, "Balistic");

            if(!waitMotionDone())
                break;

            runFeatures();

            std::cout<<endl<<"Testing the object!"<<endl;
            cvWaitKey(3000);

#endif

            Sal.inib(SALIENCY_INHIBIT_CENTER);
            decide = SALIENCYMAP;
            break;


        }


    }
    SegmentObj.saveFeatures();
    BalisticDoneInPort.interrupt();
    BalisticDoneInPort.close();
    commandLocationPort.interrupt();
    commandLocationPort.close();
    MotionPortLeftPort.interrupt();
    MotionPortLeftPort.close();
    MotionPortRightPort.interrupt();
    MotionPortRightPort.close();
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
    camicubLeft.execute();
    camicubRight.execute();

#if DEBUGMAIN
    dispImageLeft.execute();
    dispImageRight.execute();
#endif
    featureLeft.execute();
    featureRight.execute();
    GaborLeft.execute();
    GaborRight.execute();

    Match.execute();
#if DEBUGMAIN
    DisplayMatch.execute();
#endif
    SegmentObj.readfromport();

}
