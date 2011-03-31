/*
 * VisionModule.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: migliore
 */

#include "VisionModule.h"

#include <yarp/os/Network.h>
#include <yarp/math/Math.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace iCub::iKin;


VisionModule::VisionModule()
{
  // TODO Auto-generated constructor stub
 // moduleName = "TestingIMCLEVERVISION";

}


VisionModule::~VisionModule()
{
  // TODO Auto-generated destructor stub
}

double VisionModule::getPeriod()
{
   return 1; //module periodicity (seconds)
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool VisionModule::updateModule()
{

  count++;
  //printf("[%d] updateModule\n", count);
  cout<<"["<<count<<"]"<< " step... "<<endl;

  //Use YARP port to get images
  getImageLeft =  ( imageInLeft.getInputCount() > 0 );
  getImageRight = ( imageInRight.getInputCount() > 0 );

  if ( getImageLeft + getImageRight < 1){
    Time::delay(0.1);
    cout<<"There are no images"<<endl;
  }
  else{
    cout<<"There are images"<<endl;
    //Time::delay(0.3);
  }

  if (Bottle *head=inputHeadPort.read(false))
  {
     //get data and convert from degrees to radians
     head0=head->get(0).asDouble()*M_PI/180;
     head1=head->get(1).asDouble()*M_PI/180;
     head2=head->get(2).asDouble()*M_PI/180;
     head3=head->get(3).asDouble()*M_PI/180;
     head4=head->get(4).asDouble()*M_PI/180;
     head5=head->get(5).asDouble()*M_PI/180;
     receivedHead = true;
  }

  if (Bottle *torso=inputTorsoPort.read(false))
  {
     //get data and convert from degrees to radians
     torso0=torso->get(0).asDouble()*M_PI/180;
     torso1=torso->get(1).asDouble()*M_PI/180;
     torso2=torso->get(2).asDouble()*M_PI/180;
     receivedTorso = true;
  }

  //Estimate rototranslation camera->world model
  if (receivedHead && receivedTorso)
   {

      v[0]=torso2;
      v[1]=torso1;
      v[2]=torso0;
      v[3]=head0;
      v[4]=head1;
      v[5]=head2;
      v[6]=head3;

      //it is a LEFT eye
      v[7]=head4+head5/2;

      //Matrix transformation;
      transformation_left = chainEye_left.getH(v);

      //it is a RIGHT eye
      v[7]=head4-head5/2;

      //Matrix transformation;
      transformation_right = chainEye_right.getH(v);

      RT_left = Mat(4,4, CV_64FC1, transformation_left.data());
      RT_right = Mat(4,4, CV_64FC1, transformation_right.data());

      RT_left.convertTo(RT_left, CV_32FC1);
      RT_right.convertTo(RT_right, CV_32FC1);

      vision->setUpCamera2World(RT_left, RT_right);

      //TODO Test if it is possible estimate the F matrix from encoder readings
//      Mat RT_w2r = RT_right.inv();
// //    Mat RT_w2l = RT_left.inv();
//
//       Mat RT_l2r = RT_w2r*RT_left;
//
//       cout<<RT_l2r<<endl;

   }
  else{
      //We have to compute the RT Matrix
  }

  if ( getImageLeft + getImageRight > 1){

    iCubleft = imageInLeft.read();  // read an image
    iCubright = imageInRight.read();  // read an image

    imageLeft = Mat((IplImage*)iCubleft->getIplImage(),false);
    imageRight = Mat((IplImage*)iCubright->getIplImage(),false);

    vision->setUpImageStereoImages(imageLeft, imageRight);
    cout<<"New objects detected: "<<vision->detectObjects(object_list)<<endl;
    cout<<"Current number of objects: "<<object_list.size()<<endl;

    outputImageLeft = vision->getOutputImage("left");
    outputImageRight = vision->getOutputImage("right");



    if(!outputImageLeft.empty() && !outputImageRight.empty()){
    //Send output images ... just place to outputImageLeft or outputImageRight what do you want to see
      ImageOf<PixelBgr> &imageLeftTmp = imageOutLeft.prepare();
      ImageOf<PixelBgr> &imageRightTmp = imageOutRight.prepare();

      imageLeftTmp.resize(outputImageLeft.cols,outputImageLeft.rows);
      imageRightTmp.resize(outputImageRight.cols,outputImageRight.rows);

      IplImage image_iplLeftimage = (IplImage) outputImageLeft;
      IplImage image_iplRightimage = (IplImage) outputImageRight;

      cvCopy( &image_iplLeftimage, (IplImage *) imageLeftTmp.getIplImage());
      cvCopy( &image_iplRightimage, (IplImage *) imageRightTmp.getIplImage());

      imageOutLeft.write();
      imageOutRight.write();
    }




    for(uint i=0; i<object_list.size(); i++){

        //Send object position on the port
        Bottle output_message,answer;
        ostringstream buffer2send;
//        output_message.clear();
//        answer.clear();
//        buffer2send.clear();
//        flush(buffer2send);

        string command2send = object_list[i].getSimCommand(POSE, isICubSim);
        cout<<"Command transmitted: "<<endl;
        cout<<command2send<<endl;
        buffer2send<<command2send;
        output_message.fromString(buffer2send.str().c_str());
        moduleOutput.write(output_message, answer);
        cout<<"Answer : "<<answer.toString()<<endl;

//        output_message.clear();
//        answer.clear();
//        flush(buffer2send);
//
//        cout<<"Command transmitted: "<<endl;
//        cout<<object_list[i].getSimCommand(ORIENTATION, isICubSim)<<endl;
//        buffer2send<<object_list[i].getSimCommand(ORIENTATION, isICubSim);
//        output_message.fromString(buffer2send.str().c_str());
//        moduleOutput.write(output_message, answer);
//        cout<<"Answer : "<<answer.toString()<<endl;

    }


  }



   return true;
}

/*
* Message handler. Just echo all received messages.
*/
bool VisionModule::respond(const Bottle& command, Bottle& reply)
{
   cout<<"Got something, echo is on"<<endl;
   if (command.get(0).asString()=="quit" || command.get(0).asString()=="exit"){
       close();
       return false;
   }
   else{

     reply.clear();
     reply.addString("The command is not valid");

   }
    // reply=output_message; //command;
   return true;
}

/*
* Configure function. Receive a previously initialized
* resource finder object. Use it to configure your module.
* Open port and attach it to message handler.
*/
bool VisionModule::configure(yarp::os::ResourceFinder &rf)
{
  count=0;
  //handlerPort.open("/IMCLEVERVision");
  //attach(handlerPort);
  //attachTerminal();

  isICubSim = true;

 /* Process all parameters from both command-line and .ini file */

  moduleName   = "IMCLEVERVision"; //(rf.find("moduleName")).toString();
  /*
   * attach a port of the same name as the module (prefixed with a /) to the module
   * so that messages received from the port are redirected to the respond method
   */

  cout<< moduleName <<endl;
  handlerPortName =  "/";
  handlerPortName += getName();         // use getName() rather than a literal

  if (!handlerPort.open(handlerPortName.c_str())) {
     cout << getName() << ": Unable to open port " << handlerPortName << endl;
     return false;
  }

  attach(handlerPort);                  // attach to port



  //Number of Objects TODO load information from a database
  object_list.clear();

  //set-up input/output ports
  inputPortNameLeft = "/" + moduleName + "/left/image:i";
   if(! imageInLeft.open( inputPortNameLeft.c_str() )){
     return false;
   }

   inputPortNameRight = "/" + moduleName + "/right/image:i";
   if(! imageInRight.open( inputPortNameRight.c_str() )){
     return false;
   }

   outputPortNameLeft = "/" + moduleName + "/left/image:o";
   if(! imageOutLeft.open( outputPortNameLeft.c_str() )){
     return false;
   }

   outputPortNameRight = "/" + moduleName + "/right/image:o";
   if(! imageOutRight.open( outputPortNameRight.c_str() )){
     return false;
   }

   outputPortName = "/" + moduleName + "/output:o";
   if(! moduleOutput.open( outputPortName.c_str() )){
     return false;
   }

   inputHeadPortName = "/" + moduleName + "/head:i";
     if(! inputHeadPort.open( inputHeadPortName.c_str() )){
       return false;

   }

   inputTorsoPortName = "/" + moduleName + "/torso:i";
    if(! inputTorsoPort.open( inputTorsoPortName.c_str() )){
      return false;
    }

    /* Eye initialization */
    eye_left = new iCubEye("left_v2");
    eye_right = new iCubEye("right_v2");

    eye_left->releaseLink(0);
    eye_left->releaseLink(1);
    eye_left->releaseLink(2);
    cout<<"Left eye Degrees Of Freedom = "<<eye_left->getDOF()<<endl;
    eye_right->releaseLink(0);
    eye_right->releaseLink(1);
    eye_right->releaseLink(2);
    cout<<"Left eye Degrees Of Freedom = "<<eye_right->getDOF()<<endl;

    chainEye_left=*(eye_left->asChain());
    chainEye_right=*(eye_right->asChain());
    receivedHead = false;
    receivedTorso = false;

    v.resize(8);
    eyeTargetPos.resize(4);
    rootTargetPos.resize(4);
    transformation_left.resize(4,4);
    transformation_right.resize(4,4);
    rotation4obj.resize(4,4);
    rootRTargetPos.resize(4,4);
    root2simtransformation.resize(4,4);

    root2simtransformation[0][0] = 0; root2simtransformation[0][1] =-1; root2simtransformation[0][2] = 0; root2simtransformation[0][3] = 0;
    root2simtransformation[1][0] = 0; root2simtransformation[1][1] = 0; root2simtransformation[1][2] = 1; root2simtransformation[1][3] = 0.5484;
    root2simtransformation[2][0] =-1; root2simtransformation[2][1] = 0; root2simtransformation[2][2] = 0; root2simtransformation[2][3] = -0.04;
    root2simtransformation[3][0] = 0; root2simtransformation[3][1] = 0; root2simtransformation[3][2] = 0; root2simtransformation[3][3] = 1;
//    root2simtransformation[0][0] = 0; root2simtransformation[0][1] =-1; root2simtransformation[0][2] = 0; root2simtransformation[0][3] = 0;
//    root2simtransformation[1][0] = 0; root2simtransformation[1][1] = 0; root2simtransformation[1][2] = 1; root2simtransformation[1][3] = 0;
//    root2simtransformation[2][0] =-1; root2simtransformation[2][1] = 0; root2simtransformation[2][2] = 0; root2simtransformation[2][3] = 0;
//    root2simtransformation[3][0] = 0; root2simtransformation[3][1] = 0; root2simtransformation[3][2] = 0; root2simtransformation[3][3] = 1;

   // RT = [ 0 -1 0 0; 0 0 1 0.5484; -1 0 0 -0.04; 0 0 0 1 ]

    /*Vision utils*/
    vision = new CVUtils();

   return true ;      // let the RFModule know everything went well
}

/*
* Interrupt function.
*/
bool VisionModule::interruptModule()
{
   cout<<"Interrupting your module, for port cleanup"<<endl;
   imageInLeft.interrupt();
   imageInRight.interrupt();
   inputHeadPort.interrupt();
   inputTorsoPort.interrupt();
   handlerPort.interrupt();

   delete vision;

   return true;
}

/*
* Interrupt function.
*/
bool VisionModule::open(Searchable& config)
{

  //TODO change the config setting for customizable configuration (ref Taio code)

  /* prepare and configure the resource finder */
   ResourceFinder rf;
   rf.setVerbose( true );
   rf.setDefaultConfigFile( "IMCLEVERVision.ini" ); //overridden by --from parameter
   rf.setDefaultContext( "IMCLEVERVision/conf" );   //overridden by --context parameter
   rf.configure( "ICUB_ROOT", 0, NULL );
   //rf.configure( "../home/migliore/Dropbox/Projects/YarpObjectLocalization/bin/", 0, NULL );

   cout<<"Start module"<<endl;


   if( !configure(rf) ){
     return false;
   }
   return true;
}


/*
* Close function, to perform cleanup.
*/
bool VisionModule::close()
{
   cout<<"Calling close function\n";
   imageInLeft.close();
   imageInRight.close();
   inputHeadPort.close();
   inputTorsoPort.close();
   handlerPort.close();

   delete vision;

   return true;
}
