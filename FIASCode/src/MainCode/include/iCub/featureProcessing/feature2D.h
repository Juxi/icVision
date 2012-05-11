#ifndef feature2D_H
#define feature2D_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <string>
#include <stdio.h>
#include <iostream>

#include "iCub/camera/displayiplimage.h"

#include "iCub/variables.h"

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGHARRIS 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class feature2D
{
public:

    //feature2D();

    //void feature2DInit(std::string name = "feature2D",std::string side="side");//,std::string input="input");
    feature2D(std::string name = "feature2D",std::string side="side");//,std::string input="input");
    ~feature2D();

    void setParam(unsigned int cornerCount=1000, double qualityLevel=0.01, double minDistance=3.0);

    void execute();
    void HarrisExecute();
    void FastFeatureExecute();
    void StarFeatureExecute();
    void MserFeatureExecute();
    void SiftFeatureExecute();
    void SurfFeatureExecute();

private:

    //class names
    string nameclass;
    string namesideclass;
    string	nameinput;

    /*		READ AN IMAGE	*/
    //port names
    string nameImageInPort;
    string nameSenderImagePort;
    BufferedPort<ImageOf<PixelRgb> > ReceivingImagePort;
    IplImage* iplimagein;
    ImageOf<PixelRgb> *yarpimagein;
    bool receiveimage;

    /*		SEND HARRIS	*/
    //port names
    string nameHarrisOutPort;
    //create a port to send images
    BufferedPort<Bottle> SendingHarrisPort;

    /*		CALCULATE HARRIS	*/
    IplImage* grayimg;
    IplImage* mp_eig_image;
    IplImage* mp_temp_image;
    CvPoint2D32f* mp_corners;
    unsigned int m_cornerCount; //maximum number of corners returned
    double m_qualityLevel;  //threshold for corners
    double m_minDistance; // minimum distance between corners
    int m_block_size;
    int m_use_harris;
    double m_k;

    /*Calculate FAST*/

    /*Calculate STAR*/


    /*		SEND A GRAY IMAGE	*/
    string nameGrImageOutPort;
    BufferedPort<ImageOf<PixelMono> > SendingGrImagePort;
    IplImage *iplgrimageout;
    ImageOf<PixelMono> yarpgrimageout;


    /*		DISPLAY HARRIS	*/
#if DEBUGHARRIS
    string nameImageOutPort;
    DisplayIplImage *dispImage;
    BufferedPort<ImageOf<PixelRgb> > SendingImagePort;
    ImageOf<PixelRgb> yarpimageout;
#endif

#define HARRIS_CORNER 1
#define FAST_FEATURE 2
#define STAR_FEATURE 3
#define MSER_FEATURE 4
#define SIFT_FEATURE 5
#define SURF_FEATURE 6



};


#endif
