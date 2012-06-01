
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <yarp/math/Math.h>
#include "main.h"

#include <stdio.h>
#include <string>
#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

bool LookAtLocation::threadInit()
{
    std::cout<<"Thread initialization"<<endl;

    gohome = true;
    end=false;
    track=false;

    //initialize here variables
    printf("ControlThread:starting\n");

    //Property optGaze("(device gazecontrollerclient)");
    Property optGaze;
    optGaze.put("device","gazecontrollerclient");
    optGaze.put("remote","/iKinGazeCtrl");
    optGaze.put("local","/AutonomousMotion/gaze_client");

    Property optHead;
    optHead.put("device", "remote_controlboard");
    optHead.put("remote", "/icub/head");
    optHead.put("local", "/AutonomousMotion/local/head");

    MotionPortLeft.open("/motion/left/in");
    MotionPortRight.open("/motion/right/in");

    nameBalisticDoneOutPort = "/motion/BalisticDone/out";
    BalisticDoneOutPort.open(nameBalisticDoneOutPort.c_str());

    commandInPort.open("/motion/commandMotion/in");

    PixelLocationsOutPort.open("/motion/pixel_locations/out");

    clientGaze = new PolyDriver;
    if (!clientGaze->open(optGaze))
    {
        std::cout<<endl<<endl<<"COULD NOT OPEN CLIENTGAZE. ***ERROR***"<<endl<<endl;
        delete clientGaze;
        return false;
    }

    // open the view
    if (clientGaze->isValid()) {
        clientGaze->view(igaze);
    }
    igaze->storeContext(&startup_context_id);

    head = new PolyDriver;
    if (!head->open(optHead))
    {
        std::cout<<endl<<endl<<"COULD NOT OPEN CLIENTGAZE. ***ERROR***"<<endl<<endl;
        delete head;
        return false;
    }

    // open the view
    head->view(iencs);
    head->view(ivel);
    head->view(ipos);

    // set trajectory time:
    igaze->setNeckTrajTime(2.0);//(1.5);
    igaze->setEyesTrajTime(1.5);//(1.0);
#if BIND_NECK_ROLL
    igaze->bindNeckRoll(-5.0,5.0);
#endif
    //igaze->blockNeckRoll();

    if ( (!iencs) || (!ivel) || (!ipos) )
    {
        printf("Cannot get interface to robot head\n");
        return false;
    }

    joints = 0;

    iencs->getAxes(&joints);

    encoders.resize(joints);
    tmp.resize(joints);
    commands_position.resize(joints);
    commands_velocity.resize(joints);
    setpoints.resize(joints);
    checkpos.resize(joints);

    velocityFactor = 0.5;

    pxl.resize(2);
    pxr.resize(2);
    cl.resize(2);
    cr.resize(2);

    cl[0]=160.0;
    cl[1]=120.0;
    cr[0]=160.0;
    cr[1]=120.0;

    vx=0;
    vy=0;
    vz=0;

    state=STATE_HOME;

    x3d.resize(3);
    x3dRead.resize(3);
    ang3d.resize(3);
    ang3dRead.resize(3);
    pixelLeft.resize(2);

    for (int i=0; i<joints; i++)
    {
        encoders[i] = 0;
    }

    counter = 0;
    setbool = false;

    printf("Thead Initialization done\n");

    return true;

}

void LookAtLocation::run()
{
    std::cout<<"thread run"<<endl;

    while (isStopping() != true)
    {
        if (isStopping()) break;
        if (state==STATE_TRACK)
        {
            gohome = false; //SHould be in the beginning of the if statement only
            end = false;
            track = true;
            std::cout<<"STATE_TRACK"<<endl;

            commandText = NULL;
            std::cout<<endl<<"Read the command"<<endl;
            commandText = commandInPort.read(true);
            if(commandText!=NULL)
            {
                commandContent = commandText->get(0).asString();
                if(commandContent == "checkboundary")
                {
                    std::cout<<"received command: checkboundary"<<endl;
                    //Boundary check
#if BOUNDARY_CHECK
                    igaze->getAngles(ang3dRead);
                    if(fabs(ang3dRead[0])>AZIMUTH_RANGE/2.0||fabs(ang3dRead[1])>ELEVATION_RANGE/2.0)
                    {
                        ang3d[0]=ang3dRead[0]-ang3dRead[0]*RECOIL_FACTOR;
                        ang3d[1]=ang3dRead[1]-ang3dRead[1]*RECOIL_FACTOR;
                        igaze->lookAtAbsAngles(ang3d);
                        igaze->waitMotionDone();
                        std::cout<<endl<<"Recoil"<<endl;
                    }
#endif
                }
                if(commandContent == "getlocation")
                {
                    std::cout<<"received command: getlocation"<<endl;
                    std::cout<<endl<<"learning code has asked to send the visited locations"<<endl;
                    Bottle& LocationPixels = PixelLocationsOutPort.prepare();
                    LocationPixels.clear();

                    LocationPixels.addInt(locations3DSave.size());
                    std::cout<<endl<<"Number of visited locations so far: "<<locations3DSave.size()<<endl;
                    if(locations3DSave.size()!=0)
                    {
                        for(int i=0;i<locations3DSave.size();i++)
                        {
                            x3d[0]=locations3DSave[i].x;
                            x3d[1]=locations3DSave[i].y;
                            x3d[2]=locations3DSave[i].z;

                            success2Dpixel = igaze->get2DPixel(0,x3d,pixelLeft);
                            std::cout<<"x "<<x3d[0]<<" y "<<x3d[1]<<" z "<<x3d[2]<<endl;

                            LocationPixels.addInt(success2Dpixel);
                            if(success2Dpixel)
                            {
                                LocationPixels.addDouble(pixelLeft[0]);
                                LocationPixels.addDouble(pixelLeft[1]);
                                std::cout<<"pxl "<<pixelLeft[0]<<" pyl "<<pixelLeft[1]<<endl;
                            }
                            else
                            {
                                LocationPixels.addDouble(0);
                                LocationPixels.addDouble(0);
                                std::cout<<" failed to obtain pixel within the visible range "<<endl;
                            }
                        }
                    }
                    std::cout<<endl<<"Waiting for the learning code to read the visited locations as pixels"<<endl;
                    PixelLocationsOutPort.write(true);
                }
                if(commandContent == "moverobot")
                {
                    std::cout<<"received command: moverobot"<<endl;
                    targetIndL=NULL;
                    targetIndR=NULL;
                    std::cout<<endl<<"Waiting for a target left"<<endl;
                    targetIndL = MotionPortLeft.read(true);
                    std::cout<<"Waiting for a target right"<<endl;
                    targetIndR = MotionPortRight.read(true);
                    if (targetIndL != NULL && targetIndL->size()==4 && targetIndR != NULL && targetIndR->size()==4)
                    {
                        std::cout<<"RECEIVED INPUT"<<endl;
                        xL = targetIndL->get(0).asDouble();
                        yL = targetIndL->get(1).asDouble();
                        confL = targetIndL->get(2).asDouble();
                        TypeL = targetIndL->get(3).asString();

                        xR = targetIndR->get(0).asDouble();
                        yR = targetIndR->get(1).asDouble();
                        confR = targetIndR->get(2).asDouble();
                        TypeR = targetIndR->get(3).asString();

#if DISPLAY_VALUES
                        std::cout << endl<<"xL = "<<xL<<endl;
                        std::cout << "yL = "<<yL<<endl;
                        std::cout << "xR = "<<xR<<endl;
                        std::cout << "yR = "<<yR<<endl;

                        iencs->getEncoders(encoders.data());
                        std::cout<<"nencoders values"<<endl;
                        std::cout<<encoders[0]<<endl<<encoders[1]<<endl<<encoders[2]<<endl<<encoders[3]<<endl<<encoders[4]<<endl<<encoders[5]<<endl;
#endif
                        if((TypeL=="Balistic") && (TypeR=="Balistic"))
                        {
                            pxl[0]=xL;
                            pxl[1]=yL;
                            pxr[0]=xR;
                            pxr[1]=yL;

                            //igaze->lookAtStereoPixels(pxl,pxr);
                            igaze->lookAtMonoPixel(0,pxl);
                            igaze->waitMotionDone();
                            std::cout<<endl<<"Got Balistic!"<<endl;

                            Bottle& respond = BalisticDoneOutPort.prepare();
                            respond.clear();
                            respond.addString("BalisticDone");
                            BalisticDoneOutPort.write(true);

                            saveLocation();

                            std::cout<<endl<<"Motion Done!"<<endl;

                        }                        
                    }
                }
            }
        }
        if(state==STATE_END)
        {
            printf("\nSTATE_END\n");
            gohome = false;
            end=true;
            track = false;
            state=STATE_END;
            break;
        }
        if(state==STATE_HOME)
        {
            //std::cout<<"STATE HOME"<<endl;
            //printf("\nSTATE_HOME\n");
            gohome = true;
            track = false;
            end = false;
            //state=STATE_IDLE;
        }
    }
    std::cout<<"EXIT FROM RUN"<<endl;
}

void LookAtLocation::setMode(int mode)
{
    std::cout<<"entered set mode "<<endl;
    state=mode;
}

void LookAtLocation::onStop()
{
    std::cout<<"visited onstop"<<endl;
}
void LookAtLocation::interrupt()
{

    MotionPortLeft.interrupt();
    MotionPortRight.interrupt();

    //BalisticDoneOutPort.interrupt();

    //imagePortLeft.interrupt();
    //imagePortRight.interrupt();
}
void LookAtLocation::threadRelease()
{

    printf("LookAtLocation:stopping the robot\n");

    MotionPortLeft.interrupt();
    MotionPortLeft.close();
    MotionPortRight.interrupt();
    MotionPortRight.close();

    BalisticDoneOutPort.interrupt();
    BalisticDoneOutPort.close();

    imagePortLeft.interrupt();
    imagePortLeft.close();
    imagePortRight.interrupt();
    imagePortRight.close();

    commandInPort.interrupt();
    commandInPort.close();

    PixelLocationsOutPort.interrupt();
    PixelLocationsOutPort.close();

    iencs->getEncoders(encoders.data());


    if     (encoders[0]<0.3 && encoders[0]>-0.3 &&
            encoders[1]<0.3 && encoders[1]>-0.3 &&
            encoders[2]<0.3 && encoders[2]>-0.3 &&
            encoders[3]<0.3 && encoders[3]>-0.3 &&
            encoders[4]<0.3 && encoders[4]>-0.3 &&
            encoders[5]<0.3 && encoders[5]>-0.3)
    {
        printf("\niCub is already at home, Thread closing \n");

    }
    else
    {
        printf("\niCub is going to home position. Please wait...\n");
        for(int i =0; i<joints;i++)
        {
            while(encoders[i]>0.3 || encoders[i]<-0.3)
            {
                if (encoders[i]>0.3)
                    ivel->velocityMove(i,-5);
                else if(encoders[i]<-0.3)
                    ivel->velocityMove(i,5);
                iencs->getEncoders(encoders.data());
            }
        }
        printf("\niCub is now at home. Thread closing\n");
    }

    ivel->stop();
    ipos->stop();

    igaze->stopControl(); //This will make the eyes move to right corner
    igaze->restoreContext(startup_context_id);

    clientGaze->close();
    head->close();

    //delete head; // This will make the getfixationpoint not work the second time...
    //delete clientGaze;

    printf("\nQuitting the thread\n");
    printf("Type quit or Press 'Ctrl C' to quit the program \n ");

}


bool AutoMovModule::configure(ResourceFinder &rf)
{
    std::cout<<"Module configuration"<<endl;

    Property optNeck;
    optNeck.put("device", "remote_controlboard");
    optNeck.put("remote", "/icub/head");
    optNeck.put("local", "/AutonomousMotion/local/neck");

    neck = new PolyDriver;
    if (!neck->open(optNeck))
    {
        delete neck;
        return false;
    }

    // open the view
    neck->view(encs);
    neck->view(vel);
    neck->view(pos);

    if ( (!encs) || (!vel) || (!pos) )
    {
        printf("Cannot get interface to robot head\n");
        return false;
    }

    jnts = 0;

    encs->getAxes(&jnts);

    nencoders.resize(jnts);
    ntmp.resize(jnts);
    ncommands_position.resize(jnts);
    ncommands_velocity.resize(jnts);


    int j;
    for (j = 0; j < jnts; j++) {
        ntmp[j] = 1.0;
    }
    pos->setRefSpeeds(ntmp.data());

    thr=new LookAtLocation();
    if (!thr->start())
    {
        delete thr;
        return false;
    }

    handlerPort.open("/Autonomous/motion/command:i");
    Network::connect("/motion/input","/Autonomous/motion/command:i");
    //opening the port to send commands to the
    cmdPort.open("/Autonomous/motion/command:o");

    attach(handlerPort);
    attachTerminal();

    return true;
}


bool AutoMovModule::respond(const Bottle& command, Bottle& reply)
{
    fprintf(stderr, "Got something, echo is on\n");
    if (command.get(0).asString()=="end")
    {
        std::cout<<"ASKED TO END THE THREAD"<<endl;
        thr->setMode(STATE_END);
        return true;
    }
    else if (command.get(0).asString()=="quit")
    {
        std::cout<<"ASKED TO QUIT THE MODULE"<<endl;
        close();
        return false;
    }
    else if (command.get(0).asString()=="track")
    {
        //fprintf(stderr, "Switching to tracking mode\n");
        std::cout<<"Switching to tracking mode"<<endl;
        thr->track = true;
        thr->setMode(STATE_TRACK);
        return true;
    }
    else if (command.get(0).asString()=="idle")
    {
        //fprintf(stderr, "Idle\n");
        std::cout<<"Idle"<<endl;
        thr->setMode(STATE_IDLE);
        return true;
    }
    else if (command.get(0).asString()=="home")
    {
        //fprintf(stderr, "Going to home position\n");
        std::cout<<"Going to home position"<<endl;
        thr->setMode(STATE_HOME);
        //thr->gohome=true;
        return true;
    }
    else
    {
        reply=command;
        return true;
    }
}

bool AutoMovModule::interruptModule()
{
    cmdPort.interrupt();
    handlerPort.interrupt();
    return true;
}

bool AutoMovModule::close()
{
    printf("Ask thread to stop from Module close\n");
    if(thr->isRunning())
    {
        thr->stop();
        printf("Thread stopped\n");
    }
    else
        printf("THread was already stopped \n");
    delete thr;
    printf("Thread deleted\n");

    pos->stop();
    vel->stop();
    neck->close();
    //delete neck;
    printf("Neck Closed\n");

    cmdPort.close();
    handlerPort.close();

    return true;
}

double AutoMovModule::getPeriod()    { return 0.02/*1.0*/;  }
bool   AutoMovModule::updateModule()
{

    //if(thr->isRunning())
    {
        if(thr->state == STATE_HOME)
        {
            thr->track = false;
            //thr->interrupt();
            //thr->MotionPortLeft.lastRead();
            encs->getEncoders(nencoders.data());
            if     (nencoders[0]<0.3 && nencoders[0]>-0.3 &&
                    nencoders[1]<0.3 && nencoders[1]>-0.3 &&
                    nencoders[2]<0.3 && nencoders[2]>-0.3 &&
                    nencoders[3]<0.3 && nencoders[3]>-0.3 &&
                    nencoders[4]<0.3 && nencoders[4]>-0.3)
            {
                thr->gohome=false;
                thr->end=false;
                thr->track=false;
                thr->state=STATE_IDLE;
                printf("\niCub is at home \nGoing to STATE_IDLE\n");
                printf("\nType 'track' to run the code\n");
            }
            else
            {
                for(int i =0; i<jnts;i++)
                {
                    {

                        if (nencoders[i]>0.3)
                            vel->velocityMove(i,-5);
                        else if(nencoders[i]<-0.3)
                            vel->velocityMove(i,5);
                        encs->getEncoders(nencoders.data());
                    }
                }
                printf("\niCub is not at home \nPlease wait while it is going to home position\n");
            }
        }

        if(thr->state==STATE_END)
        {
            thr->threadRelease();
        }
    }
    if(thr->track == true)
    {
        thr->gohome = false;
        thr->end = false;
    }


    return true;
}

int main(int argc, char *argv[])
{
    putenv("GABBO_ROOT=/home/pramod/robot/GABOR_WITH_RECOGNITION/AutonomousVisionSystem20aug2011/root_ini");

    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("motion.ini");
    rf.configure("GABBO_ROOT",argc,argv);

    Network yarp;

    if (!yarp.checkNetwork())
    {
        printf("No yarp network, quitting\n");
        return false;
    }

    // we need to initialize the drivers list
    YARP_REGISTER_DEVICES(icubmod)

            AutoMovModule mod;

    mod.runModule(rf);

    printf("returned to main \n");

    return 0;
}


void LookAtLocation::saveLocation()
{
    igaze->getFixationPoint(x3dRead);

    xyzLocTemp.x = x3dRead[0];
    xyzLocTemp.y = x3dRead[1];
    xyzLocTemp.z = x3dRead[2];

    if(locations3DSave.size()<LOCATION_HISTORY)
    {
        locations3DSave.push_back(xyzLocTemp);
    }
    else
    {
        locations3DSave.insert(locations3DSave.begin(),xyzLocTemp);
        locations3DSave.pop_back();
    }

    igaze->getAngles(ang3dRead);

#if DISPLAY_VALUES
    std::cout<<"x value : "<<x3dRead[0]<<endl;
    std::cout<<"y value : "<<x3dRead[1]<<endl;
    std::cout<<"z value : "<<x3dRead[2]<<endl;

    std::cout<<"azimuth value : "<<ang3dRead[0]<<endl;
    std::cout<<"elevation value : "<<ang3dRead[1]<<endl;
    std::cout<<"vergence value : "<<ang3dRead[2]<<endl;
#endif

}

/*void LookAtLocation::saveImage()
{
    imageLeft = imagePortLeft.read();
    imageRight = imagePortRight.read();
    if (imageLeft!=NULL && imageRight!=NULL) { // check we actually got something
        frame_tempLeft = (IplImage*)imageLeft->getIplImage();
        frame_tempRight = (IplImage*)imageRight->getIplImage();
        frame1Left = cvCreateImage( cvSize(frame_tempLeft->width,  frame_tempLeft->height), frame_tempLeft->depth, 3 );
        cvCvtColor(frame_tempLeft,frame1Left, CV_RGB2BGR);
        frame1Left->origin=0;
        cvShowImage("cNameLeft",frame1Left);
        sprintf(imageFileNameLeft,"%s/imageLeft%04d.jpg",folderPath,imgIndex);
        std::cout<<"save left image file at "<<imageFileNameLeft<<endl;
        cvSaveImage(imageFileNameLeft,frame1Left);

        frame1Right = cvCreateImage( cvSize(frame_tempRight->width,  frame_tempRight->height), frame_tempRight->depth, 3 );
        cvCvtColor(frame_tempRight,frame1Right, CV_RGB2BGR);
        frame1Right->origin=0;
        //cvShowImage("cNameRight",frame1Right);
        sprintf(imageFileNameRight,"%s/imageRight%04d.jpg",folderPath,imgIndex);
        std::cout<<"save right image file at "<<imageFileNameRight<<endl;
        cvSaveImage(imageFileNameRight,frame1Right);
    }
}*/
