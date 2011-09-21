/*
 * RecordModule.h
 *
 *  Created on: May 26, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef _VISION_FILTER_MODULE_H_
#define _VISION_FILTER_MODULE_H_

//STD include
#include <string.h>

//Get all OS and signal processing YARP classes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

//Get opencv libraries
#include <opencv2/opencv.hpp>
using namespace cv;

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;

typedef enum {LEFT, RIGHT, BOTH} recordFlag; //What camera do you want acquire?

class VisionFilterModule : public yarp::os::Module
{
  private:

    //Module settings
    string moduleName;
    string handlerPortName;
    Port handlerPort; //a port to handle messages

    /*port name strings*/
    string inputPortNameLeft;
    string inputPortNameRight;

    /* thread parameters: they are pointers so that they refer to the original variables */
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageInLeft;
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageInRight;
    bool getImageLeft, getImageRight;

    /*Image support*/
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCub_leftImg;
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCub_rightImg;
	Mat imageLeft, imageRight;

  public:
    VisionFilterModule();
    virtual ~VisionFilterModule();
    double getPeriod();
    bool updateModule();
    bool respond(const Bottle& command, Bottle& reply);
    bool configure(yarp::os::ResourceFinder &rf);
    bool interruptModule();
    bool open(Searchable& config); //member to set the object up.
    bool close();
};

#endif
