//From tutorial
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Time.h>
#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/GazeControl.h>
#include <yarp/dev/PolyDriver.h>

#include <gsl/gsl_math.h>
#include <deque>

//Our code

/*#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>*/

#include <stdio.h>
#include <string>
#include <iostream>

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#define DEBUG 1
#define STATE_END           3
#define STATE_HOME          2
#define STATE_TRACK         1
#define STATE_IDLE          0

#define LOCATION_HISTORY    3 //How many locations in teh past you want to inhibit

#define AZIMUTH_RANGE 60 //when robot moves outside this angle it comes back. This is to avoid objects out of the view you wanted.
#define ELEVATION_RANGE 50 //same thing as above but in vertical direction
#define BOUNDARY_CHECK 1 // do you want to do the boundary check?
#define DISPLAY_VALUES 1 //Display motor values
#define RECOIL_FACTOR 0.66 //When it goes outside the boundary, by howmuch amount you want to come back into your field of view.
#define BIND_NECK_ROLL 0

char imageFileNameLeft[200];
char imageFileNameRight[200];

YARP_DECLARE_DEVICES(icubmod)

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;

class LookAtLocation : public yarp::os::Thread //RateThread
{

    PolyDriver *clientGaze;
    PolyDriver *head;

    IGazeControl *igaze;

    IPositionControl *ipos;
    IVelocityControl *ivel;
    IEncoders        *iencs;

    yarp::sig::Vector setpoints;
    yarp::sig::Vector tmp;
    yarp::sig::Vector encoders;
    yarp::sig::Vector commands_position;
    yarp::sig::Vector commands_velocity;
    yarp::sig::Vector checkpos;

    BufferedPort<Bottle> MotionPortLeft;
    BufferedPort<Bottle> MotionPortRight;
    Bottle *targetIndL;
    Bottle *targetIndR;

    BufferedPort<Bottle> commandInPort;
    Bottle *commandText;
    string commandContent;

    BufferedPort<Bottle> PixelLocationsOutPort;

    /*SEND POINT TO MOTION	*/
    BufferedPort<Bottle> BalisticDoneOutPort;
    string nameBalisticDoneOutPort;

    string TypeL;
    string TypeR;

    double xL;
    double yL;

    double xR;
    double yR;

    double confL;
    double confR;

    double distxL;
    double distyL;

    double distxR;
    double distyR;

    double distL;
    double distR;

    double x;
    double y;

    int joints;

    yarp::sig::Vector pxl, cl, pxr, cr;

    double velocityFactor;

    double vx;
    double vy;
    double vz;  

    BufferedPort<ImageOf<PixelRgb> > imagePortLeft;
    BufferedPort<ImageOf<PixelRgb> > imagePortRight;

    /*ImageOf<PixelRgb> *imageLeft;
    ImageOf<PixelRgb> *imageRight;
    IplImage *frame_tempLeft;
    IplImage *frame_tempRight;
    IplImage *frame1Left;
    IplImage *frame1Right;*/

    yarp::sig::Vector x3d;
    yarp::sig::Vector x3dRead;
    yarp::sig::Vector ang3d;
    yarp::sig::Vector ang3dRead;
    yarp::sig::Vector pixelLeft;
    bool success2Dpixel;

    typedef struct xyzLoc
    {
        double x;
        double y;
        double z;
    }xyzLoc;
    xyzLoc xyzLocTemp;

    std::vector<xyzLoc> locations3DSave;
    int startup_context_id;

    int binsAzimuth;
    int binsElevation;
    int* visitedAngleHistogram;
    bool setbool;int counter;

public:

    bool end;
    int state;
    bool gohome;
    bool track;


    bool threadInit();
    void interrupt();
    void threadRelease();
    void run();
    void setMode(int mode);
    void onStop();
    void saveLocation();

    // void saveImage();

};



class AutoMovModule: public RFModule
{
    LookAtLocation *thr;
protected:

    PolyDriver *neck;

    IPositionControl *pos;
    IVelocityControl *vel;
    IEncoders        *encs;

    yarp::sig::Vector ntmp;
    yarp::sig::Vector nencoders;
    yarp::sig::Vector ncommands_position;
    yarp::sig::Vector ncommands_velocity;

    int jnts;

public:
    Port cmdPort;
    Port handlerPort;
    virtual bool configure(ResourceFinder &rf);
    bool respond(const Bottle& command, Bottle& reply);
    virtual bool close();
    virtual double getPeriod();
    virtual bool   updateModule();
    bool interruptModule();

};

