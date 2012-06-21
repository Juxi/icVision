#ifndef DisplayIplImage_H
#define DisplayIplImage_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGDISPLAY 0

#define SAVEIPLIMAGE 0
#define DISPLAYIPLIMAGE 1

using namespace std;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class DisplayIplImage
{
public:

    DisplayIplImage(std::string name="DisplayIplImage",std::string side="side",std::string input="input", int x=0, int y=0);
    ~DisplayIplImage();

    void execute();
    void createWindow();
    void destroyWindow();

private:

    //class names
    string nameclass;
    string namesideclass;
    string	nameinput;

    /*		READ AN IMAGE	*/
    string nameImageInPort;
    string nameSenderImagePort;
    BufferedPort<ImageOf<PixelRgb> > ReceivingImagePort;
    IplImage* iplimagein;
    ImageOf<PixelRgb> *yarpimagein;
    bool receiveimage;

    //yarpview
    BufferedPort<ImageOf<PixelRgb> > DisplayImageYarpViewPort;
    ImageOf<PixelRgb> DisplayImageYarpImageOut;
    string yarpviewName;
    IplImage* iplimageinyarpview1;

#if SAVEIPLIMAGE
    int number;
    char* svName;
    char saveName[250];
    char FolderImagePath[200];
    char command[250];
#endif

    /*		DISPLAY	*/
    int posx;
    int posy;
    bool createwindow;

};


#endif
