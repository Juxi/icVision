/*
 * VisionModule.h
 *
 *  Created on: Nov 17, 2010
 *      Author: migliore
 */

#ifndef VISIONMODULE_H_
#define VISIONMODULE_H_

//Get all OS and signal processing YARP classes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

//#include <iCub/ctrl/math.h>

#include <iCub/iKin/iKinFwd.h>
#include <opencv2/opencv.hpp>

#include "WorldObject.h"
#include "CVUtils.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;

class VisionModule :public Module
{
  private:
    /* module parameters */
    string moduleName;
    string handlerPortName;

    Port handlerPort; //a port to handle messages
    int count;

    /*Simulator selection*/
    bool isICubSim;
    bool sendData2Sim;

    /*port name strings*/
    string inputPortNameLeft;
    string inputPortNameRight;
    string outputPortNameLeft;
    string outputPortNameRight;
    string outputPortName;
    string inputHeadPortName;
    string inputTorsoPortName;

    /* thread parameters: they are pointers so that they refer to the original variables */
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageInLeft;
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageInRight;
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageOutLeft;
    BufferedPort<ImageOf<yarp::sig::PixelBgr> >  imageOutRight;
    BufferedPort<yarp::os::Bottle> inputHeadPort;
    BufferedPort<yarp::os::Bottle> inputTorsoPort;
    bool getImageLeft, getImageRight;
    bool receivedHead;
    bool receivedTorso;

    //output Module
    Port moduleOutput;

    /* to estimate obj position in the world*/
    iCub::iKin::iCubEye   *eye_left, *eye_right;
    iCub::iKin::iKinChain  chainEye_left, chainEye_right;

    yarp::sig::Vector v;
    yarp::sig::Matrix transformation_left, transformation_right;
    yarp::sig::Matrix rotation4obj;
    yarp::sig::Vector eyeTargetPos;
    yarp::sig::Vector rootTargetPos;
    yarp::sig::Matrix rootRTargetPos;
    yarp::sig::Matrix root2simtransformation;

    Mat RT_left, RT_right;

    double head0,head1,head2,head3,head4,head5;
    double torso0,torso1,torso2;
    double targetPosInX,targetPosInY,targetPosInZ,targetPosInGood;

    /*Image support*/
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCubleft;
    yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCubright;
    Mat imageLeft, imageRight;
    Mat outputImageLeft, outputImageRight;

    /*Vision*/
    CVUtils* vision;

    /*Object list*/
    int objId;
    vector<WorldObject> object_list;

  public:
    VisionModule();
    double getPeriod();
    bool updateModule();
    bool respond(const Bottle& command, Bottle& reply);
    bool configure(yarp::os::ResourceFinder &rf);
    bool interruptModule();
    bool open(Searchable& config); //member to set the object up.
    bool close();

    bool created;

    virtual
    ~VisionModule();

  private:

};


#endif /* VISIONMODULE_H_ */
