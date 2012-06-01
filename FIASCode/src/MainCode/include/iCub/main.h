#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include "variables.h"

#include "camera/camera.h"
#include "camera/cameraicub.h"
#include "camera/displayiplimage.h"
#include "featureProcessing/feature2D.h"
#include "featureProcessing/gabordescriptor.h"
#include "featureProcessing/detectobject.h"
#include "camera/displaymatching.h"
#include "saliency/saliency.h"
#include "segmentation/segmentation.h"

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

using namespace std;

using namespace yarp::os;
using namespace yarp::sig;

#define DEBUGMAIN 1
#define STEPBYSTEPMAIN 0

#define LIMITEDTIME 1
#define TIMELIMIT 100
#define SAMPLERATE 1.0 //10 per second

#define COUNTINHIB 1
#define COUNTFRAMES 1

#define SALIENCYMAP 1
#define READIFBALLISTICDONE 2
#define INITIALIZECUEINTEGRATION 3
#define CUEINTEGRATION 4
#define SEGMENTOBJECT 5
#define DORECOGNITION 6

char pathResults[250];
FILE *fileptr;

bool checkTimeUp();
bool checkClock();
int checkTime();
void sendcommand(string command);
void sendtomotion(double pointSalLeftX,double pointSalLeftY,double pointSalRightX,double pointSalRightY, char *type);
bool waitMotionDone();
void runFeatures();

int decide = 1;

double tstart, tstop, ttime;
double tstartbal, tstopbal, ttimebal;
double starttimerprogram, stoptimerprogram, timeprogram;
double starttimerobject, stoptimerobject, timeobject;

BufferedPort<Bottle> commandLocationPort;

BufferedPort<Bottle> BalisticDoneInPort;
Bottle *BalisticDone;
string Done;

BufferedPort<Bottle> MotionPortLeftPort;
BufferedPort<Bottle> MotionPortRightPort;

double ang1,ang2,ang3;

CameraiCub camicubLeft("Camera","left","main");
CameraiCub camicubRight("Camera","right","main");
#if DEBUGMAIN
DisplayIplImage dispImageLeft("DisplayImage","left","Camera_left",0,0);
DisplayIplImage dispImageRight("DisplayImage","right","Camera_right",320,0);
#endif
feature2D featureLeft("Harris","left");
feature2D featureRight("Harris","right");

GaborDescriptor GaborLeft("Gabor","left","Harris_left");
GaborDescriptor GaborRight("Gabor","right","Harris_right");

DetectObject	Match("Match");

Saliency		Sal("Saliency");

Segmentation	SegmentObj("Segmentation");
#if DEBUGMAIN
    DisplayMatching	DisplayMatch("DisplayMatch","Match",0,240);
#endif
    int numbersave = 0;

