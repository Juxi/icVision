/************************************************************
 * Copyright: (C) 2011 Juxi Leitner	                    	*
 * Author: Juxi Leitner <juxi.leitner@gmail.com> @ IDSIA.ch *
 *  based on code for the recordmodule by : Davide Migliore	*
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.*
 ************************************************************/

#ifndef PLAYMODULE_H_
#define PLAYMODULE_H_

//STD include
#include <string.h>

//Get all OS and signal processing YARP classes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

typedef enum {LEFT, RIGHT, BOTH} recordFlag; //What camera do you want acquire?

//Get opencv libraries
#include <opencv2/opencv.hpp>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;

class PlayModule : public yarp::os::Module
{
  private:

    //Module settings
    string moduleName;
    string handlerPortName;
    Port handlerPort; //a port to handle messages

    //Record settings
    recordFlag cam2play;
    int framecounter;
    string dir4left, dir4right;
    vector<int> params;

    /*port name strings*/
    string outputPortNameLeft;
    string outputPortNameRight;

    /* thread parameters: they are pointers so that they refer to the original variables */
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageOutLeft;
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageOutRight;

    bool getImageLeft, getImageRight;

    /*Image support*/
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *imgLeft;
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *imgRight;
    Mat imageLeft, imageRight;
	
    //Function to save image
    bool readImage(Mat& image2save, string directory, string camera, int framecounter);
	

  public:
    PlayModule();
    virtual ~PlayModule();
    double getPeriod();
    bool updateModule();
    bool respond(const Bottle& command, Bottle& reply);
    bool configure(yarp::os::ResourceFinder &rf);
    bool interruptModule();
    bool open(Searchable& config); //member to set the object up.
    bool close();
};

#endif
