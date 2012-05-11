#ifndef DisplayIplImageGray_H
#define DisplayIplImageGray_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGDISPLAY 0

#define SAVEGRAYIMAGE 0
#define DISPLAYGRAYIMAGE 1

using namespace std;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class DisplayIplImageGray
{
public:

    DisplayIplImageGray(std::string name="DisplayIplImageGray",std::string side="side",std::string input="input", int x=0, int y=0);
    ~DisplayIplImageGray();

    void execute();
    void createWindow();
    void destroyWindow();

private:

    //class names
    string nameclass;
    string namesideclass;
    string	nameinput;

    /*		READ AN IMAGE	*/
    //port names
    string nameImageInPort;
    string nameSenderImagePort;
    BufferedPort<ImageOf<PixelMono> > ReceivingImagePort;
    IplImage* iplimagein;
    ImageOf<PixelMono> *yarpimagein;
    bool receiveimage;

    //yarpview
    BufferedPort<ImageOf<PixelRgb> > DisplayGrayImageYarpViewPort;
    ImageOf<PixelRgb> DisplayGrayImageYarpImageOut;
    string yarpviewName;
    IplImage* iplimageinyarpview1;


#if SAVEGRAYIMAGE
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
