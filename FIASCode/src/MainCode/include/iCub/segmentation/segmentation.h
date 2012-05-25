#ifndef Segmentation_H
#define Segmentation_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include "iCub/variables.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGSegmentation 1
#define DISPLAYCLASS 1
#define HORDISPTHRES 4.1


#define REMOVEHARRISNEAROBJ 1 

#define STEPBYSTEPCLASS 0
#define DISPLAYINHIBITION 1

#define DISPLAYCLASSIMAGE 1
#define SAVECLASSIMAGE 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class Segmentation
{
public:

    Segmentation(std::string name = "Segmentation");

    ~Segmentation();

    int* segmentation();
    void VisitedLocationBlobDisplay(int iterationLocation,int LocX, int LocY);
    int* segmentationInhibition();

    void segmentObject(CvPoint PointOL,bool zerodisp);
    int* segment(bool zerodisp);
    void displayObject(int objectCount);

    void setSelectHarris();
    void inhibitSamplesLocationHistory();
    void sendInhibitionToSaliency();
    void releaseVariablesSelectHarris();
    void displayObject();
    void saveFeatures();

    void initializefeat();

    void readfromport();

    void saveDatabase();
    void readDatabase();

    void createWindow();
    void destroyWindow();


    int widthVote, heightVote, objectVote;
    int objLocX, objLocY;

    int numberfeatures;

    int newfeatures;
    vector<int> learnfeat;
    vector<int> checklearn;


    CvPoint WinnerL;


    bool stopupdate;
    bool continuelearning;

    CvPoint2D32f meanObjectLeft;
    CvPoint2D32f meanObjectRight;

    bool fileSave;
    int numberPoses;
    int objectCount;

private:

    string nameclass;

    /*		READ LEFT IMAGE	*/
    string nameImageLeftInPort;
    string nameSenderImageLeftPort;
    BufferedPort<ImageOf<PixelRgb> > ReceivingImageLeftPort;
    IplImage* iplimageleftin;
    ImageOf<PixelRgb> *yarpimageleftin;
    bool receiveimageleft;

    /*		READ RIGHT IMAGE	*/
    string nameImageRightInPort;
    string nameSenderImageRightPort;
    BufferedPort<ImageOf<PixelRgb> > ReceivingImageRightPort;
    IplImage* iplimagerightin;
    ImageOf<PixelRgb> *yarpimagerightin;
    bool receiveimageright;

    /*		READ HARRIS	LEFT	*/
    string nameHarrisLeftInPort;
    string nameSenderHarrisLeftPort;
    BufferedPort<Bottle> ReceivingHarrisLeftPort;
    bool receiveharrisleft;
    Bottle *HarrisLeftIn;
    unsigned int m_cornerCountLeft;
    //maximum number of corners returned
    CvPoint2D32f* mp_cornersleft;

    /*		READ HARRIS RIGHT	*/
    string nameHarrisRightInPort;
    string nameSenderHarrisRightPort;
    BufferedPort<Bottle> ReceivingHarrisRightPort;
    bool receiveharrisright;
    Bottle *HarrisRightIn;
    unsigned int m_cornerCountRight; //maximum number of corners returned
    CvPoint2D32f* mp_cornersright;

    /*		READ GABOR	LEFT	*/
    string nameGaborLeftInPort;
    string nameSenderGaborLeftPort;
    BufferedPort<Bottle> ReceivingGaborLeftPort;
    bool receivegaborleft;
    Bottle *GaborLeftIn;
    CvMat* m_descriptorleft;
    unsigned int sizeleft;
    unsigned int descwidthleft;

    /*		READ GABOR RIGHT	*/
    string nameGaborRightInPort;
    string nameSenderGaborRightPort;
    BufferedPort<Bottle> ReceivingGaborRightPort;
    bool receivegaborright;
    Bottle *GaborRightIn;
    CvMat* m_descriptorright;
    unsigned int sizeright;
    unsigned int descwidthright;

    /*RECEIVE PIXEL POINTS FROM MOTION*/
    BufferedPort<Bottle> PixelLocationsInPort;
    Bottle *PixelLocationsData;
    typedef struct pixelLocLeft
    {
        double pxl;
        double pxr;
    }pixelLocLeft;
    pixelLocLeft pixelLocTemp;
    vector<pixelLocLeft> pixelLocFromMotion;

    //for parallelization
    string nameInhibitHarrisOutPort;
    BufferedPort<Bottle> InhibitHarrisOutPort;
    vector<int> selectHarris;
    int countPixelsFromMotion;

    //yarpview
    BufferedPort<ImageOf<PixelRgb> > SegmentationYarpViewPort1;
    BufferedPort<ImageOf<PixelRgb> > SegmentationYarpViewPort2;
    BufferedPort<ImageOf<PixelRgb> > SegmentationYarpViewPort3;
    BufferedPort<ImageOf<PixelRgb> > SegmentationYarpViewPort4;
    ImageOf<PixelRgb> SegmentationYarpImageOut1;
    ImageOf<PixelRgb> SegmentationYarpImageOut2;
    ImageOf<PixelRgb> SegmentationYarpImageOut3;
    ImageOf<PixelRgb> SegmentationYarpImageOut4;
    IplImage* iplimageinyarpview1;
    IplImage* iplimageinyarpview2;
    IplImage* iplimageinyarpview3;
    IplImage* iplimageinyarpview4;

    IplImage* salimg;
    IplImage* blobsAllObjects;
    IplImage* blobsAllVisitedLocations;
    IplImage* votingImage;
    IplImage* featureImage;
    IplImage* featureImageTemp;
    IplImage* meanDetectionImg;
    IplImage* objectImage;


    unsigned int sizeGaborLeft;
    unsigned int widthGaborLeft;
    unsigned int heightRegionLeft;
    unsigned int widthRegionLeft;

    float* leftdata;
    float* rightdata;
    unsigned int   stepleft;
    unsigned int   stepright;
    CvPoint alreadyPresent;
    CvFont font;
    int counterSave;
    int countFocussing;

    int objectPresence;

    int* array5;

    int CountForMean;

    /*		SEND WINNER POINT	*/
    CvPoint winnerPosL;
    CvSize winnerSizeL;
    CvRect winnerRectL;
    int reliabilityL;
    CvSize imageSizeL;
    CvPoint winnerPosR;
    CvSize winnerSizeR;
    CvRect winnerRectR;
    int reliabilityR;
    CvSize imageSizeR;
    string namePointLeftOutPort;
    BufferedPort<Bottle> SendingPointLeftPort;
    string namePointRightOutPort;
    BufferedPort<Bottle> SendingPointRightPort;

    /*CvPoint objectLocations[NO_OF_OBJECTS];
    bool objectFound[NO_OF_OBJECTS];*/

    bool createwindow;

    string nameVisitedLocationBlobs;
    string nameTestWhenFocussed;
    string nameObjectsDisplay;

    FILE *fp1;
    FILE *fp2;

    //Display objects
    IplImage *imageObjectResize[OBJECT_LIMIT];
    int poseIndex[OBJECT_LIMIT];
    IplImage *imageObjects;


/*#if SAVECLASSIMAGE
    char* svName1;
    char saveName1[250];
    char* svName2;
    char saveName2[250];
    char* svName3;
    char saveName3[250];
    char* svName4;
    char saveName4[250];
    char FolderImagePath[200];
    char command[250];
    char fileSavePath[250];
#endif*/

};

/*static CvScalar colors[] =
{
    {{0,0,255}},
    {{0,128,255}},
    {{0,255,255}},
    {{0,255,0}},
    {{255,128,0}},
    {{255,255,0}},
    {{255,0,0}},
    {{255,0,255}},
    {{255,255,255}},

};*/
static CvScalar colors[14] = { 			// in RGB order BGR
                                        {{   0,   0, 128 }},	// deep red
                                        {{   0, 128,   0 }},	// deep green
                                        {{ 128,   0, 128 }},	// deep violet
                                        {{   0,   0, 255 }},	// red
                                        {{   0, 255,   0 }},	// green
                                        {{   0, 165, 255 }},	// orange
                                        {{   0, 255, 255 }},	// yellow
                                        {{ 128,   0,   0 }},	// deep blu
                                        {{ 255,   0,   0 }},	// blu
                                        {{ 255,   0, 255 }},	// violet
                                        {{ 128, 128,   0 }},	// deep light blu
                                        {{ 255, 255,   0 }},	// light blu
                                        {{ 128, 128, 128 }},	// grey
                                        {{ 255, 255, 255 }},	// white
                     };

#endif
