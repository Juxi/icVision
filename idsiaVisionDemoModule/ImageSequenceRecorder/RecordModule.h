/*
 * RecordModule.h
 *
 *  Created on: May 26, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef RECORDMODULE_H_
#define RECORDMODULE_H_

//STD include
#include <string.h>

//Get all OS and signal processing YARP classes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

//Get opencv libraries
#include <opencv2/opencv.hpp>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;


typedef enum {LEFT, RIGHT, BOTH} recordFlag; //What camera do you want acquire?


class RecordModule : public yarp::os::Module
{
  private:

    //Module settings
    string moduleName;
    string handlerPortName;
    Port handlerPort; //a port to handle messages

    //Record settings
    recordFlag cam2save;
    int framecounter;
    string dir4left, dir4right;
    vector<int> params;

    /*port name strings*/
    string inputPortNameLeft;
    string inputPortNameRight;

    /* thread parameters: they are pointers so that they refer to the original variables */
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageInLeft;
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageInRight;
    bool getImageLeft, getImageRight;

    /*Image support*/
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCubleft;
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCubright;
    Mat imageLeft, imageRight;

    //Function to save image
    bool saveImage(Mat& image2save, string directory, string camera, int framecounter, vector<int> & params);

  public:
    RecordModule();
    virtual ~RecordModule();
    double getPeriod();
    bool updateModule();
    bool respond(const Bottle& command, Bottle& reply);
    bool configure(yarp::os::ResourceFinder &rf);
    bool interruptModule();
    bool open(Searchable& config); //member to set the object up.
    bool close();
};

#endif /* RECORDMODULE_H_ */
